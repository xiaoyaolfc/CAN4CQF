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

package can4cqf.can4cqf.applications.ethernet;
// 导入SignalAndGateway
import signalsandgateways.applications.ethernet.EthernetGatewayApplication;
// 导入CAN4CQF
import can4cqf.can4cqf.applications.ethernet.IEthernetGatewayHost;
import can4cqf.can4cqf.nodes.ethernet.IEEE8021QchHost;
// 使用在gateway中的ethernet模块，由于TSN模块需要与Switch中模块相同，故使用该模块
module TSNCQFEthernetGatewayHost extends IEEE8021QchHost like IEthernetGatewayHost
{
    parameters:
        @display("bgb=750,503");
    //        int numberOfTT_Vlinks; // (unused) Forced by interface
    //        int numberOfRC_Vlinks; // (unused) Forced by interface
    gates:
        output upperLayerOut;
        input upperLayerIn;
    submodules:
        gatewayApp: EthernetGatewayApplication {
            @display("p=530.322,49.686");
        }
    connections:
        gatewayApp.upperLayerOut --> upperLayerOut;
        gatewayApp.upperLayerIn <-- upperLayerIn;
}
