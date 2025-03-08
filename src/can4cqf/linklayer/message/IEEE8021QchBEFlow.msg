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

cplusplus {{
#include "inet/linklayer/ethernet/EtherFrame_m.h"
}}
import inet.linklayer.ethernet.EtherFrame;
namespace CAN4CQF;

//
// TODO generated message class
//
packet IEEE8021QchBEFlow extends inet::EthernetIIFrame {
    // 无数据载荷时的payload
    byteLength = 22;
    // IEEE802.1Q的网络类型提示
    etherType = 0x8100;
    // 表示报文优先级的，TT流仅使用前两个优先级
    uint8_t pcp;
    //或者结合PCP来指示在存在拥塞的情况下有资格丢弃的帧。 
    bool de;    
    // VLAN标识符
    uint16_t VID = 0;   
    // 设置message 颜色
    //#给几个可选项：rect,oval,triangle,hexagon,circle,roundedrect
    string displayString = "b=10,10,rect,black";  
}
