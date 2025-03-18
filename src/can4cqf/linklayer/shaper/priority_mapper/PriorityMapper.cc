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

#include "PriorityMapper.h"

namespace CAN4CQF {

Define_Module(PriorityMapper);

void PriorityMapper::handleMessage(cMessage *msg) {
    cPacket *pkt = check_and_cast<cPacket*>(msg);

    // 使用CoRE4INET的扩展控制信息获取机制
    if (auto ctrlInfo = dynamic_cast<ExtendedIeee802Ctrl*>(pkt->getControlInfo())) {
        int priority = mapDscpToPriority(ctrlInfo->getDscp());

        // 创建符合CoRE4INET标准的流控制标签
        auto flowCtrl = new CCFlowControlInfo();
        flowCtrl->setPriority(priority);
        pkt->addControlInfo(flowCtrl);
    }
    send(pkt, "out");
}

int PriorityMapper::mapDscpToPriority(int dscp) {
    // 示例映射规则（根据实际需求调整）
    if (dscp >= 40) return 7;  // 最高优先级
    else if (dscp >= 30) return 5;
    else return 3;
}

} //namespace
