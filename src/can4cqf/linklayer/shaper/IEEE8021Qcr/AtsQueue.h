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
#include <queue>
#include <omnetpp.h>
#include "core4inet/buffer/base/QueueBuffer.h"

using namespace omnetpp;

namespace CAN4CQF {

/**
 * TODO - Generated class
 */
class AtsQueue : public cSimpleModule
{
private:
    std::queue<cPacket *> queue;         // 数据包队列
    int capacity;                        // 最大队列长度

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

  public:
    // 函数功能：检查是否可以发送数据（信用是否足够）
    // 返回值：true表示允许发送
    bool canSendPacket(cPacket *pkt);
};

} //namespace

#endif
