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

#ifndef __CAN2CQF_ATSFLOWESTIMATOR_H_
#define __CAN2CQF_ATSFLOWESTIMATOR_H_

#include <omnetpp.h>

using namespace omnetpp;

namespace CAN4CQF {

class ATSFlowEstimator : public cSimpleModule {
  private:
    double linkRate;         // 链路速率（bps）
    double processingDelay;  // 处理延迟（秒）
    cMessage *sendPacketEvent; // 定时发送事件
    simtime_t estimatedTransmissionTime; // 预计传输时间
    cQueue packetQueue;      // 内部数据包队列

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    simtime_t computeTransmissionTime(cPacket *pkt); // 计算预计传输时间

  public:
    ATSFlowEstimator();
    virtual ~ATSFlowEstimator();
};


} //namespace

#endif
