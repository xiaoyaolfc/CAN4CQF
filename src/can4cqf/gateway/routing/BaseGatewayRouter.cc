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

#include "BaseGatewayRouter.h"
#include <algorithm>
#include <cstdlib>
//#include "signalsandgateways/gateway/messages/GatewayAggregationMessage.h"
//INET
#include "inet/linklayer/common/MACAddress.h"
//CoRE4INET
#include "core4inet/linklayer/ethernet/AS6802/CTFrame.h"
//Auto-generated messages
#include "fico4omnet/linklayer/can/messages/CanDataFrame_m.h"
#include "core4inet/linklayer/ethernet/avb/AVBFrame_m.h"
#include "core4inet/linklayer/ethernet/base/EtherFrameWithQTag_m.h"
//CAN4CQF
#include "can4cqf/linklayer/message/IEEE8021QchTTFlow_m.h"
#include "can4cqf/linklayer/message/IEEE8021QchBEFlow_m.h"

using namespace std;
using namespace FiCo4OMNeT;
using namespace CoRE4INET;

namespace CAN4CQF {

Define_Module(BaseGatewayRouter);
Define_Module(BaseGatewayRouter);

const std::string BaseGatewayRouter::CANPREFIX = "can_";
const std::string BaseGatewayRouter::AVBPREFIX = "avb_";
const std::string BaseGatewayRouter::TTEPREFIX = "tte_";
const std::string BaseGatewayRouter::ETHPREFIX = "eth_";
const std::string BaseGatewayRouter::VIDPREFIX = "vid_";
const std::string BaseGatewayRouter::RAWPREFIX = "raw_";

void BaseGatewayRouter::initialize()
{
    droppedFramesSignal = registerSignal("droppedFramesSignal");

    handleParameterChange(nullptr);
    readConfigXML();
}

void BaseGatewayRouter::handleParameterChange(const char* parname) {

    if (!parname || !strcmp(parname, "gatewayID"))
    {
        this->gatewayID = par("gatewayID").stringValue();
        if(this->gatewayID.empty() || !strcmp(this->gatewayID.c_str(), "auto")){
            //auto create id!
            this->gatewayID = this->getParentModule()->getParentModule()->getName();
        }
    }
}

void BaseGatewayRouter::handleMessage(cMessage *msg)
{
    if(msg->arrivedOn("in")){
        vector<int> destinationGateIndices;
        if(CanDataFrame* canFrame = dynamic_cast<CanDataFrame*>(msg)){
//            添加部分测试代码，用来判断是否********************8
            cGate *arrivalGate = msg->getArrivalGate();
            int sourceIndex = arrivalGate->getIndex();
//            **********************************
            destinationGateIndices = getDestinationGateIndices(canFrame->getArrivalGate()->getIndex(), CANPREFIX + to_string(canFrame->getCanID()));
        }else if(AVBFrame* avbFrame = dynamic_cast<AVBFrame*>(msg)){
            destinationGateIndices = getDestinationGateIndices(avbFrame->getArrivalGate()->getIndex(), AVBPREFIX + to_string(avbFrame->getStreamID()));
        }else if(CTFrame* ctFrame = dynamic_cast<CTFrame*>(msg)){
            destinationGateIndices = getDestinationGateIndices(ctFrame->getArrivalGate()->getIndex(), TTEPREFIX + to_string(ctFrame->getCtID()));
        }else if(inet::EthernetIIFrame* ethernetFrame = dynamic_cast<inet::EthernetIIFrame*>(msg)){
            destinationGateIndices = getDestinationGateIndices(ethernetFrame->getArrivalGate()->getIndex(), ETHPREFIX + ethernetFrame->getDest().str());
            if(EthernetIIFrameWithQTag* ethernetQFrame = dynamic_cast<EthernetIIFrameWithQTag*>(msg)){
                if(ethernetQFrame->getVID() > 0 && ethernetQFrame->getVID() < 4096){
                    vector<int> destinationGateIndicesWithVID = getDestinationGateIndices(ethernetQFrame->getArrivalGate()->getIndex(), ETHPREFIX + ethernetQFrame->getDest().str() + VIDPREFIX + to_string(ethernetQFrame->getVID()));
                    destinationGateIndices.insert(destinationGateIndices.end(), destinationGateIndicesWithVID.begin(), destinationGateIndicesWithVID.end());
                }
            } else if(IEEE8021QchTTFlow* ttflow = dynamic_cast<IEEE8021QchTTFlow*>(msg)){
                if(ttflow->getVID() > 0 && ttflow->getVID() < 4096){
                    vector<int> destinationGateIndicesWithVID = getDestinationGateIndices(ttflow->getArrivalGate()->getIndex(), ETHPREFIX + ttflow->getDest().str() + VIDPREFIX + to_string(ttflow->getVID()));
                    destinationGateIndices.insert(destinationGateIndices.end(), destinationGateIndicesWithVID.begin(), destinationGateIndicesWithVID.end());
                }
            }
        }
        if (destinationGateIndices.empty()) {
            emit(droppedFramesSignal,static_cast<unsigned long>(1));
        }
        for(size_t i=0; i<destinationGateIndices.size(); i++){
            if(destinationGateIndices[i] < gateSize("out")){
                send(msg->dup(), "out", destinationGateIndices[i]);
            }
        }
    }
    delete msg;
}

void BaseGatewayRouter::readConfigXML(){
    cXMLElement* xmlDoc = par("configXML").xmlValue();
    string gatewayName = this->gatewayID;
    string xpath = "/config/gateway[@id='" + gatewayName + "']/routing";
    cXMLElement* xmlRouting = xmlDoc->getElementByPath(xpath.c_str(), xmlDoc);
    if(xmlRouting){
        cXMLElementList xmlRoutes = xmlRouting->getChildrenByTagName("route");
        for(size_t i=0; i<xmlRoutes.size(); i++){
            int source = atoi(xmlRoutes[i]->getAttribute("source"));
            int destination = atoi(xmlRoutes[i]->getAttribute("destination"));
            cXMLElementList xmlRouteMessages = xmlRoutes[i]->getChildren();
            for(size_t j=0; j<xmlRouteMessages.size(); j++){
                const char* xmlMessageId;
                xmlMessageId = xmlRouteMessages[j]->getAttribute("canId");
                if(xmlMessageId){
                    string messageID = xmlMessageId;
                    routing[source][destination].push_back(CANPREFIX + messageID);
                }
                xmlMessageId = xmlRouteMessages[j]->getAttribute("streamId");
                if(xmlMessageId){
                    string messageID = xmlMessageId;
                    routing[source][destination].push_back(AVBPREFIX + messageID);
                }
                xmlMessageId = xmlRouteMessages[j]->getAttribute("ctId");
                if(xmlMessageId){
                    string messageID = xmlMessageId;
                    routing[source][destination].push_back(TTEPREFIX + messageID);
                }
                xmlMessageId = xmlRouteMessages[j]->getAttribute("id");
                if(xmlMessageId){
                    string messageID = xmlMessageId;
                    routing[source][destination].push_back(RAWPREFIX + messageID);
                }
                xmlMessageId = xmlRouteMessages[j]->getAttribute("dst");
                if(xmlMessageId){
                    string messageID = xmlMessageId;
                    const char* xmlVid = xmlRouteMessages[j]->getAttribute("vid");
                    if(xmlVid){
                    int vid = atoi(xmlVid);
                        if(vid > 0 && vid < 4096){
                            messageID = messageID + VIDPREFIX + to_string(vid);
                        }
                    }
                    routing[source][destination].push_back(ETHPREFIX + messageID);
                }
            }
        }
    }
}

vector<int> BaseGatewayRouter::getDestinationGateIndices(int sourceIndex, string messageID){
    vector<int> destinationGateIndices;
    for(map<int, list<string> >::iterator it = routing[sourceIndex].begin(); it != routing[sourceIndex].end(); ++it)
    {
        if(find(it->second.begin(), it->second.end(), messageID) != it->second.end()){
            destinationGateIndices.push_back(it->first);
        }
    }
    return destinationGateIndices;
}
} //namespace
