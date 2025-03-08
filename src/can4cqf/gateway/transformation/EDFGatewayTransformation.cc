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

#include <can4cqf/gateway/transformation/EDFGatewayTransformation.h>

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

Define_Module(EDFGatewayTransformation);
const int EDFGatewayTransformation::CANCRCBITLENGTH = 16;
EDFGatewayTransformation::EDFGatewayTransformation(){
    this->EncapCanFrameCntSignal = 0;
    this->RAE_BandwidthSignal = 0;
}

void EDFGatewayTransformation::initialize()
{
    EncapCanFrameCntSignal = registerSignal("EncapCanFrameCnt");
    RAE_BandwidthSignal = registerSignal("RAE_Bandwidth");
    handleParameterChange(nullptr);
    readConfigXML();
}

void EDFGatewayTransformation::handleParameterChange(const char* parname) {

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
void EDFGatewayTransformation::readConfigXML(){
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

void EDFGatewayTransformation::handleMessage(cMessage *msg)
{
    if(msg->arrivedOn("in")){
        if(ARTF* artf = dynamic_cast<ARTF*>(msg)){
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
}

void EDFGatewayTransformation::transformartfToTTFlow(ARTF* artf){
    IEEE8021QchTTFlow* ttflow = new IEEE8021QchTTFlow();
    string messageName = createMessageName("ARTF");
    // 增加测试语句
    // EV<<"ARTF封装了"<<artf->getEncapCnt()<<"个CAN报文";
    std::list<FiCo4OMNeT::CanDataFrame*> ARTFList_new = artf->getEncapUnits();
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
    ttflow->encapsulate(artf);
    // 修改IEEE8021Q帧的比特大小
    if(ttflow->getEncapsulatedPacket()->getByteLength() <= (MIN_ETHERNET_FRAME_BYTES - ETHER_MAC_FRAME_BYTES - ETHER_8021Q_TAG_BYTES)){
        ttflow->setByteLength(MIN_ETHERNET_FRAME_BYTES);
    }else{
        ttflow->setByteLength(ttflow->getEncapsulatedPacket()->getByteLength() + ETHER_MAC_FRAME_BYTES + ETHER_8021Q_TAG_BYTES);
    }
    send(ttflow,"out");
    // 清空存储语句
}
// 下面的函数是针对下行链路：TSN->CAN
void EDFGatewayTransformation::transformTTFlowToCanDataFrame(IEEE8021QchTTFlow* ttflow){
/*
 * 函数逻辑，对于接收的ttflow，首先判断其解封装的类型，如果是HRTF，直接进一步解封装为CanDataFrame，直接发送到Can模块;
 * 如果是ARTF，需要先遍历所有ARTF是否存在即将超过deadline的报文，如果有那么先转发该报文，如果没有就按解封装的顺序逐个转发。
 * */
    if (dynamic_cast<ARTF*>(ttflow->getEncapsulatedPacket()) != nullptr) {
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
string EDFGatewayTransformation::createMessageName(const char* additionalInformation){
    string str;
    str.append(gatewayID);
    str.append(" - ");
    str.append(additionalInformation);
    return str;
}
} //namespace
