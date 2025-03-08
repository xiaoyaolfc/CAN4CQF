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

#include "EthernetGatewayApplication.h"
#include "core4inet/applications/base/ApplicationBase.h"
#include "core4inet/linklayer/ethernet/avb/AVBFrame_m.h"
#include "core4inet/incoming/base/Incoming.h"
// 导入自定义的TT和BE流数据
#include "can4cqf/linklayer/message/IEEE8021QchTTFlow_m.h"
#include "can4cqf/linklayer/message/IEEE8021QchBEFlow_m.h"

//using namespace CoRE4INET;
//using namespace FiCo4OMNeT;

namespace CAN4CQF {

Define_Module(EthernetGatewayApplication);

EthernetGatewayApplication::EthernetGatewayApplication(){

}

EthernetGatewayApplication::~EthernetGatewayApplication(){

}

void EthernetGatewayApplication::initialize()
{
    TTApplicationBase::initialize();
}
/*
 * else if(CTFrame* ctFrame = dynamic_cast<CTFrame*>(msg)){
            std::list<CoRE4INET::CTBuffer*> buffer = ctbuffers[ctFrame->getCtID()];
            for(std::list<CoRE4INET::CTBuffer*>::iterator buf = buffer.begin(); buf!=buffer.end(); ++buf){
                Incoming *in = dynamic_cast<Incoming *>((*buf)->gate("in")->getPathStartGate()->getOwner());
                sendDirect(ctFrame->dup(), in->gate("in"));
            }else if(inet::EthernetIIFrame* ethernetFrame = dynamic_cast<inet::EthernetIIFrame*>(msg)){
            for (std::list<BGBuffer*>::iterator buf = bgbuffers.begin(); buf != bgbuffers.end(); ++buf) {
                sendDirect(ethernetFrame->dup(), (*buf)->gate("in"));
            }
            delete ethernetFrame;
        }
 * */
void EthernetGatewayApplication::handleMessage(cMessage *msg)
{
    TTApplicationBase::handleMessage(msg);
    if(msg->arrivedOn("in")){
        send(msg, "upperLayerOut");
    } else if(msg->arrivedOn("upperLayerIn")){
        if(IEEE8021QchTTFlow* ttflow = dynamic_cast<IEEE8021QchTTFlow*>(msg)){
            sendDirect(ttflow->dup(),gate("in"));
            delete(ttflow);
        } else if(IEEE8021QchBEFlow* beflow = dynamic_cast<IEEE8021QchBEFlow*>(msg)){
            sendDirect(beflow->dup(),gate("in"));
            delete(beflow);
        }
//        下边儿的代码有问题，暂时不会解决
//        else if(inet::EthernetIIFrame* ethernetFrame = dynamic_cast<inet::EthernetIIFrame*>(msg)){
//            for (std::list<CoRE4INET::BGBuffer*>::iterator buf = ApplicationBase::bgbuffers.begin(); buf != ApplicationBase::bgbuffers.end(); ++buf) {
//                sendDirect(ethernetFrame->dup(), (*buf)->gate("in"));
//            }
//            delete ethernetFrame;
//        }
    } else{
        delete msg;
    }
}

} //namespace
