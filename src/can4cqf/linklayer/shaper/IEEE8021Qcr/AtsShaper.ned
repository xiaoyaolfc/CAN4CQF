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
import can4cqf.can4cqf.linklayer.shaper.IEEE8021Qcr.AtsQueue;
import can4cqf.can4cqf.linklayer.shaper.IEEE8021Qcr.CreditCalculator;
import can4cqf.can4cqf.linklayer.shaper.priority_mapper.PriorityMapper;

module AtsShaper {
    parameters:
        @class(AtsShaper);
    gates:
        input in;          // 输入端口（连接网络）
        output out;        // 输出端口（连接物理链路）
        input creditIn @directIn;  // 门类型改为纯输入
        output creditOut;          // 输出门声

    submodules:
        creditCalc: CreditCalculator {
            @display("p=100,100");
        }
        queue: AtsQueue {
            @display("p=200,100");
        }
        priMapper: PriorityMapper;  // 优先级映射模块

    connections:
        // 修正为独立路径
        creditCalc.out --> creditOut;  // 信用值输出到外部
        in --> queue.in;               // 数据输入保持原样
        queue.out --> out;             // 数据输出到物理端口
}