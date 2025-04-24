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

#include "ATSFlowEstimator.h"

namespace CAN4CQF {

Define_Module(ATSFlowEstimator);

ATSFlowEstimator::ATSFlowEstimator() {
    sendPacketEvent = nullptr;
}

ATSFlowEstimator::~ATSFlowEstimator() {
    cancelAndDelete(sendPacketEvent);
}

void ATSFlowEstimator::initialize() {
    linkRate = par("linkRate").doubleValue();
    processingDelay = par("processingDelay").doubleValue();
    sendPacketEvent = new cMessage("sendPacketEvent");
    cQueue packetQueue;      // 内部数据包队列
}

void ATSFlowEstimator::handleMessage(cMessage *msg) {
    if (msg == sendPacketEvent) {
        // 定时器触发时发送队列中的第一个包
        if (!packetQueue.isEmpty()) {
            cPacket *pkt = check_and_cast<cPacket *>(packetQueue.pop());
            send(pkt, "out");
        }
    } else {
        // 新数据包到达，加入队列并计算传输时间
        cPacket *pkt = check_and_cast<cPacket *>(msg);
        packetQueue.insert(pkt);
        estimatedTransmissionTime = computeTransmissionTime(pkt);
        scheduleAt(simTime() + estimatedTransmissionTime, sendPacketEvent);
    }
}

simtime_t ATSFlowEstimator::computeTransmissionTime(cPacket *pkt) {
    double transmissionTime = pkt->getBitLength() / linkRate;
    return transmissionTime + processingDelay;
}
} //namespace
