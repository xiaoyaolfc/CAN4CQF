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

// 20.8.7日更改HRTF，不再使用payload封装CAN报文，选择使用cpacket自带的encapsule实现，HRTF报文仅包含源CAN网络，目的CAN网络，封装报文数量
//cplusplus {{
//#include "fico4omnet/linklayer/can/messages/CanDataFrame_m.h"
//typedef FiCo4OMNeT::CanDataFrame* cMessagePointer;
//}}
//class noncobject cMessagePointer;
//
// 在buffer模块使用，功能为表示当前封装CAN报文的HRTF业务流类型
//
namespace CAN4CQF;
//packet HRTF {
//	cMessagePointer payload;	    
//}
packet HRTF{
	uint16_t encapnum;
	uint16_t source;
	uint16_t destination;
}