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

package can4cqf.can4cqf.nodes.ethernet;

// 导入CQF
import can4cqf.can4cqf.linklayer.port.IEEE8021QcrPHYPort;
// INET
import inet.linklayer.contract.IMACAddressTable;
import inet.linklayer.contract.IMACRelayUnit;
import inet.common.lifecycle.NodeStatus;
import inet.networklayer.common.InterfaceTable;
// 导入CoRE
//import core4inet.linklayer.port.TSNPHYPort;
import core4inet.scheduler.ITTEScheduler;
import core4inet.synchronisation.base.ISync;
import core4inet.utilities.modules.CheckedBuffer;
import core4inet.services.avb.SRP.SRPTable;
import core4inet.services.avb.SRP.SRProtocol;
import core4inet.linklayer.beHandling.avb.relay.SRPRelay;
import core4inet.incoming.IEEE8021Qbv.IEEE8021QbvIncoming;
// 目标：在TSN交换机中实现使用CQF作为调度机制
// CQF主要功能：使用两个高优先级队列Q7、Q6传输TT流，在GCL门控的奇数time-slot时Q7发送数据，Q6接收数据；在偶数time-slot时Q6发送数据，Q7接收数据。

module TSNATSSwitch
{
	parameters:
	    @display("bgb=,;i=block/switch");
        @networkNode;
        @labels(node,ethernet-node);
        // Optional: Number of Buffers with Incoming Module for TT and RC traffic 
        // (When not manually declared in own deriving .ned file)
        int numBuffer = default(0);
        // Forwardingdelay of the device hardware
        double hardware_delay @unit(s) = default(8us);
        *.interfaceTableModule = default(absPath(".interfaceTable"));

    gates:
        // Physical ports of the switch
        inout ethg[] @loose @labels(EtherFrame-conn);

    types:
        channel ProcessingDelay extends ned.DelayChannel
        {
        }

    submodules:
        // Keeps track of the status of network node (up, down, etc.)
        status: NodeStatus {
            @display("p=100,50;is=n");
        }

        // Implementation of the forwarding part
        beswitch: SRPRelay {
            parameters:
                @display("p=100,350");
            gates:
                ifIn[sizeof(parent.ethg)];
                ifOut[sizeof(parent.ethg)];
        }

        //The mac table associated with the beswitch
        macTable: <default("MACAddressTable")> like IMACAddressTable {
            @display("p=100,250");
        }

        // Holds interface names
        interfaceTable: InterfaceTable {
            @display("p=200,250");
        }

        // Physical ports of the switch
        phy[sizeof(ethg)]: IEEE8021QcrPHYPort {
            parameters:
                @display("p=100,450,row,100");
                //For the switch it is necessary to have promiscuous mode enabled
                promiscuous = true;
        }

        // AVB critical traffic control
        avbCTC: IEEE8021QbvIncoming {
            parameters:
                @display("p=500,250");
                hardware_delay = parent.hardware_delay;
        }

        // Sync module for synchronisation of the scheduler
        sync: <default("DummySync")> like ISync {
            parameters:
                @display("p=300,50");
        }

        // Scheduler for the device
        scheduler: <default("Scheduler")> like ITTEScheduler {
            parameters:
                @display("p=200,50");
        }

        // Buffers with Incoming Module for TT and RC traffic (optional)
        checkedBuffer[numBuffer]: CheckedBuffer {
            @display("p=600,150");
        }

        // Stream Reservation Protocol table
        srpTable: SRPTable {
            @display("p=400,250");
        }

        // Stream Reservation Protocol implementation
        srpProtocol: SRProtocol {
            @display("p=300,250");
        }

    connections:
        for i=0..sizeof(ethg)-1 {
            phy[i].phys <--> ethg[i];
            phy[i].upperLayerIn <-- ProcessingDelay {  delay = (parent.hardware_delay/2); } <-- beswitch.ifOut[i];
            phy[i].upperLayerOut --> ProcessingDelay {  delay = (parent.hardware_delay/2);} --> beswitch.ifIn[i];
        }

        srpProtocol.out --> beswitch.srpIn;
        srpProtocol.in <-- beswitch.srpOut;	      
}