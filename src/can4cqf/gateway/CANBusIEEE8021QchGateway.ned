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

package can4cqf.can4cqf.gateway;

import signalsandgateways.applications.can.CanGatewayNode;
// 导入can2cqfagg
import can4cqf.can4cqf.applications.ethernet.IEthernetGatewayHost;
import can4cqf.can4cqf.gateway.GatewayBase;
// 为论文仿真网关的核心模块
// 目的：引入CQF作为以太网的“终端”，该gateway由三部分组成：CanNode,transformation,TSN module; CanNode应该是不用改的，tran..是后面
// 需要大改的部分。我想使用CQF作为确定性传输机制
module CANBusIEEE8021QchGateway
{
    parameters:
        @display("i=block/layer;bgb=548,231");

        //Number of CAN bus ports
        int countOfCanBusses = default(1);
        //Number of Ethernet ports
        int countOfEthernetPorts = default(1);

        //Gateway ID in gateway config file. If auto or empty the gateway module name will be used.
        string gatewayID = default("auto");
        *.gatewayID = gatewayID;

    gates:
        //ethernet connection
        inout ethg[countOfEthernetPorts];
        //bus connection
        inout cang[countOfCanBusses];

    submodules:
        //bus module
        can[countOfCanBusses]: CanGatewayNode {
            @display("p=460,97,c,130");
        }

        //ethernet module
        tsn[countOfEthernetPorts]: <default("TSNCQFEthernetGatewayHost")> like IEthernetGatewayHost {
            @display("p=93,97,c,130");
        }

        //gateway module
        gateway: GatewayBase {
            @display("p=276,97");
            numPorts = countOfCanBusses + countOfEthernetPorts;
        }

    connections:
        for i=0..countOfCanBusses-1 {
            can[i].upperLayerOut --> gateway.lowerLayerIn++;
            can[i].upperLayerIn <-- gateway.lowerLayerOut++;
            can[i].gate <--> cang++;
        }
        for i=0..countOfEthernetPorts-1 {
            tsn[i].upperLayerOut --> gateway.lowerLayerIn++;
            tsn[i].upperLayerIn <-- gateway.lowerLayerOut++;
            tsn[i].ethg <--> ethg++;
        }        
}
