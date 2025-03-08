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

#include "GatewayTransformation.h"
//std
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iterator>
#include <utility>
//INET
#include "inet/linklayer/common/MACAddress.h"
#include "inet/linklayer/ethernet/Ethernet.h"
//CoRE4INET
#include "core4inet/base/CoRE4INET_Defs.h"
//FiCo4OMNeT
//Auto-generated messages
#include "fico4omnet/linklayer/can/messages/CanDataFrame_m.h"

// 由于要使用大量其他框架定义的报文，故导入相应的namespace

using namespace FiCo4OMNeT;
//using namespace CoRE4INET;
using namespace omnetpp;
using namespace std;
using namespace inet;

namespace CAN4CQF {

Define_Module(GatewayTransformation);
const int GatewayTransformation::CANCRCBITLENGTH = 16;

void GatewayTransformation::initialize()
{
    handleParameterChange(nullptr);
    readConfigXML();
    EncapCanFrameCntSignal = registerSignal("EncapCanFrameCnt");
}

void GatewayTransformation::handleParameterChange(const char* parname) {

    if (!parname || !strcmp(parname, "gatewayID"))
    {
        this->gatewayID = par("gatewayID").stringValue();
        if(this->gatewayID.empty() || !strcmp(this->gatewayID.c_str(), "auto")){
            //auto create id!
            this->gatewayID = this->getParentModule()->getParentModule()->getName();
        }
    }
}
void GatewayTransformation::readConfigXML(){
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
//                else if(type.compare("toMisc") == 0){
//                    cXMLElementList xmlRawData = xmlTransformation->getChildrenByTagName("rawdata");
//                    for(cXMLElementList::iterator it = xmlRawData.begin(); it != xmlRawData.end(); ++it){
//                        uint16_t xmlId = static_cast<uint16_t>(atoi((*it)->getAttribute("id")));
//                        cXMLElementList xmlCanFrames = (*it)->getChildrenByTagName("canframe");
//                        for(cXMLElementList::iterator it2 = xmlCanFrames.begin(); it2 != xmlCanFrames.end(); ++it2){
//                            canToRawData[static_cast<unsigned int>(atoi((*it2)->getAttribute("canId")))].push_back(xmlId);
//                        }
//                    }
//                }
            }
        }
    }
}
void GatewayTransformation::handleMessage(cMessage *msg)
{
    if(msg->arrivedOn("in")){
        if(HRTF* hrtf = dynamic_cast<HRTF*>(msg)){
            emit(this->EncapCanFrameCntSignal, static_cast<long>(hrtf->getEncapnum()));
            transformhrtfToTTFlow(hrtf);
        }else if(ARTF* artf = dynamic_cast<ARTF*>(msg)){
            emit(this->EncapCanFrameCntSignal, static_cast<long>(artf->getordernum()));
            transformartfToTTFlow(artf);
        }else if(IEEE8021QchTTFlow* ttflow = dynamic_cast<IEEE8021QchTTFlow*>(msg)){
            transformTTFlowToCanDataFrame(ttflow);
        }
    }
}

// 下面的两个函数都是上行链路：CAN->TSN的封装
void GatewayTransformation::transformhrtfToTTFlow(HRTF* hrtf){
    IEEE8021QchTTFlow* ttflow = new IEEE8021QchTTFlow;
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

void GatewayTransformation::transformartfToTTFlow(ARTF* artf){
    IEEE8021QchTTFlow* ttflow = new IEEE8021QchTTFlow;
    string messageName = createMessageName("ARTF");
    // 由于ARTF存储了一系列CAN帧，所要要使用for循环遍历全部list，对每一个CAN帧进行处理，artf->getPayload() ==  std::list<FiCo4OMNeT::CanDataFrame*>
    std::list<FiCo4OMNeT::CanDataFrame*> ARTFList;
    // 从ARTF中解封装需要使用循环遍历的方式，不妨使用函数实现
    ARTFList = ARTFDecapsulateToARTFList(artf->dup());
    int totalBytelength = 0;
    for(std::list<FiCo4OMNeT::CanDataFrame*>::iterator it = ARTFList.begin() ; it != ARTFList.end() ; ++it){
        if(canToQEthernet.find((*it)->getCanID()) != canToQEthernet.end()){
            for(list<QInfo>::iterator it1 = canToQEthernet[(*it)->getCanID()].begin(); it1 != canToQEthernet[(*it)->getCanID()].end(); ++it1){
                ttflow->setDest(MACAddress((*it1).mac.c_str()));
                ttflow->setVID((*it1).vid);
                ttflow->setPcp((*it1).pcp);
                ttflow->setName(messageName.c_str());
            }
            totalBytelength += (*it)->getByteLength();
        }
    }
    totalBytelength -= ETHER_MAC_FRAME_BYTES + ETHER_MAC_FRAME_BYTES + ETHER_8021Q_TAG_BYTES;
    ttflow->encapsulate(artf);
    // 修改IEEE8021Q帧的比特大小
    if(totalBytelength <= 64){
        ttflow->setByteLength(MIN_ETHERNET_FRAME_BYTES);
    }else{
        ttflow->setByteLength(totalBytelength);
    }
    send(ttflow,"out");
}
// 下面的函数是针对下行链路：TSN->CAN
void GatewayTransformation::transformTTFlowToCanDataFrame(IEEE8021QchTTFlow* ttflow){
/*
 * 函数逻辑，对于接收的ttflow，首先判断其解封装的类型，如果是HRTF，直接进一步解封装为CanDataFrame，直接发送到Can模块;
 * 如果是ARTF，需要先遍历所有ARTF是否存在即将超过deadline的报文，如果有那么先转发该报文，如果没有就按解封装的顺序逐个转发。
 * */
    // 出现错误，也许将HRTF和ARTF内容进行更改，不使用payload存储报文？但是HRTF和ARTF不是cPacket类型，讲道理因该是不行！
    if (dynamic_cast<HRTF*>(ttflow->getEncapsulatedPacket()) != nullptr){
        HRTF* hrtf = dynamic_cast<HRTF*>(ttflow->decapsulate());
        // 当进入该语句解封装后为hrtf，还需要进一步解封装为CanDataFrame
        // 测试语句
        EV<<"封装了"<<hrtf->getEncapnum()<<"个CAN报文";
        FiCo4OMNeT::CanDataFrame* canframe = dynamic_cast<FiCo4OMNeT::CanDataFrame*>(hrtf->decapsulate());
        send(canframe,"out");
    }else if(dynamic_cast<ARTF*>(ttflow->getEncapsulatedPacket()) != nullptr){
        ARTF* artf = dynamic_cast<ARTF*>(ttflow->decapsulate());
        std::list<FiCo4OMNeT::CanDataFrame*> ARTFList = ARTFDecapsulateToARTFList(artf);
        for(std::list<FiCo4OMNeT::CanDataFrame*>::iterator it = ARTFList.begin();
                it !=  ARTFList.end() ; ++it){
           send((*it),"out");
        }
    }
    delete ttflow;
}
// 创建一个储存名称的函数
string GatewayTransformation::createMessageName(const char* additionalInformation){
    string str;
    str.append(gatewayID);
    str.append(" - ");
    str.append(additionalInformation);
    return str;
}
// 用于ARTF解封装
std::list<FiCo4OMNeT::CanDataFrame*> GatewayTransformation::ARTFDecapsulateToARTFList(ARTF* artf){
    std::list<FiCo4OMNeT::CanDataFrame*> ARTFListCopy;
    if(artf){
        while(FiCo4OMNeT::CanDataFrame* dataframe = artf->decapARTF()){
            dataframe->setBitLength(dataframe->getBitLength() + CANCRCBITLENGTH);
            ARTFListCopy.push_back(dataframe);
         }
        delete artf;
    }
    return ARTFListCopy;
}
} //namespace
