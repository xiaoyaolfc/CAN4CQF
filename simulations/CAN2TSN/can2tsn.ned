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

package can4cqf.simulations.CAN2TSN;

import can4cqf.can4cqf.nodes.ethernet.TSNCQFSwitch;
import can4cqf.can4cqf.gateway.BusEthernetGateway;
import can4cqf.can4cqf.applications.TSNCANHost.TSNCANHost;
// 导入fico4omnet
import fico4omnet.bus.can.CanBus;
import fico4omnet.nodes.can.CanNode;
// 导入SignalsAndGateways
import signalsandgateways.applications.ethernet.TSNEthernetGatewayHost;

network Can2tsn
{
    parameters:
        @display("bgb=785,440,white");
    types:
        //
        // 所有链路均配置为每秒1000MBit，可变延时
        //
        channel cable extends ned.DatarateChannel
        {
            datarate = 1000Mbps;
            //bit error rate (probability (0.0-1.0) per bit)  
            ber = 0;
            //packet error rate (probability (0.0-1.0) per packet)  
            per = 0;
        }
    submodules:
        canBus1: CanBus {
            @display("p=190,238");
        }
        canNode1: CanNode {
            @display("p=91,237");
        }
        gateway1: Gateway1 {
            parameters:
                @display("p=275,238");
            gates:
                cang[1];
                ethg[1];
        }
        switch1: Switch1 {
            parameters:
                @display("p=358,238");
            gates:
                ethg[3];
        }
        switch2: Switch2 {
            parameters:
                @display("p=434,238");
            gates:
                ethg[2];
        }
        canNode2: CanNode {
            @display("p=714,237");
        }
        canBus2: CanBus {
            @display("p=607,238");
        }
        gateway2: Gateway1 {
            parameters:
                @display("p=515,238");
            gates:
                cang[1];
                ethg[1];
        }
        tsncanhost: TSNCANHost {
            @display("p=358,122");
        }
    connections:
        canNode1.gate <--> canBus1.gate++;
        canNode2.gate <--> canBus2.gate++;
        gateway1.cang[0] <--> canBus1.gate++;
        gateway1.ethg[0] <--> cable {  delay = 0ns; } <--> switch1.ethg[0];
        switch1.ethg[1] <--> cable {  delay = 0ns; } <--> switch2.ethg[0];
        switch1.ethg[2] <--> cable {  delay = 0ns; } <--> tsncanhost.gate;
        switch2.ethg[1] <--> cable {  delay = 0ns; } <--> gateway2.ethg[0];
        gateway2.cang[0] <--> canBus2.gate++;

}
