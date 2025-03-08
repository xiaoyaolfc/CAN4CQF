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

package can4cqf.simulations.big_network_baseline;

import can4cqf.can4cqf.nodes.ethernet.TSNCQFSwitch;
import can4cqf.can4cqf.gateway.BusEthernetGateway;
// 导入fico4omnet
import fico4omnet.bus.can.CanBus;
import fico4omnet.nodes.can.CanNode;
//	使用16个ECU，3个交换机

network Big_network_baseline
{
    parameters:
        @display("bgb=1013,497,white");
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
            @display("p=186,216");
        }
        canBus2: CanBus {
            @display("p=739,216");
        }
        canNode1: CanNode {
            @display("p=194,38");
        }
        canNode2: CanNode {
            @display("p=122,49");
        }
        canNode3: CanNode {
            @display("p=65,96");
        }
        canNode4: CanNode {
            @display("p=50,172");
        }
        canNode5: CanNode {
            @display("p=50,248");
        }
        canNode6: CanNode {
            @display("p=65,307");
        }
        canNode7: CanNode {
            @display("p=122,357");
        }
        canNode8: CanNode {
            @display("p=194,374");
        }
        canNode9: CanNode {
            @display("p=719,38");
        }
        canNode10: CanNode {
            @display("p=803,49");
        }
        canNode11: CanNode {
            @display("p=869,96");
        }
        canNode12: CanNode {
            @display("p=908,172");
        }
        canNode13: CanNode {
            @display("p=918,248");
        }
        canNode14: CanNode {
            @display("p=879,307");
        }
        canNode15: CanNode {
            @display("p=803,357");
        }
        canNode16: CanNode {
            @display("p=719,374");
        }
        gateway1: Gateway1 {
            parameters:
                @display("p=271,216");
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
                @display("p=372,216");
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
   		
   		canNode9.gate <--> canBus2.gate++;
   		canNode10.gate <--> canBus2.gate++;
   		canNode11.gate <--> canBus2.gate++;
   		canNode12.gate <--> canBus2.gate++;
   		canNode13.gate <--> canBus2.gate++;
   		canNode14.gate <--> canBus2.gate++;    		 
   		canNode15.gate <--> canBus2.gate++;
   		canNode16.gate <--> canBus2.gate++; 
//	连接网关   		
        gateway1.cang[0] <--> canBus1.gate++;
        gateway2.cang[0] <--> canBus2.gate++;
//	连接交换机
		gateway1.ethg[0] <--> cable {  delay = 10ns; } <--> switch1.ethg[0];
        gateway2.ethg[0] <--> cable {  delay = 10ns; } <--> switch3.ethg[1];       
        switch1.ethg[1] <--> cable{delay = 100ns; } <--> switch2.ethg[0];
        switch2.ethg[1] <--> cable{delay = 100ns; } <--> switch3.ethg[0];      
}
