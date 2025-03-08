#include "VarGatewayTransformation.h"
#include "inet/linklayer/common/MACAddress.h"
#include "inet/linklayer/ethernet/Ethernet.h"
//CoRE4INET
#include "core4inet/base/CoRE4INET_Defs.h"
//FiCo4OMNeT
//Auto-generated messages
#include "fico4omnet/linklayer/can/messages/CanDataFrame_m.h"
using namespace FiCo4OMNeT;
using namespace inet;

namespace CAN4CQF {

Define_Module(VarGatewayTransformation);
const int VarGatewayTransformation::CANCRCBITLENGTH = 16;

VarGatewayTransformation::VarGatewayTransformation(){
    this->EncapCanFrameCntSignal = 0;
    this->Circular_BandwidthSignal = 0;
}

VarGatewayTransformation::~VarGatewayTransformation(){
    // 删除 canfdNeedResendList 中剩余的消息，防止内存泄漏
    for(auto it = this->canfdNeedResendList.begin(); it != this->canfdNeedResendList.end(); ++it){
        delete *it;
    }
    this->canfdNeedResendList.clear();
}

void VarGatewayTransformation::initialize()
{
    EncapCanFrameCntSignal = registerSignal("EncapCanFrameCnt");
    Circular_BandwidthSignal = registerSignal("RAE_Bandwidth");
    this->timeout = par("timeout").doubleValue();
    handleParameterChange(nullptr);
    readConfigXML();
}

void VarGatewayTransformation::handleParameterChange(const char* parname) {

    if (!parname || !strcmp(parname, "gatewayID"))
    {
        this->gatewayID = par("gatewayID").stringValue();
        if(this->gatewayID.empty() || !strcmp(this->gatewayID.c_str(), "auto")){
            //auto create id!
            this->gatewayID = this->getParentModule()->getParentModule()->getName();
        }
    }
}

// 从XML读取配置，主要用到"toqEthernet"
void VarGatewayTransformation::readConfigXML(){
    cXMLElement* xmlDoc = par("configXML").xmlValue();
    string gatewayName = this->gatewayID;
    string thisGateNo = to_string(this->getIndex());
    string xpath = "/config/gateway[@id='" + gatewayName + "']";
    cXMLElement* xmlGateway = xmlDoc->getElementByPath(xpath.c_str(), xmlDoc);
    cXMLElementList xmlTransformations = xmlGateway->getChildrenByTagName("transformation");
    for (auto iterTrans = xmlTransformations.begin(); iterTrans != xmlTransformations.end(); iterTrans++) {
        cXMLElement* xmlTransformation = *iterTrans;
        if(!strcmp(xmlTransformation->getAttribute("destination"), thisGateNo.c_str())){
            if(xmlTransformation){
                string type = xmlTransformation->getAttribute("type");
                if(type.compare("toEthernet") == 0){
                    // 将CAN封装成以太网
                    cXMLElementList xmlEthernetFrames = xmlTransformation->getChildrenByTagName("ethernetframe");
                    for(cXMLElementList::iterator it = xmlEthernetFrames.begin(); it != xmlEthernetFrames.end(); ++it){
                        string xmlDst = (*it)->getAttribute("dst");
                        cXMLElementList xmlCanFrames = (*it)->getChildrenByTagName("canframe");
                        for(cXMLElementList::iterator it2 = xmlCanFrames.begin(); it2 != xmlCanFrames.end(); ++it2){
                            canToBEEthernet[static_cast<unsigned int>(atoi((*it2)->getAttribute("canId")))].push_back(xmlDst);
                        }
                    }
                    // 将CAN封装成IEEE8021Q帧，主要采用，因为Qch和Q没有什么区别
                    cXMLElementList xmlQFrames = xmlTransformation->getChildrenByTagName("ethernetqframe");
                    for(cXMLElementList::iterator it = xmlQFrames.begin(); it != xmlQFrames.end(); ++it){
                        string xmlDst = (*it)->getAttribute("dst");
                        const char* xmlVid = (*it)->getAttribute("vid");
                        int vid = 0;
                        if(xmlVid){
                            vid = atoi(xmlVid);
                            if(vid < 1 || vid > 4096){
                                vid = 0;
                            }
                        }
                        const char* xmlPcp = (*it)->getAttribute("pcp");
                        int pcp = 0;
                        if(xmlPcp){
                            pcp = atoi(xmlPcp);
                            if(pcp < 1 || pcp > 7){
                                pcp = 0;
                            }
                        }
                        cXMLElementList xmlCanFrames = (*it)->getChildrenByTagName("canframe");
                        for(cXMLElementList::iterator it2 = xmlCanFrames.begin(); it2 != xmlCanFrames.end(); ++it2){
                            QInfo qinfo;
                            qinfo.mac = xmlDst;
                            qinfo.vid = static_cast<uint16_t>(vid);
                            qinfo.pcp = static_cast<uint8_t>(pcp);
                            canToQEthernet[static_cast<unsigned int>(atoi((*it2)->getAttribute("canId")))].push_back(qinfo);
                        }
                    }
                }else if(type.compare("toCan") == 0){
                    cXMLElementList xmlCANFrames = xmlTransformation->getChildrenByTagName("canframe");
                    for(cXMLElementList::iterator it = xmlCANFrames.begin(); it != xmlCANFrames.end(); ++it){
                        unsigned int xmlCANID = static_cast<unsigned int>(atoi((*it)->getAttribute("canId")));
                        canToCan.push_back(xmlCANID);
                    }
                    cXMLElementList xmlEthernetFrames = xmlTransformation->getChildrenByTagName("ethernetframe");
                    for(cXMLElementList::iterator it = xmlEthernetFrames.begin(); it != xmlEthernetFrames.end(); ++it){
                        string xmlDst = (*it)->getAttribute("dst");
                        beEthernetToCan.push_back(xmlDst);
                    }
                    cXMLElementList xmlQFrames = xmlTransformation->getChildrenByTagName("ethernetqframe");
                    for(cXMLElementList::iterator it = xmlQFrames.begin(); it != xmlQFrames.end(); ++it){
                        string xmlDst = (*it)->getAttribute("dst");
                        qEthernetToCan.push_back(xmlDst);
                    }
                    cXMLElementList xmlAVBFrames = xmlTransformation->getChildrenByTagName("avbframe");
                }
            }
        }
    }
}

void VarGatewayTransformation::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        // 处理自消息
        string rdySendMsgName = msg->getName();
        for (auto it = this->canfdNeedResendList.begin(); it != this->canfdNeedResendList.end(); ) {
            if ((*it)->getCanID() == static_cast<unsigned int>(atoi(rdySendMsgName.c_str()))) {
                CanDataFrame* msgToSend = (*it)->dup(); // 创建消息副本
                EV<<"已发送CANFD，id："<<msgToSend->getCanID()<<endl;
                send(msgToSend, gate("out"));  // 发送副本
                cancelEvent(msg);  // 取消事件
                delete *it;  // 删除原消息，防止内存泄漏
                it = this->canfdNeedResendList.erase(it);  // 从列表中移除
                delete msg;  // 删除自消息，防止内存泄漏
                break;
            } else {
                ++it;  // 只有在没有发送的情况下才移动迭代器
            }
        }
    } else if (msg->arrivedOn("in")) {
        // 处理通过输入端口到达的消息
        if (CircularRotationMessage* crclmsg = dynamic_cast<CircularRotationMessage*>(msg)) {
            // 如果是 CircularRotationMessage 类型的消息
            emit(this->EncapCanFrameCntSignal, static_cast<long>(crclmsg->encapsulationCount()));
            IEEE8021QchTTFlow* ttflow = transformcrclmsgToTTFlow(crclmsg);
            send(ttflow, gate("out"));  // 发送 ttflow 消息
            // crclmsg 已经被封装到 ttflow 中，不需要删除
        } else if (IEEE8021QchTTFlow* ttflow = dynamic_cast<IEEE8021QchTTFlow*>(msg)) {
            // 如果是 IEEE8021QchTTFlow 类型的消息
            double bandwidth = ttflow->getBitLength() / (this->timeout * 1e8);
            emit(this->Circular_BandwidthSignal, static_cast<double>(bandwidth));
            transformTTFlowToCan(ttflow);  // 转换并处理
            delete ttflow;  // 删除 ttflow，防止内存泄漏
        } else {
            // 未识别的消息类型，删除以防止内存泄漏
            delete msg;
        }
    } else {
        // 未识别的消息类型，删除以防止内存泄漏
        delete msg;
    }
}

IEEE8021QchTTFlow* VarGatewayTransformation::transformcrclmsgToTTFlow(CircularRotationMessage* crclmsg){
    IEEE8021QchTTFlow* ttflow = new IEEE8021QchTTFlow();
    string messageName = createMessageName("crclmsg");
    this->crcToMap = crclmsg->decapsulateCanDataFrameAndHoldTime();
    for(std::map<FiCo4OMNeT::CanDataFrame*,double>::iterator it = this->crcToMap.begin();it!=this->crcToMap.end();++it){
        if(canToQEthernet.find((*it).first->getCanID()) !=canToQEthernet.end()){
            for(list<QInfo>::iterator it1 = canToQEthernet[(*it).first->getCanID()].begin(); it1 != canToQEthernet[(*it).first->getCanID()].end(); ++it1){
                ttflow->setDest(MACAddress((*it1).mac.c_str()));
                ttflow->setVID((*it1).vid);
                ttflow->setPcp((*it1).pcp);
                ttflow->setName(messageName.c_str());
            }
        }
    }
    ttflow->encapsulate(crclmsg);  // crclmsg 的所有权转移到 ttflow
    if(ttflow->getEncapsulatedPacket()->getByteLength() <= (MIN_ETHERNET_FRAME_BYTES - ETHER_MAC_FRAME_BYTES - ETHER_8021Q_TAG_BYTES)){
        ttflow->setByteLength(MIN_ETHERNET_FRAME_BYTES);
    }else{
        ttflow->setByteLength(ttflow->getEncapsulatedPacket()->getByteLength() + ETHER_MAC_FRAME_BYTES + ETHER_8021Q_TAG_BYTES);
    }
    return ttflow;    
}

void VarGatewayTransformation::transformTTFlowToCan(IEEE8021QchTTFlow* ttflow){
    CircularRotationMessage* crclmsg = dynamic_cast<CircularRotationMessage*>(ttflow->getEncapsulatedPacket());
    // 注意：解封后的second的单位是s
    this->canfdToMap = crclmsg->decapsulateCanDataFrameAndHoldTime();
    for(auto canfdHoldTime: canfdToMap){
        if(canfdHoldTime.second < 0){
            // Negative hold time, skip scheduling
            EV_WARN << "Negative hold time (" << canfdHoldTime.second << " s) for message with ID " << canfdHoldTime.first->getCanID() << ", skipping scheduling.\n";
            delete canfdHoldTime.first; // Delete the message
            continue;
        }
        this->canfdNeedResendList.push_back(canfdHoldTime.first);
        string holdTimemsgName = to_string(canfdHoldTime.first->getCanID());
        cMessage* holdtimemsg = new cMessage(holdTimemsgName.c_str());
        scheduleAt(simTime() + static_cast<simtime_t>(canfdHoldTime.second), holdtimemsg);
    }
}


string VarGatewayTransformation::createMessageName(const char* additionalInformation){
    string str;
    str.append(gatewayID);
    str.append(" - ");
    str.append(additionalInformation);
    return str;    
}

} //namespace
