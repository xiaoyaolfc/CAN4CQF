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

//import can4cqf.can4cqf.nodes.can.CanNode;
import fico4omnet.nodes.can.CanNode;
import fico4omnet.bus.can.CanBus;
import can4cqf.can4cqf.gateway.CANBusIEEE8021QchGateway;

module TSNCANHost
{
    parameters:
        @display("i=device/device,bgb,250,185;bgb=379,312");
    gates:
        inout gate;  // 输出门
    submodules:
        canNode: CanNode {
            @display("p=294,139");
        }
        gateway: CANBusIEEE8021QchGateway {
            @display("p=89,140");
        }
        canbus: CanBus {
            @display("p=199,140");
        }
    connections:
        gate <--> gateway.ethg[0];  // 外部网络接口
        gateway.cang[0] <--> canbus.gate++;
        canbus.gate++ <--> canNode.gate;  // 独立CAN接口
}
