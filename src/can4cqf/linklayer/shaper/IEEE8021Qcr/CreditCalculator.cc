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

#include "CreditCalculator.h"
#include <algorithm>

namespace CAN4CQF {

Define_Module(CreditCalculator);

void CreditCalculator::initialize() {
    credit = par("initialCredit").doubleValue();
    idleSlope = par("idleSlope").doubleValue();
    sendSlope = par("sendSlope").doubleValue();
    lastUpdate = simTime();

    // 每1ms触发一次信用更新
    cMessage *timer = new cMessage("CreditUpdate");
    scheduleAt(simTime() + 0.001, timer);
}

void CreditCalculator::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage()) {
        // 周期性更新信用值（即使没有流量）
        updateCredit(false);
        scheduleAt(simTime() + 0.001, msg);
    }
}

// 更新后的信用计算逻辑（需与消息定义同步）
double CreditCalculator::updateCredit(bool isSending) {
    simtime_t now = simTime();
    double elapsed = (now - lastUpdate).dbl();

    // 计算逻辑保持不变
    if (isSending) {
        credit += elapsed * sendSlope;
    } else {
        credit += elapsed * idleSlope;
    }

    // 使用标准库或自定义clamp
    double maxCredit = par("maxCredit").doubleValue();
    credit = clamp(credit, -maxCredit, maxCredit);   // C++17标准下有效

    // 发送消息需匹配消息类接口
    CreditMessage *creditMsg = new CreditMessage("CreditUpdate");
    creditMsg->setCreditValue(credit);  // 必须与消息类定义的setter名称一致
    send(creditMsg, "out");

    lastUpdate = now;
    return credit;
}

} //namespace
