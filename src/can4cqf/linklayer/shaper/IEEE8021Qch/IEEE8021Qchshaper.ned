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

package can4cqf.can4cqf.linklayer.shaper.IEEE8021Qch;
import core4inet.linklayer.shaper.base.IShaper;
//import core4inet.linklayer.shaper.IEEE8021Qbv.queueing.IIEEE8021QbvQueueing;
import core4inet.buffer.base.IBuffer;
import core4inet.linklayer.shaper.IEEE8021Qbv.selectionAlgorithm.IIEEE8021QbvSelectionAlgorithm;
import core4inet.linklayer.shaper.IEEE8021Qbv.gate.IIEEE8021QbvGate;
import core4inet.linklayer.shaper.IEEE8021Qbv.selection.IIEEE8021QbvSelection;
import core4inet.linklayer.shaper.IEEE8021Qbv.gate.IIEEE8021QbvGateControlList;
// 导入CQF
//import can2cqfagg.can2cqfagg.cqf.ICQF;
import can4cqf.can4cqf.linklayer.shaper.IEEE8021Qch.IRxgate;
import can4cqf.can4cqf.linklayer.shaper.IEEE8021Qch_queueing.IIEEE8021QchQueueing;
// 与上一个的区别为CQF采用有GatewayControlList同时控制GCL和Rxgate
module IEEE8021Qchshaper like IShaper
{
    parameters:
        @display("i=block/classifier");
        //Untagged VLAN. Outgoing frames with this VLAN get untagged.
        int untaggedVID = default(0);
        //Interface allows frames with this VLAN to leave the port.
        string taggedVIDs = default("0");
        // Number of priorities.
        int numPCP = default(8);
        // Bandwidth of the TDMA traffic with higher priority than the CBS on this port (for compability with SRProtocol module)
        // 在此端口(为了与SRProtocol模块兼容)上具有比CBS更高优先级的TDMA业务的带宽
        int AVBHigherPriorityBandwidth @unit(bps) = default(0bps);
        // CQF门控数量
        int numrx = default(2);
    gates:
        // Input of the IEEE 802.1Qbv shaper.
        input in @labels(EtherFrame);
        // Input of the IEEE 802.1Qbv shaper for the direct in of the Port.
        input inD @labels(EtherFrame);
        // Output of the IEEE 802.1Qbv shaper.
        output out @labels(EtherFrame);
    submodules:
        queueing: <default("IEEE8021QchQueueing")> like IIEEE8021QchQueueing {
            parameters:
                @display("p=74,143");
                untaggedVID = untaggedVID;
                taggedVIDs = taggedVIDs;
                numPCP = numPCP;
        }
        queue[numPCP]: <default("IEEE8021QbvQueue")> like IBuffer {
            @display("p=292,143,c,130");
        }
        transmissionSelectionAlgorithm[numPCP]: <default("IEEE8021QbvSelectionAlgorithm")> like IIEEE8021QbvSelectionAlgorithm {
            @display("p=498,143,c,130");
        }

        transmissionGate[numPCP]: <default("IEEE8021QbvGate")> like IIEEE8021QbvGate {
            @display("p=790,143,c,130");
        }
        // 添加Rxgate，用IEEEQbvGate代码表示，通过GCL控制
        rxGate[numrx]: <default("IEEE8021QbvGate")> like IIEEE8021QbvGate {
            @display("p=175,637,c,130");
        }
        transmissionSelection: <default("IEEE8021QbvSelection")> like IIEEE8021QbvSelection {
            parameters:
                @display("p=1049,143");
                numPCP = numPCP;
        }
        gateControlList: <default("IEEE8021QbvGateControlList")> like IIEEE8021QbvGateControlList {
            parameters:
                @display("p=790,55");
                numGates = numPCP;
        }
    connections:
        in --> queueing.in[0];//使用整形器的帧从in门进入队列
        inD --> queueing.in[1];//不使用整形器的帧从inD门进入队列
        //# 感觉还得是0～5为BE流，6,7为TT流
        for i=0..numPCP-3 {
            queueing.out[i] --> queue[i].in;
        }
//        for j=6..7 {
//        	queueing.out[j] --> rxGate[j].in;
//        }
        transmissionSelection.out --> out;
}
