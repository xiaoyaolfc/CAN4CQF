//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "RAEGatewayTransformation.h"
//INET
#include "inet/linklayer/common/MACAddress.h"
#include "inet/linklayer/ethernet/Ethernet.h"
//CoRE4INET
#include "core4inet/base/CoRE4INET_Defs.h"
//FiCo4OMNeT
//Auto-generated messages
#include "fico4omnet/linklayer/can/messages/CanDataFrame_m.h"
using namespace omnetpp;
using namespace std;
using namespace inet;
using namespace FiCo4OMNeT;


namespace CAN4CQF {

Define_Module(RAEGatewayTransformation);
const int RAEGatewayTransformation::CANCRCBITLENGTH = 16;

RAEGatewayTransformation::RAEGatewayTransformation(){
    this->EncapCanFrameCntSignal = 0;
    this->RAE_BandwidthSignal = 0;
}

void RAEGatewayTransformation::initialize()
{
    handleParameterChange(nullptr);
    readConfigXML();
    this->maxNuminCQF = par("maxNuminCQF").intValue();
    this->timeout = par("timeout").doubleValue();
    this->Deterministic_flow = par("Deterministic_flow").stringValue();
    this->order_list.clear();
    EncapCanFrameCntSignal = registerSignal("EncapCanFrameCnt");
    RAE_BandwidthSignal = registerSignal("RAE_Bandwidth");
}

// 参数检测
void RAEGatewayTransformation::handleParameterChange(const char* parname) {

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
void RAEGatewayTransformation::readConfigXML(){
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
void RAEGatewayTransformation::handleMessage(cMessage *msg)
{
    if(msg->arrivedOn("in")){
        if(HRTF* hrtf = dynamic_cast<HRTF*>(msg)){
            transformhrtfToTTFlow(hrtf);
        }else if(ARTF* artf = dynamic_cast<ARTF*>(msg)){
            // 发送signal，数值为当前封装的报文数量
            emit(this->EncapCanFrameCntSignal, static_cast<long>(artf->getordernum()));
            transformartfToTTFlow(artf);
        }else if(IEEE8021QchTTFlow* ttflow = dynamic_cast<IEEE8021QchTTFlow*>(msg)){
            // 发送signal，表示网络带宽利用率: sum(si.length)*data_rate / T_CQF
            double bandwidth = ttflow->getBitLength() / (this->timeout * 1e8);
            emit(this->RAE_BandwidthSignal, static_cast<double>(bandwidth));
            transformTTFlowToCanDataFrame(ttflow);
            delete ttflow;
        }else{
            delete msg;
        }
    }
//    delete msg;
}

// 下面的两个函数都是上行链路：CAN->TSN的封装
// 针对HRTF封装，由于RAE中可能存在单独封装，但是根据目前设计，即使为one-to-one的封装方式，仍然采用ARTF进行封装，因此本函数可能暂时用不上
void RAEGatewayTransformation::transformhrtfToTTFlow(HRTF* hrtf){
    IEEE8021QchTTFlow* ttflow = new IEEE8021QchTTFlow();
    string messageName = createMessageName("HRTF");
    // 为了下面使用方便，这里定义一个CanDataFrame用于表示HRTF里封装的帧
    FiCo4OMNeT::CanDataFrame* canFrame = new FiCo4OMNeT::CanDataFrame;
    canFrame = dynamic_cast<FiCo4OMNeT::CanDataFrame*>(hrtf->dup()->decapsulate());
    canFrame->setBitLength(canFrame->getBitLength() - CANCRCBITLENGTH);
    // 判断从XML读取到的CAN信息是否与刚刚收到的CAN-ID匹配
    if(canToQEthernet.find(canFrame->getCanID()) != canToQEthernet.end()){
        for(list<QInfo>::iterator it = canToQEthernet[canFrame->getCanID()].begin(); it != canToQEthernet[canFrame->getCanID()].end(); ++it){
            ttflow->encapsulate(hrtf);
            ttflow->setDest(MACAddress((*it).mac.c_str()));
            ttflow->setVID((*it).vid);
            ttflow->setPcp((*it).pcp);
            ttflow->setName(messageName.c_str());
            if (ttflow->getEncapsulatedPacket()->getByteLength() <= (MIN_ETHERNET_FRAME_BYTES - ETHER_MAC_FRAME_BYTES - ETHER_8021Q_TAG_BYTES)){
                ttflow->setByteLength(MIN_ETHERNET_FRAME_BYTES);
            }else{
                ttflow->setByteLength(ttflow->getEncapsulatedPacket()->getByteLength() + ETHER_MAC_FRAME_BYTES + ETHER_8021Q_TAG_BYTES);
            }
        }
    }
    send(ttflow,"out");
    delete canFrame;
}

// 针对ARTF的封装为本模块的核心
void RAEGatewayTransformation::transformartfToTTFlow(ARTF* artf){
    IEEE8021QchTTFlow* ttflow = new IEEE8021QchTTFlow();
    string messageName = createMessageName("ARTF");
    // 存储当前ARTF封装报文的数量，由于当前封装的CAN报文存储在ARTF中的order_GW中，故后续操作应该基于order_GW
    int encap_num = artf->getordernum();
    // 从ARTF解封装出 到达顺序、时间戳
    this->order_GW =  Decap_orderfromeARTF(artf);
    // 计算当前循环的次数
    double T_CQF_num_now = std::floor(simTime().dbl() / this->timeout);
    // 调整到达顺序
    ARTF* artf_new = new ARTF();
    artf_new = Resort_order(order_GW, T_CQF_num_now, encap_num);
    // 增加测试语句
    EV<<"ARTF封装了"<<artf_new->getEncapCnt()<<"个CAN报文";
    std::list<FiCo4OMNeT::CanDataFrame*> ARTFList_new = artf_new->getEncapUnits();
    for(std::list<FiCo4OMNeT::CanDataFrame*>::iterator it = ARTFList_new.begin() ; it != ARTFList_new.end() ; ++it){
        if(canToQEthernet.find((*it)->getCanID()) != canToQEthernet.end()){
            for(list<QInfo>::iterator it1 = canToQEthernet[(*it)->getCanID()].begin(); it1 != canToQEthernet[(*it)->getCanID()].end(); ++it1){
                ttflow->setDest(MACAddress((*it1).mac.c_str()));
                ttflow->setVID((*it1).vid);
                ttflow->setPcp((*it1).pcp);
                ttflow->setName(messageName.c_str());
            }
        }
    }
    ttflow->encapsulate(artf_new);
    // 修改IEEE8021Q帧的比特大小
    if(ttflow->getEncapsulatedPacket()->getByteLength() <= (MIN_ETHERNET_FRAME_BYTES - ETHER_MAC_FRAME_BYTES - ETHER_8021Q_TAG_BYTES)){
        ttflow->setByteLength(MIN_ETHERNET_FRAME_BYTES);
    }else{
        ttflow->setByteLength(ttflow->getEncapsulatedPacket()->getByteLength() + ETHER_MAC_FRAME_BYTES + ETHER_8021Q_TAG_BYTES);
    }
    send(ttflow,"out");
    // 清空存储语句
    this->order_GW.clear();
}
// 下面的函数是针对下行链路：TSN->CAN
void RAEGatewayTransformation::transformTTFlowToCanDataFrame(IEEE8021QchTTFlow* ttflow){
/*
 * 函数逻辑，对于接收的ttflow，首先判断其解封装的类型，如果是HRTF，直接进一步解封装为CanDataFrame，直接发送到Can模块;
 * 如果是ARTF，需要先遍历所有ARTF是否存在即将超过deadline的报文，如果有那么先转发该报文，如果没有就按解封装的顺序逐个转发。
 * */
    // 出现错误，也许将HRTF和ARTF内容进行更改，不使用payload存储报文？但是HRTF和ARTF不是cPacket类型，讲道理因该是不行！
    if (dynamic_cast<HRTF*>(ttflow->getEncapsulatedPacket()) != nullptr){
        HRTF* hrtf = dynamic_cast<HRTF*>(ttflow->decapsulate());
        // 当进入该语句解封装后为hrtf，还需要进一步解封装为CanDataFrame
        // 测试语句
        EV<<"解封装了"<<hrtf->getEncapnum()<<"个CAN报文";
        FiCo4OMNeT::CanDataFrame* canframe = dynamic_cast<FiCo4OMNeT::CanDataFrame*>(hrtf->decapsulate());
        send(canframe,"out");
        delete hrtf;
    }else if (dynamic_cast<ARTF*>(ttflow->getEncapsulatedPacket()) != nullptr) {
        ARTF* artf = dynamic_cast<ARTF*>(ttflow->decapsulate());
        this->ARTFList.clear();
        while (FiCo4OMNeT::CanDataFrame* dataframe = artf->decapARTF()) {
            this->ARTFList.push_back(dataframe);
        }
        EV<<"解封装了"<<artf->getEncapCnt()<<"个CAN报文";
        for (std::list<FiCo4OMNeT::CanDataFrame*>::iterator it = ARTFList.begin();
             it != ARTFList.end(); ++it) {
            FiCo4OMNeT::CanDataFrame* dataframe = *it;//(*it)->dup();
            dataframe->setBitLength(dataframe->getBitLength() + CANCRCBITLENGTH);
            send(dataframe, "out");
        }
        delete artf;  // Release ownership of ARTF
    }
}
// 创建一个储存名称的函数
string RAEGatewayTransformation::createMessageName(const char* additionalInformation){
    string str;
    str.append(gatewayID);
    str.append(" - ");
    str.append(additionalInformation);
    return str;
}


// 从ARTF中解封装到达顺序和时间戳
std::map<FiCo4OMNeT::CanDataFrame*, simtime_t> RAEGatewayTransformation::Decap_orderfromeARTF(ARTF* artf){
    std::map<FiCo4OMNeT::CanDataFrame*, simtime_t> order_tmp;
    if(artf->getordernum() > 0){
        simtime_t arr_time;
        while(FiCo4OMNeT::CanDataFrame* dataframe = artf->decapARTF()){
            dataframe->setBitLength(dataframe->getBitLength() + CANCRCBITLENGTH);
            arr_time = artf->decap_order_arrtime(dataframe);
//            order_tmp.insert(pair<FiCo4OMNeT::CanDataFrame*, simtime_t>(dataframe->dup(), arr_time));
            order_tmp.insert(pair<FiCo4OMNeT::CanDataFrame*, simtime_t>(dataframe, arr_time));
//            delete dataframe;
        }
        delete artf;
    }
    return order_tmp;
}

// 调整报文封装顺序
ARTF* RAEGatewayTransformation::Resort_order(std::map<FiCo4OMNeT::CanDataFrame*, simtime_t> order_GW, double T_CQF, int encap_num){
    // 使用 order_info存储
    for (std::map<FiCo4OMNeT::CanDataFrame*, simtime_t>::iterator it1 = order_GW.begin();
            it1 != order_GW.end(); ++it1){
        order_info flow;
        flow.dataframe = it1->first;
        flow.arr_time = it1->second;
        flow.index = order_list.size()>0 ? order_list.size() + 1: 1;
//        auto iterator it2 = order_list.begin();
        std::vector<order_info>::reverse_iterator it2 = order_list.rbegin();
        if(it1 == order_GW.begin()){
            flow.interval_time = 0;
        }else{
            int64_t current_time = (flow.arr_time).inUnit(SIMTIME_US);
            int64_t pre_time = (it2->arr_time).inUnit(SIMTIME_US);
            flow.interval_time = static_cast<double>(current_time - pre_time) * 1e-6;
        }
        if(flow.interval_time == 0){
            flow.issucc = false;
            // 针对每个队列中的第一个元素，采用随机数的方式定义其可能的排队时延
            flow.queue_time = 2.2e-4 * (rand() % (this->maxNuminCQF + 1));
        } else if(std::abs(flow.interval_time <= flow.dataframe->getBitLength() * 1e-6)){
// 当两个相邻CAN报文到达时间间隔小于某个数值，即判断为连续到达顺序
            flow.issucc = true;
            for(std::vector<order_info>::reverse_iterator it3 = order_list.rbegin();
                    it3 != order_list.rend(); ++it3){
                if(it3->issucc){
                    flow.queue_time += static_cast<double>(it3->dataframe->getBitLength() + 24) * 5e-5;
                }else{
                    flow.queue_time = static_cast<double>(it3->dataframe->getBitLength() + 24) * 5e-5;
                    break;
                }
            }
        } else{
            flow.issucc = false;
            flow.queue_time = flow.dataframe->getBitLength() * 5e-5;
        }
        this->order_list.push_back(flow);
    }
// 可以认为order_list已经完成封装，下面需要调整顺序了
    std::sort(std::begin(order_list), std::end(order_list),
            [](const order_info& p1, const order_info& p2) {return p1.get_queue_time() < p2.get_queue_time();});
//    针对高优先级确定性保证机制，采用strict priority。需要用户指定那些流需要设定确定性保证
    std::istringstream iss(this->Deterministic_flow);
    std::vector<int> dtrflow;
    std::string token;
    while (std::getline(iss, token, ',')) {
        dtrflow.push_back(std::stoi(token));
    }
//    遍历order_list中是否存在Deterministic_flow，如果存在，则将该flow移动至头部位置，如果没有，则无影响
    for(std::vector<int>::reverse_iterator it3 = dtrflow.rbegin() ; it3 != dtrflow.rend() ; ++it3){
        for(vector<order_info>::iterator it4 = order_list.begin() ; it4 != order_list.end() ; ++it4){
            if(it4->dataframe->getCanID() == static_cast<unsigned int>(*it3)){
                std::rotate(order_list.begin(), it4, it4 + 1);
            }
        }
    }
    ARTF* artf = new ARTF();
    for(std::vector<order_info>::iterator it4 = order_list.begin();
            it4 != order_list.end() ; ++it4){
        artf->encapCanDataFrame(it4->dataframe);
        artf->setBitLength(artf->getBitLength() + it4->dataframe->getBitLength());
    }
    // 清空存储元素
    this->order_list.clear();
    return artf;
}
// 从ARTF中解封装

} //namespace
