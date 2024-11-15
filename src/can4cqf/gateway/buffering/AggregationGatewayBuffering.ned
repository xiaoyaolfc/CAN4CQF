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
import can4cqf.can4cqf.gateway.buffering.IGatewayBuffering;
//
//# 定义自己的缓冲区代码，主要为了实现根据GCL状态和约束条件判断实现帧封装，所以在NED文件中应该增加识别GCL模块
//# 并在.cc文件中增加约束条件判断函数。对于到达的CAN帧，封装目标仅为TT Flow,BE Flow
simple AggregationGatewayBuffering like IGatewayBuffering
{
    parameters:    	
        @display("i=block/buffer");
    	@signal[pool*MessageSize](type=unsigned long);
    	@signal[pool*HoldUpTime](type=simtime_t);
    	
    	//Statistic about the poolMessage size which are forwarded to the next module
    	@statisticTemplate[poolMessageSize](title="pool size ($name)"; record=count,sum,mean,min,max, "vector(last)"; interpolationmode=linear); 	
    	//Statistic about the time each frame within a pool was hold back.
    	@statisticTemplate[poolHoldUpTime](title="Pool-Hold-Up-Time ($name)"; record=mean,max,min,vector; interpolationmode=sample-hold);
    	@class("AggregationGatewayBuffering");
    	//Path of the gateway configuration xml file
    	xml configXML;
        
        //Gateway ID in gateway config file. If auto or empty the gateway module name will be used.
        string gatewayID = default("auto");
		// 时延约束阈值
		double alpha = default(0.5);
		// 封装数量约束
		int gamma = default(25);
		// 业务流划分阈值
		double sigma = default(0.015);         
		// 所有CAN报文中最大的周期
		double Tmax = default(1s) @unit(s);     
		// 定义HRTF优先级判断阈值
		int MaxHRTFID = default(5);     
    	
    gates:
        input in;
        output out;    
}
