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


import can4cqf.can4cqf.linklayer.inControl.AVB_8021Q_InControl;
import can4cqf.can4cqf.linklayer.shaper.IEEE8021Qcr.ATSShaper;
import inet.linklayer.ethernet.EtherMACFullDuplex;
//
// IEEE8021QcrPHYPort: 实现物理层+数据链路层端口，整合 ATS 整形、入控制和 MAC 功能
//
module IEEE8021QcrPHYPort {
    parameters:
        @display("bgb=188,180;i=device/port");
        bool promiscuous = default(false);
        double static_tx_delay = default(0);
        string interfaceTableModule;
        int untaggedVID = default(0);
        string taggedVIDs = default("0");
        
        *.interfaceTableModule = default(absPath(this.interfaceTableModule));
        
    gates:
        // 上层接口
        input upperLayerIn @loose @labels(EtherFrame);
        input upperLayerDirectIn @directIn @labels(EtherFrame);
        output upperLayerOut @loose @labels(EtherFrame);
        // 物理层双向接口
        inout phys @labels(EtherFrame);

    submodules:
        // 出站 ATS 整形模块
        shaper: ATSShaper {
            @display("p=48,45");
            untaggedVID = parent.untaggedVID;
            taggedVIDs = parent.taggedVIDs;
        }
        // 入站控制模块
        inControl: AVB_8021Q_InControl {
            parameters:
                @display("p=142,45");
                promiscuous = parent.promiscuous;
                untaggedVID = parent.untaggedVID;
                taggedVIDs = parent.taggedVIDs;
        }
        // MAC 模块
        mac: EtherMACFullDuplex {
            parameters:
                promiscuous = true; 
                // 指定 MAC 内部的队列模块为 ATSShaper 内部的 ATSQueue
                queueModule = "^.shaper.queue";
            @display("p=87,139");
        }
    connections:
        // 将上层的两个输入分别连接到 ATSShaper 的 in[0] 和 in[1]
        upperLayerIn --> shaper.in[0];
        upperLayerDirectIn --> shaper.in[1];
        // ATSShaper 的输出连接到 MAC 的上层输入
        shaper.out --> mac.upperLayerIn;
        // MAC 上层输出连接到入站控制模块
        mac.upperLayerOut --> inControl.in;
        // 入站控制模块输出送往上层
        inControl.out --> upperLayerOut;
        // 物理接口直接连接到 MAC 的物理接口
        phys <--> mac.phys;
}
