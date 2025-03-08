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
//import core4inet.linklayer.inControl.AVB_8021Q_InControl;
import can4cqf.can4cqf.linklayer.inControl.AVB_8021Q_InControl;
import can4cqf.can4cqf.linklayer.shaper.IEEE8021Qch.IEEE8021Qch_shaper;
import inet.linklayer.ethernet.EtherMACFullDuplex;
// 实现了CQF的物理层+数据链路层的端口
module IEEE8021QchPHYPort
{
    parameters:
        @display("bgb=188,180;i=device/port");
        
        // if true, all packets are received, otherwise only the ones with matching destination MAC address
        // 如果为真，则接收所有数据包，否则只接收与目的MAC地址匹配的数据包
        bool promiscuous = default(false);
        //Static propagation delay for the port
        // 针对端口的静态传播时延
        double static_tx_delay @unit(s) = default(0ns);
        //Module holds Interfacenames
        // 模块持有接口名
        string interfaceTableModule;
        //Untagged VLAN. Untagged incoming frames get tagged with this VLAN. Outgoing frames with this VLAN get untagged
        //未标记VLAN,未标记的传入帧用这个VLAN标记。具有此VLAN的出站帧得到解标记
        int untaggedVID = default(0);
        //Interface allows frames with this VLAN to enter and leave the port
        //接口允许具有此VLAN的帧进入和离开端口
        string taggedVIDs = default("0");
        
        *.interfaceTableModule = default(absPath(interfaceTableModule));
        
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
            untaggedVID = untaggedVID;
            taggedVIDs = taggedVIDs;
        }
        
        //Module for the organisation of inbound traffic
        inControl: AVB_8021Q_InControl {
            parameters:
                @display("p=142,45");
                promiscuous = promiscuous;
                untaggedVID = untaggedVID;
            	taggedVIDs = taggedVIDs;
        }
        
        //MAC interface
        mac: EtherMACFullDuplex {
            parameters:
                promiscuous = true; 
                queueModule = "^.shaper.transmissionSelection";
                @display("p=87,139");
        }
        
    connections:
        upperLayerIn --> shaper.in;
        upperLayerDirectIn --> shaper.inD;
        upperLayerOut <-- inControl.out;
        shaper.out --> mac.upperLayerIn;
        inControl.in <-- mac.upperLayerOut;
        phys <--> mac.phys;        
}
