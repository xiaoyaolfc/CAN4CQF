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

package can4cqf.can4cqf.applications.TSNCANHost;

//
// TODO auto-generated module
//
simple Transformation
{
    parameters:
        @display("i=block/transport");
        // T_CQF队列切换周期
        double timeout @unit(s) = default(0.0002s);        
    gates:
		inout extGate;  // 对外部网络的接口
        inout canGate;  // 对CAN节点的独立接口
}
