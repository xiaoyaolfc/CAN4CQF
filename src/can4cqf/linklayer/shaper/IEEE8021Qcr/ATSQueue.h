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

#ifndef __CAN2CQF_ATSQUEUE_H_
#define __CAN2CQF_ATSQUEUE_H_

#pragma once
#include <omnetpp.h>
#include<vector>
#include<queue>
#include "queue/PacketQueueBase.h"  // 兼容 INET 3.8.3 的 PacketQueueBase

using namespace omnetpp;
using namespace std;
namespace CAN4CQF {

/**
 * TODO - Generated class
 */
class ATSQueue : public PacketQueueBase {
  protected:
    int numQueues;
    int queueCapacity;
    std::vector<std::queue<cPacket*>> queues;

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual bool isEmpty() override;
    virtual int getNumPackets() override;
    virtual cPacket* getPacket(int index) override;
    virtual cPacket* removePacket(int index) override;
};

} //namespace

#endif
