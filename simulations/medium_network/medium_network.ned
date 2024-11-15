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

package can4cqf.simulations.medium_network;
// 使用了CAN节点，CAN bus和gateway的简单网络，TSN交换机使用CQF交换机，其余设置不变，暂时设置为一跳
import can4cqf.can4cqf.nodes.ethernet.TSNCQFSwitch;
import can4cqf.can4cqf.gateway.BusEthernetGateway;
// 导入fico4omnet
import fico4omnet.bus.can.CanBus;
import fico4omnet.nodes.can.CanNode;

// 导入CAN4CQF
//import can4cqf.can4cqf.

network Medium_network
{
    parameters:
        @display("bgb=839,188,white");
    types:
        //
        // 所有链路均配置为每秒100MBit，可变延时
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
            @display("p=170,80");
        }
        canBus2: CanBus {
            @display("p=654,80");
        }
        canNode1: CanNode {
            @display("p=50,30");
        }
        canNode2: CanNode {
            @display("p=50,90");
        }
        canNode3: CanNode {
            @display("p=50,150");
        }
        canNode4: CanNode {
            @display("p=770,30");
        }
        canNode5: CanNode {
            @display("p=770,90");
        }
        canNode6: CanNode {
            @display("p=770,150");
        }
        gateway1: Gateway1 {
			parameters:
            	@display("p=255,80");
           	gates:
           	    cang[1];
           	    ethg[1];
        }
        gateway2: Gateway2 {
            @display("p=561,80");
            gates:
                cang[1];
                ethg[1];
        }
        switch1: Switch1 {
            parameters:
            	@display("p=356,80");
            gates:
                ethg[2];
        }
        switch2: Switch2 {
			parameters:
            	@display("p=465,80");
			gates:
			    ethg[2];            	
        }
	connections:
	    // CAN网络相连
   		canNode1.gate <--> canBus1.gate++;
   		canNode2.gate <--> canBus1.gate++;
   		canNode3.gate <--> canBus1.gate++;      
   		
   		canNode4.gate <--> canBus2.gate++;
   		canNode5.gate <--> canBus2.gate++;
   		canNode6.gate <--> canBus2.gate++;  
   		
        gateway1.cang[0] <--> canBus1.gate++;
        gateway2.cang[0] <--> canBus2.gate++;   	
        // 感觉下面的连接方式有问题，更改一下
//        gateway1.ethg[0] <--> cable {  delay = 10ns; } <--> switch1.ethg[0];
//        gateway2.ethg[0] <--> cable {  delay = 10ns; } <--> switch2.ethg[0];       
//        switch1.ethg[1] <--> cable{delay = 100ns; } <--> switch2.ethg[1];     	
		gateway1.ethg[0] <--> cable {  delay = 10ns; } <--> switch1.ethg[0];
        gateway2.ethg[0] <--> cable {  delay = 10ns; } <--> switch2.ethg[1];       
        switch1.ethg[1] <--> cable{delay = 100ns; } <--> switch2.ethg[0];           
        
}
