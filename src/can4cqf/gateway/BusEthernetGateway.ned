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

import signalsandgateways.gateway.GatewayBase;
import signalsandgateways.applications.can.CanGatewayNode;
// 导入can4cqf
import can4cqf.can4cqf.applications.ethernet.IEthernetGatewayHost;

module BusEthernetGateway
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
        tsn_cqf[countOfEthernetPorts]: <default("TSNCQFEthernetGatewayHost")> like IEthernetGatewayHost {
            @display("p=110,97,c,130");
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
            tsn_cqf[i].upperLayerOut --> gateway.lowerLayerIn++;
            tsn_cqf[i].upperLayerIn <-- gateway.lowerLayerOut++;
            tsn_cqf[i].ethg <--> ethg++;
        }    
}
