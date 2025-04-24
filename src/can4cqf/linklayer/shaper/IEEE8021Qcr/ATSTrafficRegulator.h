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

#ifndef __CAN2CQF_ATSTRAFFICREGULATOR_H_
#define __CAN2CQF_ATSTRAFFICREGULATOR_H_

#include <omnetpp.h>
#include <queue> // 添加队列支持

using namespace omnetpp;

namespace CAN4CQF {

/**
 * @brief - 负责控制数据流的速率，确保数据不会突发超载，而是按照 ATS 机制 平稳流动。
 */
class ATSTrafficRegulator : public cSimpleModule {
  private:
    double maxRate;         // 最大流量速率（bps）
    double shapingDelay;    // 整形延迟（秒）
    simtime_t lastSendTime; // 上一次发送时间
    cMessage *sendPacketEvent; // 定时发送事件
    cQueue packetQueue;     // 内部数据包队列

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void regulateTraffic();

  public:
    ATSTrafficRegulator();
    virtual ~ATSTrafficRegulator();
};


} //namespace

#endif
