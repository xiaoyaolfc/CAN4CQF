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

package can4cqf.can4cqf.gateway.transformation;

//
// 实现根据buffering收到的CAN报文顺序，按照EDF方式调整发送顺序
//
simple RAEGatewayTransformation like IGatewayTransformation
{
    parameters:
        @class("RAEGatewayTransformation");
        @display("i=block/transport");
        // 定义信号用来收集每个周期封装报文的数量
        @signal[EncapCanFrameCnt](type=long);
        @signal[RAE_Bandwidth](type = double);
        // 记录统计数据        
        @statistic[EncapCanFrameCnt](title="Encapsulation-CAN"; source=EncapCanFrameCnt; record=vector; interpolationmode=linear);        
        @statistic[RAE_Bandwidth](title="Bandwidth"; source=RAE_Bandwidth; record=vector; interpolationmode=linear);
        
        //Path of the gateway configuration xml file
        xml configXML;
        
        //Gateway ID in gateway config file. If auto or empty the gateway module name will be used.
        string gatewayID = default("auto");
		// 一个T_CQF内可容纳的报文数量
		int maxNuminCQF = default(10);     
		// 队列切换时间极值
		double timeout = default(0.0002);
		// 用户指定需要确定性保证的流,Deterministic assurance mechanism，需要给定流的ID 
		string Deterministic_flow = default("1");    	    
    gates:
        input in;
        output out;    
}
