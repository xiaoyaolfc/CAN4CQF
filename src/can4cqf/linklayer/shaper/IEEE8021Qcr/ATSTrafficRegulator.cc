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

#include "ATSTrafficRegulator.h"

namespace CAN4CQF {

Define_Module(ATSTrafficRegulator);

ATSTrafficRegulator::ATSTrafficRegulator() {
    sendPacketEvent = nullptr;
}

ATSTrafficRegulator::~ATSTrafficRegulator() {
    cancelAndDelete(sendPacketEvent);
}

void ATSTrafficRegulator::initialize() {
    maxRate = par("maxRate").doubleValue();
    shapingDelay = par("shapingDelay").doubleValue();
    lastSendTime = simTime();
    sendPacketEvent = new cMessage("sendPacketEvent");
    packetQueue.setName("packetQueue"); // 初始化队列
}

void ATSTrafficRegulator::handleMessage(cMessage *msg) {
    if (msg == sendPacketEvent) {
        // 定时器触发时尝试发送队列中的包
        regulateTraffic();
    } else {
        // 新数据包到达，加入队列并尝试处理
        cPacket *pkt = check_and_cast<cPacket *>(msg);
        packetQueue.insert(pkt);
        regulateTraffic();
    }
}

void ATSTrafficRegulator::regulateTraffic() {
    if (packetQueue.isEmpty())
        return;

    cPacket *pkt = check_and_cast<cPacket *>(packetQueue.front());
    simtime_t now = simTime();
    simtime_t requiredInterval = pkt->getBitLength() / maxRate;

    if (now - lastSendTime >= requiredInterval) {
        // 可以发送
        send(pkt, "out");
        packetQueue.pop();
        lastSendTime = now;
    } else {
        // 需要延迟
        scheduleAt(lastSendTime + requiredInterval, sendPacketEvent);
    }
}

} //namespace
