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

#include <can4cqf/linklayer/shaper/IEEE8021Qcr/ATSQueue.h>

namespace CAN4CQF {

Define_Module(ATSQueue);

void ATSQueue::initialize() {
    numQueues = par("numQueues").intValue();
    queueCapacity = par("queueCapacity").intValue();
    queues.resize(numQueues);
}

void ATSQueue::handleMessage(cMessage *msg) {
    cPacket *pkt = check_and_cast<cPacket *>(msg);
    int priority = pkt->getKind() % numQueues;  // 通过 `kind` 字段确定优先级

    if ((int)queues[priority].size() < queueCapacity) {
        queues[priority].push(pkt);
    } else {
        EV_WARN << "ATSQueue: 队列已满，丢弃数据包。\n";
        delete pkt;
    }
}

bool ATSQueue::isEmpty() {
    for (int i = 0; i < numQueues; i++) {
        if (!queues[i].empty()) return false;
    }
    return true;
}

int ATSQueue::getNumPackets() {
    int count = 0;
    for (auto& q : queues) {
        count += q.size();
    }
    return count;
}

cPacket* ATSQueue::getPacket(int index) {
    for (int i = 0; i < numQueues; i++) {
        if (!queues[i].empty()) {
            return queues[i].front();
        }
    }
    return nullptr;
}

cPacket* ATSQueue::removePacket(int index) {
    for (int i = 0; i < numQueues; i++) {
        if (!queues[i].empty()) {
            cPacket *pkt = queues[i].front();
            queues[i].pop();
            return pkt;
        }
    }
    return nullptr;
}

} //namespace
