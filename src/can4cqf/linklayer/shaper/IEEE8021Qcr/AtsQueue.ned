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

//
// TODO auto-generated module
//
simple AtsQueue {
    parameters:
        int queueSize @unit(pkts);       // 队列容量
        @class(AtsQueue);
        @display("i=block/queue");
    gates:
        input in;                        // 数据包输入
        output out;                      // 数据包输出（受信用控制）
}