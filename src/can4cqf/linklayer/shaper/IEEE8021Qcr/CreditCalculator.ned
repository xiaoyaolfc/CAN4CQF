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
simple CreditCalculator {
    parameters:
        double initialCredit @unit(b);   // 初始信用值（字节）
        double idleSlope @unit(b/s);     // 信用增长速率（IEEE 802.1Qcr参数）
        double sendSlope @unit(b/s);     // 信用消耗速率
        @class(CreditCalculator);        // 绑定C++类
        @display("i=block/boundedqueue");
    gates:
        input in;                        // 接收信用更新触发信号
        output out;                      // 输出当前信用值
}
