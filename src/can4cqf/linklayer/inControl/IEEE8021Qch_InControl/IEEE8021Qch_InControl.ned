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

package can4cqf.can4cqf.linklayer.inControl.IEEE8021Qch_InControl;

import core4inet.linklayer.inControl.base.IInControl;
import core4inet.utilities.classes.Timed;
//
// 处理来自MAC层的数据，将TT流发送到高优先级输出端口，BE流发送到低优先级输出端口
//
simple IEEE8021Qch_InControl extends Timed like IInControl
{
    parameters:
        @display("i=block/classifier");
        @class(IEEE8021Qch_InControl);

        //Signal is emitted when when frame is received. Contains the received frame.
        @signal[rxPk](type=inet::EtherFrame; unit=packets);
        //End-to-end latency recorded when frame is received. Contains the time from frame creation until received in the Buffer
        @statistic[rxLatency](title="End-to-end latency (received)"; source="messageAge(rxPk)"; unit=s; record=stats,histogram?,vector; interpolationmode=none);
        //Maximum packet size in Interval
        @statistic[maxPacketSize](title="Maximum Packet Size in Interval"; source=rxPk; unit=B; record="IntervalMaxVector(packetBytes)?"; interpolationmode=linear);
        //Minimum packet size in Interval
        @statistic[minPacketSize](title="Minimum Packet Size in Interval"; source=rxPk; unit=B; record="IntervalMinVector(packetBytes)?"; interpolationmode=linear);
        //Utilized Bandwidth over Interval
        @statistic[bandwidth](title="Bandwidth used over Interval"; source=rxPk; unit=B; record="IntervalSumVector(packetBytes)?"; interpolationmode=linear);
        //Floating utilized Bandwidth over Interval
        @statistic[bandwidthFloating](title="Floating Bandwidth for each Interval"; source=rxPk; unit=B; record="floatingIntervalSumVector(packetBytes)?"; interpolationmode=sample-hold);

        // if true, all packets are received, otherwise only the ones with matching destination MAC address (This does not affect CT-Frames)
        bool promiscuous = default(false);
        //Untagged VLAN. Untagged incoming frames get tagged with this VLAN.
        int untaggedVID = default(0);
        //Interface allows frames with this VLAN to enter
        string taggedVIDs = default("0");

    gates:
        //Input from the lower layer (MAC layer)
        input in @labels(EtherFrame);
        //Output for best-effort frames to the upper layer (EtherLLC or IMacRelayUnit)
        output out @labels(EtherFrame);    
}
