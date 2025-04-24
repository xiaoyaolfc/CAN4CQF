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

package can4cqf.can4cqf.linklayer.shaper.IEEE8021Qcr;

//
// ATSTokenBucket: 通过令牌桶算法控制数据流速率
//
simple ATSTokenBucket {
    parameters:
        double bucketSize @unit(Mbit) = default(1Mbit);  // 1Mb = 1e6 bit
        double tokenRate @unit(Mbps) = default(100Mbps); 
        double maxBurstSize @unit(Kbit) = default(64Kbit); // 改为 64Kb = 64e3 bit
        @display("i=block/control");
    gates:
        input in @labels(Packet);  // 输入数据包
        output out @labels(Packet); // 输出数据包
}
