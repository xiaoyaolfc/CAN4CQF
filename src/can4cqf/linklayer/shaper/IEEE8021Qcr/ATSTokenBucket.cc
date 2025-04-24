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

#include "ATSTokenBucket.h"

namespace CAN4CQF {

Define_Module(ATSTokenBucket);

ATSTokenBucket::ATSTokenBucket() {
    sendPacketEvent = nullptr;
}

ATSTokenBucket::~ATSTokenBucket() {
    cancelAndDelete(sendPacketEvent);
}

void ATSTokenBucket::initialize() {
    bucketSize = par("bucketSize").doubleValue();
    tokenRate = par("tokenRate").doubleValue();
    maxBurstSize = par("maxBurstSize").doubleValue();
    tokens = bucketSize;  // 初始时桶是满的
    lastUpdateTime = simTime();
    sendPacketEvent = new cMessage("sendPacketEvent");
    packetQueue.setName("packetQueue");  // 初始化队列
}

void ATSTokenBucket::handleMessage(cMessage *msg) {
    if (msg == sendPacketEvent) {
            // 定时器触发时尝试处理队列中的包
            if (!packetQueue.isEmpty()) {
                cPacket *pkt = check_and_cast<cPacket *>(packetQueue.pop());
                processPacket(pkt);
            }
        } else {
            // 新数据包到达，加入队列并尝试处理
            cPacket *pkt = check_and_cast<cPacket *>(msg);
            packetQueue.insert(pkt);
            processPacket(pkt);
        }
}

void ATSTokenBucket::updateTokens() {
    simtime_t now = simTime();
    double newTokens = tokenRate * SIMTIME_DBL(now - lastUpdateTime); // 根据时间间隔计算新生成的令牌
    tokens = std::min(bucketSize, tokens + newTokens); // 令牌数不能超过桶的最大容量
    lastUpdateTime = now;
}

bool ATSTokenBucket::canSendPacket(cPacket *pkt) {
    updateTokens();
    double packetSize = pkt->getBitLength();
    if (tokens >= packetSize) {
        tokens -= packetSize;
        return true;
    }
    return false;
}

void ATSTokenBucket::processPacket(cPacket *pkt) {
    if (canSendPacket(pkt)) {
        send(pkt, "out");
    } else {
        // 如果令牌不足，则等待下一次尝试发送
        scheduleAt(simTime() + (pkt->getBitLength() / tokenRate), sendPacketEvent);
    }
}

} //namespace
