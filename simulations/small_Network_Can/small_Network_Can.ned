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

package can4cqf.simulations.small_Network_Can;
// 使用了CAN节点，CAN bus和gateway的简单网络，TSN交换机使用CQF交换机，其余设置不变，暂时设置为一跳
import can4cqf.can4cqf.nodes.ethernet.TSNCQFSwitch;
import can4cqf.can4cqf.gateway.BusEthernetGateway;
// 导入fico4omnet
import fico4omnet.bus.can.CanBus;
import fico4omnet.nodes.can.CanNode;
// 导入SignalsAndGateways
import signalsandgateways.gateway.transformation.GatewayTransformation;
import signalsandgateways.gateway.buffering.AccumulationGatewayBuffering;
network Small_Network_Can
{
    parameters:
        @display("bgb=604,114,white");
    types:
        //
        // 所有链路均配置为每秒500MBit，可变延时
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
        canbus1: CanBus {
            @display("p=137,62");
        }
        canNode1: CanNode {
            @display("p=51,62");
        }
        canbus2: CanBus {
            @display("p=467,62");
        }
        canNode2: CanNode {
            @display("p=553,62");
        }
        gateway1: Gateway1 {
            @display("p=214,62");
            gates:
                cang[1];
                ethg[1];
        }

        gateway2: Gateway2 {
            @display("p=381,62");
            gates:
                cang[1];
                ethg[1];
        }

        switch: Switch {
            parameters:
                @display("p=295,62");
            gates:
                ethg[2];
        }

    connections:
        gateway1.ethg[0] <--> cable {  delay = 100ns; } <--> switch.ethg[0];
        gateway2.ethg[0] <--> cable {  delay = 50ns; } <--> switch.ethg[1];

        gateway1.cang[0] <--> canbus1.gate++;
        gateway2.cang[0] <--> canbus2.gate++;

        canbus1.gate++ <--> canNode1.gate;
        canbus2.gate++ <--> canNode2.gate;
}
