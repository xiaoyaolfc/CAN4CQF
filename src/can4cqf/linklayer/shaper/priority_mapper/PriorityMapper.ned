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

package can4cqf.can4cqf.linklayer.shaper.priority_mapper;

//
// TODO auto-generated module
//
simple PriorityMapper {
    parameters:
        string dscpToPriorityMap; // DSCP到优先级的映射表（格式："40:7,30:5,default:3"）
        @class(PriorityMapper);  // 绑定C++实现类
        @display("i=block/classifier2_vl");
        
    gates:
        input in;             // 输入门（连接流量分类器）
        output out;           // 输出门（连接整形器队列）
}
