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

package can4cqf.can4cqf.linklayer.shaper.IEEE8021Qcr;

import can4cqf.can4cqf.linklayer.shaper.IEEE8021Qcr.queue.PacketQueueBase;

//
// ATSQueue: ATS 队列模块，用于存储和按优先级调度数据包
//
simple ATSQueue {
    parameters:
        int numQueues = default(8);
        int queueCapacity = default(100);
        @display("i=block/queue");
    gates:
        // 定义输入门为向量，便于多个上层输入合并
        input in[2] @labels(Packet);
        output out @labels(Packet);
}