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

#ifndef __CAN2CQF_ATSTOKENBUCKET_H_
#define __CAN2CQF_ATSTOKENBUCKET_H_

#include <omnetpp.h>

using namespace omnetpp;

namespace CAN4CQF {

/**
 * TODO - Generated class
 */
class ATSTokenBucket : public cSimpleModule {
  private:
    double bucketSize;    // 令牌桶的最大容量（bit）
    double tokenRate;     // 令牌生成速率（bps）
    double maxBurstSize;  // 最大突发流量（bit）
    double tokens;        // 当前桶中的令牌数
    simtime_t lastUpdateTime; // 上次更新令牌的时间
    cMessage *sendPacketEvent; // 定时发送事件
    cQueue packetQueue;  // 添加内部队列

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void updateTokens();  // 更新令牌数
    bool canSendPacket(cPacket *pkt);  // 判断是否可以发送数据包
    void processPacket(cPacket *pkt);  // 处理数据包

  public:
    ATSTokenBucket();
    virtual ~ATSTokenBucket();
};


} //namespace

#endif
