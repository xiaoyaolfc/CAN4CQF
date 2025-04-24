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
// ATSFlowEstimator: 计算数据包的预计传输时间，并决定何时允许其通过
//
simple ATSFlowEstimator {
    parameters:
        double linkRate @unit(Gbps) = default(1Gbps); // 链路速率
        double processingDelay @unit(us) = default(10us); // 处理延迟
        @display("i=block/timer");

    gates:
        input in @labels(Packet);  // 输入数据包
        output out @labels(Packet); // 输出数据包
}