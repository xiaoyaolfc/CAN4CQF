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

package can4cqf.simulations.ats_rl_network;
// 导入fico4omnet
import fico4omnet.bus.can.CanBus;
import fico4omnet.nodes.can.CanNode;
// 导入SignalsAndGateways
import signalsandgateways.applications.ethernet.TSNEthernetGatewayHost;
// 导入can2cqf定义的ATS
import can4cqf.can4cqf.nodes.ethernet.TSNCQFSwitch;
import can4cqf.can4cqf.gateway.BusEthernetGateway;
// 导入core4ine
import core4inet.nodes.ethernet.IEEE8021Qbv.IEEE8021QbvEtherHost;


network Ats_rl_network
{
    parameters:
        @display("bgb=923,518,white");
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
        switch1: Switch1 {
            parameters:
                @display("p=342,236");
            gates:
                ethg[4];
        }
        switch2: Switch2 {
            parameters:
                @display("p=430,303");
            gates:
                ethg[5];
        }
        switch3: Switch3 {
            parameters:
                @display("p=430,169");
            gates:
                ethg[5];
        }
        switch4: Switch4 {
            parameters:
                @display("p=430,236");
            gates:
//                ethg[2];
				ethg[4];
        }
        switch5: Switch5 {
            parameters:
                @display("p=520,236");
            gates:
                ethg[4];
        }
        gateway1: Gateway1 {
            parameters:
                @display("p=245,236");
            gates:
                cang[1];
                ethg[1];
        }
        gateway2: Gateway2 {
            parameters:
                @display("p=615,236");
            gates:
                cang[1];
                ethg[1];
        }
        canBus1: CanBus {
            @display("p=150,236");
        }
        canBus2: CanBus {
            @display("p=715,236");
        }
        canNode1: CanNode {
            @display("p=57,104");
        }
        canNode2: CanNode {
            @display("p=57,189");
        }
        canNode3: CanNode {
            @display("p=57,275");
        }
        canNode4: CanNode {
            @display("p=57,360");
        }
        canNode5: CanNode {
            @display("p=829,363");
        }
        canNode6: CanNode {
            @display("p=829,107");
        }
        canNode7: CanNode {
            @display("p=829,192");
        }
        canNode8: CanNode {
            @display("p=829,278");
        }
        tsnhost1: IEEE8021QbvEtherHost {
            @display("p=342,401");
        }
        tsnhost2: IEEE8021QbvEtherHost {
            @display("p=529,401");
        }
        tsnhost3: IEEE8021QbvEtherHost {
            @display("p=342,64");
        }
        tsnhost4: IEEE8021QbvEtherHost {
            @display("p=529,64");
        }
        
    connections:
        canNode1.gate <--> canBus1.gate++;
        canNode2.gate <--> canBus1.gate++;
        canNode3.gate <--> canBus1.gate++;
        canNode4.gate <--> canBus1.gate++;

        canNode5.gate <--> canBus2.gate++;
        canNode6.gate <--> canBus2.gate++;
        canNode7.gate <--> canBus2.gate++;
        canNode8.gate <--> canBus2.gate++;

	    //	连接网关   		
        gateway1.cang[0] <--> canBus1.gate++;
        gateway2.cang[0] <--> canBus2.gate++;

        gateway1.ethg[0] <--> cable {  delay = 10ns; } <--> switch1.ethg[0];
        gateway2.ethg[0] <--> cable {  delay = 10ns; } <--> switch5.ethg[0];
        // 交换机之间连接
        switch1.ethg[1] <--> cable {  delay = 100ns; } <--> switch2.ethg[0];
        switch1.ethg[2] <--> cable {  delay = 100ns; } <--> switch3.ethg[0];
        switch1.ethg[3] <--> cable {  delay = 100ns; } <--> switch5.ethg[1];
        
        switch2.ethg[1] <--> cable {  delay = 100ns; } <--> switch5.ethg[2];
        switch2.ethg[2] <--> cable {  delay = 100ns; } <--> switch4.ethg[0];

		switch3.ethg[1] <--> cable {  delay = 100ns; } <--> switch5.ethg[3];
        switch3.ethg[2] <--> cable {  delay = 100ns; } <--> switch4.ethg[1];
        // host连接
        tsnhost1.ethg <--> cable {  delay = 10ns; } <--> switch2.ethg[3];
        tsnhost2.ethg <--> cable {  delay = 10ns; } <--> switch2.ethg[4];
        tsnhost3.ethg <--> cable {  delay = 10ns; } <--> switch3.ethg[3];
        tsnhost4.ethg <--> cable {  delay = 10ns; } <--> switch3.ethg[4];
}
