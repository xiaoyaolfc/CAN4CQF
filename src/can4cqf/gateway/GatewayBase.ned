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

package can4cqf.can4cqf.gateway;

import can4cqf.can4cqf.gateway.transformation.IGatewayTransformation;
import can4cqf.can4cqf.gateway.buffering.IGatewayBuffering;
import can4cqf.can4cqf.gateway.routing.IGatewayRouter;
// 实现了基础的Gateway，能够根据Config进行配置
// 将所有模块均换为修改过后的，其中主体实现部分实在buffering中
module GatewayBase
{
    parameters:
        @display("i=device/hub");

        //Number of gateway ports
        int numPorts;
        //Path of the gateway configuration xml file
        xml configXML;
        //The path in configXML is the default path for the router, buffering and transform module
        xml defaultConfigXML = configXML;
        //Static delay of the router module
        double routingDelay @unit(s) = default(0s);
        //Static delay of the buffering module
        double bufferingDelay @unit(s) = default(0s);
        //Static delay of the tranform module
        double transformDelay @unit(s) = default(0s);     
        //Gateway ID in gateway config file. If auto or empty the gateway module name will be used.
        string gatewayID = default("auto");
    gates:
        input lowerLayerIn[numPorts];
        output lowerLayerOut[numPorts];

    types:
        // Channel represents the processing delay of the router, buffering and tranform module
        channel ProcessingDelay extends ned.DelayChannel
        {
        }

    submodules:
        //module that handles the routing data
        router: <default("BaseGatewayRouter")> like IGatewayRouter {
            @display("p=63,100;i=block/routing");
            configXML = default(parent.defaultConfigXML);
            gatewayID = parent.gatewayID;
            gates:
                in[parent.numPorts];
                out[parent.numPorts];
        }

        //module that performs the transformations in all directions
//        transform[numPorts]: <default("GatewayTransformation")> like IGatewayTransformation {
		transform[numPorts]: <default("NoGatewayTransformation")> like IGatewayTransformation {
            @display("p=355,100,c,130;i=block/segm");
            configXML = default(parent.defaultConfigXML);
            gatewayID = parent.gatewayID;
        }

        //module that coordinates the preBuffer behaivior
//        buffering[numPorts]: <default("AggregationGatewayBuffering")> like IGatewayBuffering {
		buffering[numPorts]: <default("NoGatewayBuffering")> like IGatewayBuffering {            
            @display("p=200,100,c,130;i=block/subqueue");
            configXML = default(parent.defaultConfigXML);
            gatewayID = parent.gatewayID;
        }

    connections:
        for i=0..numPorts-1 {
            lowerLayerIn[i] --> router.in[i];
            router.out[i] --> ProcessingDelay {  delay = parent.routingDelay; } --> buffering[i].in;
            buffering[i].out --> ProcessingDelay {  delay = parent.bufferingDelay; } --> transform[i].in;
            transform[i].out --> ProcessingDelay {  delay = parent.transformDelay; } --> lowerLayerOut[i];
        }
}
