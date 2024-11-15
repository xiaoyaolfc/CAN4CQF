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

#include "IEEE8021QchQueueing.h"
//std
#include <algorithm>
#include <limits>
//CoRE4INET
#include "core4inet/base/CoRE4INET_Defs.h"
#include "core4inet/utilities/ConfigFunctions.h"
//Auto-generated Messages
#include "core4inet/linklayer/ethernet/base/EtherFrameWithQTag_m.h"
// 导入自建的message
#include "can4cqf/linklayer/message/IEEE8021QchTTFlow_m.h"
#include "can4cqf/linklayer/message/IEEE8021QchBEFlow_m.h"
namespace CAN4CQF {

Define_Module(IEEE8021QchQueueing);

void IEEE8021QchQueueing::initialize()
{
    this->handleParameterChange(nullptr);
}

void IEEE8021QchQueueing::handleParameterChange(const char* parname)
{
    if (!parname || !strcmp(parname, "untaggedVID"))
    {
        this->untaggedVID = static_cast<uint16_t>(CoRE4INET::parameterULongCheckRange(par("untaggedVID"), 0, MAX_VLAN_NUMBER));
    }
    if (!parname || !strcmp(parname, "taggedVIDs"))
    {
        this->taggedVIDs = cStringTokenizer(par("taggedVIDs"), ",").asIntVector();
        std::sort(taggedVIDs.begin(), taggedVIDs.end());
    }
    if (!parname || !strcmp(parname, "numPCP"))
    {
        this->numPCP = static_cast<unsigned int>(CoRE4INET::parameterULongCheckRange(par("numPCP"), 1, std::numeric_limits<int>::max()));
    }
    if (!parname || !strcmp(parname, "defaultPCP"))
    {
        this->defaultPCP = static_cast<unsigned int>(CoRE4INET::parameterULongCheckRange(par("defaultPCP"), 0, this->numPCP-1));
    }
}

void IEEE8021QchQueueing::handleMessage(cMessage *msg)
{
    if (msg->arrivedOn("in",0) || msg->arrivedOn("in",1))
    {
//        if (EthernetIIFrameWithQTag* qframe = dynamic_cast<EthernetIIFrameWithQTag*>(msg))
        if (IEEE8021QchTTFlow* qframe = dynamic_cast<IEEE8021QchTTFlow*>(msg))
        {
            //VLAN untag if requested
            if (this->untaggedVID && this->untaggedVID == qframe->getVID())
            {
                qframe->setVID(0);// 原来是0
            }
            //VLAN check if port is tagged with VLAN
            bool found = false;
            for (std::vector<int>::iterator vid = this->taggedVIDs.begin(); vid != this->taggedVIDs.end(); ++vid)
            {
                //Shortcut due to sorted vector
                if ((*vid) != qframe->getVID())
                {
                    found = true;
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
                delete qframe;
                return;
            }
            // 当输入为TT流，则发送到Rxgate中，也就是优先级为6和7的队列
            this->send(qframe, "out", qframe->getPcp());
        } //else if(IEEE8021QchBEFlow* qframe = dynamic_cast<IEEE8021QchTTFlow*>(msg)) // 以后补上
        else
        {
            this->send(msg, "out", this->defaultPCP);
        }
    }
}

} //namespace
