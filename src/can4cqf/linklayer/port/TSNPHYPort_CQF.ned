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

package can4cqf.can4cqf.linklayer.port;
// 该模块实现TSN物理层+数据链路层的端口，包括了输入整形器、输出整形器、MAC转发
// 使用在TSN的Switch上
// 导入CoRE
import core4inet.linklayer.inControl.IEEE8021Q.IEEE8021Q_InControl;
//import core4inet.linklayer.inControl.AVB_8021Q_InControl;
import core4inet.linklayer.filtering.IFiltering;
import can4cqf.can4cqf.linklayer.shaper.IEEE8021Qch.IEEE8021Qch_shaper;
// 导入INET
import inet.linklayer.ethernet.EtherMACFullDuplex;
//导入CAN4CQF
import can4cqf.can4cqf.linklayer.inControl.AVB_8021Q_InControl;

module TSNPHYPort_CQF
{
	parameters:
        @display("bgb=272,276;i=device/port");

        // if true, all packets are received, otherwise only the ones with matching destination MAC address
        bool promiscuous = default(false);
        //Static propagation delay for the port
        double static_tx_delay @unit(s) = default(0ns);
        //Module holds Interfacenames
        string interfaceTableModule;
        //Untagged VLAN. Untagged incoming frames get tagged with this VLAN. Outgoing frames with this VLAN get untagged
        int untaggedVID = default(0);
        //Interface allows frames with this VLAN to enter and leave the port
        string taggedVIDs = default("0");

        *.interfaceTableModule = default(absPath(this.interfaceTableModule));

    gates:
        //Input from the upper layer
        input upperLayerIn @loose @labels(EtherFrame);
        //Output to the upper layer
        output upperLayerOut @loose @labels(EtherFrame);
        //Direct input from the upper layer
        input upperLayerDirectIn @directIn @labels(EtherFrame);
        //Connection to the physical layer
        inout phys @labels(EtherFrame);

    submodules:
        //Module for the organisation of outbound traffic
        shaper: IEEE8021Qch_shaper {
            @display("p=48,45");
            untaggedVID = parent.untaggedVID;
            taggedVIDs = parent.taggedVIDs;
        }

        //Module for the organisation of inbound traffic
        inControl: AVB_8021Q_InControl {
            parameters:
                @display("p=195,45");
                promiscuous = parent.promiscuous;
                untaggedVID = parent.untaggedVID;
                taggedVIDs = parent.taggedVIDs;
        }

        filtering: <default("NoFiltering")> like IFiltering {
            @display("p=195,110");
        }

        //MAC interface
        mac: EtherMACFullDuplex {
            parameters:
                promiscuous = true;
                queueModule = "^.shaper.transmissionSelection";
                @display("p=102,214");
        }

    connections:
        upperLayerIn --> shaper.in;
        upperLayerDirectIn --> shaper.inD;
        upperLayerOut <-- inControl.out;
        shaper.out --> mac.upperLayerIn;
        inControl.in <-- filtering.out;
        filtering.in <-- mac.upperLayerOut;
        phys <--> mac.phys;
}
