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

#ifndef __CAN4CQF_IEEE8021QCH_INCONTROL_H_
#define __CAN4CQF_IEEE8021QCH_INCONTROL_H_

#include <omnetpp.h>
#include <algorithm>    // std::sort

//CoRE4INET
#include "core4inet/base/CoRE4INET_Defs.h"
//INET Auto-generated Messages
#include "inet/linklayer/ethernet/EtherFrame_m.h"
//can4cqf
#include "can4cqf/linklayer/message/IEEE8021QchTTFlow_m.h"
#include "can4cqf/linklayer/message/IEEE8021QchBEFlow_m.h"

using namespace omnetpp;

namespace CAN4CQF {
template<class IC>
/**
 * TODO - Generated class
 */
class IEEE8021QchInControl : public IC
{
public:
    /**
     * @brief Constructor
     */
    IEEE8021QchInControl();
protected:
    /**
     * @brief Forwards frames to the appropriate incoming modules
     *
     * Critical traffic arriving on in-gate is forwarded to the incoming modules
     * or dropped if there is no module configured. Best-effort frames are
     * forwarded through the out-gate. The function timestamps messages using the
     * received and received_total parameters.
     *
     * @param msg incoming message
     */
    virtual void handleMessage(cMessage *msg) override;

    /**
     * @brief Indicates a parameter has changed.
     *
     * @param parname Name of the changed parameter or nullptr if multiple parameter changed.
     */
    virtual void handleParameterChange(const char* parname) override;

private:
    /**
     * @brief Untagged VLAN.
     * Untagged incoming frames get tagged with this VLAN. Outgoing frames with this VLAN get untagged.
     */
    uint16_t untaggedVID;
    /**
     * @brief Tagged VLANs.
     * Only outgoing frames with one of the VLANs in this list are transmitted. Default is "0" to allow for untagged frames
     */
    std::vector<int> taggedVIDs;
};

template<class IC>
IEEE8021QchInControl<IC>::IEEE8021QchInControl()
{
untaggedVID = 0;
}

template<class IC>
void IEEE8021QchInControl<IC>::handleMessage(cMessage *msg)
{
if (msg->arrivedOn("in"))
{
    if (inet::EtherFrame *frame = dynamic_cast<inet::EtherFrame*>(msg))
    {

        if (EthernetIIFrameWithQTag* qframe = dynamic_cast<EthernetIIFrameWithQTag*>(frame))
        {
            //VLAN tag if requested
            if (this->untaggedVID && qframe->getVID() == 0)
            {
                qframe->setVID(this->untaggedVID);
            }
            //VLAN check if port is tagged with VLAN
            bool found = false;
            for (std::vector<int>::iterator vid = this->taggedVIDs.begin(); vid != this->taggedVIDs.end(); ++vid)
            {
                //Shortcut due to sorted vector
                if ((*vid) > qframe->getVID())
                {
                    break;
                }
                if ((*vid) == qframe->getVID())
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                EV_WARN << "Received IEEE 802.1Q frame with VID " << qframe->getVID()
                        << " but port was not tagged with that VID. Frame was dropped";
                delete qframe;
                return;
            }
        } else if(IEEE8021QchTTFlow* ttflow = dynamic_cast<IEEE8021QchTTFlow*>(msg)){
            // 绑定VLANv标签
            if(this->untaggedVID && ttflow->getVID() == 0){
                ttflow->setVID(this->untaggedVID);
            }
        } else if(IEEE8021QchBEFlow* beflow = dynamic_cast<IEEE8021QchBEFlow*>(msg)){
            if(this->untaggedVID && beflow->getVID() == 0){
                beflow->setVID(this->untaggedVID);
            }
        }

        this->recordPacketReceived(frame);

        if (IC::isPromiscuous() || frame->getDest().isMulticast())
        {
            cSimpleModule::send(msg, "out");
        }
        else
        {
            inet::MACAddress address;
            address.setAddress(this->getParentModule()->getSubmodule("mac")->par("address"));
            if (frame->getDest().equals(address))
            {
                cSimpleModule::send(msg, "out");
            }
            else
            {
                IC::handleMessage(msg);
            }
        }
    }
    else
    {
        throw cRuntimeError("Received message on port in that is no EtherFrame");
    }
}
else
{
    IC::handleMessage(msg);
}
}

template<class IC>
void IEEE8021QchInControl<IC>::handleParameterChange(const char* parname)
{
IC::handleParameterChange(parname);

if (!parname || !strcmp(parname, "untaggedVID"))
{
    this->untaggedVID = static_cast<uint16_t>(parameterULongCheckRange(cComponent::par("untaggedVID"), 0,
           MAX_VLAN_NUMBER));
}
if (!parname || !strcmp(parname, "taggedVIDs"))
{
    taggedVIDs = cStringTokenizer(cComponent::par("taggedVIDs"), ",").asIntVector();
    std::sort(taggedVIDs.begin(), taggedVIDs.end());
}
};

} //namespace

#endif
