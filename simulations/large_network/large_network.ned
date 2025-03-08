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

package can4cqf.simulations.large_network;

import can4cqf.can4cqf.nodes.ethernet.TSNCQFSwitch;
import can4cqf.can4cqf.gateway.BusEthernetGateway;
// 导入fico4omnet
import fico4omnet.bus.can.CanBus;
import fico4omnet.nodes.can.CanNode;
//	使用16个ECU，3个交换机
network Large_network
{
    parameters:
        @display("bgb=1013,497,white");
    types:
        //
        // 所有链路均配置为每秒100MBit，可变延时
        //
        channel cable extends ned.DatarateChannel
        {
            datarate = 100Mbps;
            //bit error rate (probability (0.0-1.0) per bit)  
            ber = 0;
            //packet error rate (probability (0.0-1.0) per packet)  
            per = 0;
        }
    submodules:
        canBus1: CanBus {
            @display("p=186,216");
        }
        canBus2: CanBus {
            @display("p=739,216");
        }
        canNode1: CanNode {
            @display("p=270.77625,62.29125");
        }
        canNode2: CanNode {
            @display("p=185.6025,36.86625");
        }
        canNode3: CanNode {
            @display("p=82.63125,62.29125");
        }
        canNode4: CanNode {
            @display("p=40.68,143.65125");
        }
        canNode5: CanNode {
            @display("p=40.68,213.56999");
        }
        canNode6: CanNode {
            @display("p=40.68,322.8975");
        }
        canNode7: CanNode {
            @display("p=82.63125,373.7475");
        }
        canNode8: CanNode {
            @display("p=185.6025,397.90125");
        }
        canNode9: CanNode {
            @display("p=270.77625,373.7475");
        }
        canNode10: CanNode {
            @display("p=303.82874,307.6425");
        }
        canNode11: CanNode {
            @display("p=612.7425,69.91875");
        }
        canNode12: CanNode {
            @display("p=738.59625,36.86625");
        }
        canNode13: CanNode {
            @display("p=874.62,62.29125");
        }
        canNode14: CanNode {
            @display("p=914.02875,143.65125");
        }
        canNode15: CanNode {
            @display("p=935.63995,213.56999");
        }
        canNode16: CanNode {
            @display("p=914.02875,289.845");
        }
        canNode17: CanNode {
            @display("p=874.62,364.84875");
        }
        canNode18: CanNode {
            @display("p=738.59625,406.8");
        }
        canNode19: CanNode {
            @display("p=663.59247,373.7475");
        }
        canNode20: CanNode {
            @display("p=612.7425,322.8975");
        }
        gateway1: Gateway1 {
            parameters:
                @display("p=291.11624,214.84125");
            gates:
                cang[1];
                ethg[1];
        }
        gateway2: Gateway2 {
            @display("p=646,216");
            gates:
                cang[1];
                ethg[1];
        }
        switch1: Switch1 {
            parameters:
                @display("p=378.8325,214.84125");
            gates:
                ethg[2];
        }
        switch2: Switch2 {
            parameters:
                @display("p=472,216");
            gates:
                ethg[2];
        }
        switch3: Switch3 {
            parameters:
                @display("p=564,216");
            gates:
                ethg[2];
        }
    connections:
        canNode1.gate <--> canBus1.gate++;
        canNode2.gate <--> canBus1.gate++;
        canNode3.gate <--> canBus1.gate++;
        canNode4.gate <--> canBus1.gate++;
        canNode5.gate <--> canBus1.gate++;
        canNode6.gate <--> canBus1.gate++;
        canNode7.gate <--> canBus1.gate++;
        canNode8.gate <--> canBus1.gate++;
        canNode9.gate <--> canBus1.gate++;
        canNode10.gate <--> canBus1.gate++;

        canNode11.gate <--> canBus2.gate++;
        canNode12.gate <--> canBus2.gate++;
        canNode13.gate <--> canBus2.gate++;
        canNode14.gate <--> canBus2.gate++;
        canNode15.gate <--> canBus2.gate++;
        canNode16.gate <--> canBus2.gate++;
        canNode17.gate <--> canBus2.gate++;
        canNode18.gate <--> canBus2.gate++;
        canNode19.gate <--> canBus2.gate++;
        canNode20.gate <--> canBus2.gate++;
        //	连接网关   		
        gateway1.cang[0] <--> canBus1.gate++;
        gateway2.cang[0] <--> canBus2.gate++;
        //	连接交换机
        gateway1.ethg[0] <--> cable {  delay = 10ns; } <--> switch1.ethg[0];
        gateway2.ethg[0] <--> cable {  delay = 10ns; } <--> switch3.ethg[1];
        switch1.ethg[1] <--> cable {  delay = 100ns; } <--> switch2.ethg[0];
        switch2.ethg[1] <--> cable {  delay = 100ns; } <--> switch3.ethg[0];
}
