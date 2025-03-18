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

namespace CAN4CQF;

message CreditMessage extends cPacket
{
    // 定义基础数据类型
    double creditValue;  // 必须声明信用值字段
    int credits;
    simtime_t generationTime;
    bool congestionFlag; // 网络拥塞状态标记

    // 可选扩展字段（根据实际需求增减）
    string sourceQueue;
    double bandwidthRequirement;
}