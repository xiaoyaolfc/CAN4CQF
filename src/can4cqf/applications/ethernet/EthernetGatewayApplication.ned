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

import can4cqf.can4cqf.applications.ethernet.IEthernetGatewayApplication;
import can4cqf.can4cqf.applications.IEEE8021Qch.TTApplicationBase;
//
// TODO auto-generated module
//
simple EthernetGatewayApplication extends TTApplicationBase like IEthernetGatewayApplication
{
    parameters:
    	@class(EthernetGatewayApplication);
    	@display("i=block/app");
    gates:
        //Connection to the gateway    
        output upperLayerOut @labels(EtherFrame);
        input upperLayerIn @labels(EtherFrame); 
        //Input gate for the incoming best-effort messages
        input in @directIn @labels(EtherFrame);               
}
