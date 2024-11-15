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

package can4cqf.can4cqf.gateway.buffering;

// 缓冲模块的接口
moduleinterface IGatewayBuffering
{
    parameters:
        // gatewayXML文件路径
        xml configXML;
        
        //在XML中定义的Gateway ID. If auto or empty the gateway module name will be used.
        string gatewayID;
        
    gates:
        input in;
        output out;      
}
