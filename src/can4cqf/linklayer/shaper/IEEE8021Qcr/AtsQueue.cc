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

#include "AtsQueue.h"

namespace CAN4CQF {

Define_Module(AtsQueue);

void AtsQueue::initialize() {
    capacity = par("queueSize").intValue();
}

void AtsQueue::handleMessage(cMessage *msg) {
    if (msg->arrivedOn("in")) {
        if (queue.size() < capacity) {
            queue.push(check_and_cast<cPacket *>(msg));
            EV << "Packet enqueued. Queue size: " << queue.size() << endl;
        } else {
            EV << "Queue full. Packet dropped." << endl;
            delete msg;
        }
    }
}

bool AtsQueue::canSendPacket(cPacket *pkt) {
    // 从CreditCalculator获取当前信用值
    double credit = getParentModule()->getSubmodule("creditCalc")
                    ->par("credit").doubleValue();
    return credit >= pkt->getByteLength();
}

} //namespace
