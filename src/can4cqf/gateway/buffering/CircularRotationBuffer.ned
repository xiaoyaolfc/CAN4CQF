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

//
// TODO auto-generated module
//
simple CircularRotationBuffer like IGatewayBuffering
{
    parameters:    	
        @display("i=block/buffer");
    	@signal[pool*MessageSize](type=unsigned long);
    	@signal[pool*HoldUpTime](type=simtime_t);
    	
    	//Statistic about the poolMessage size which are forwarded to the next module
    	@statisticTemplate[poolMessageSize](title="pool size ($name)"; record=count,sum,mean,min,max, "vector(last)"; interpolationmode=linear); 	
    	//Statistic about the time each frame within a pool was hold back.
    	@statisticTemplate[poolHoldUpTime](title="Pool-Hold-Up-Time ($name)"; record=mean,max,min,vector; interpolationmode=sample-hold);
    	@class("CircularRotationBuffer");
    	//Path of the gateway configuration xml file
    	xml configXML;
        
        //Gateway ID in gateway config file. If auto or empty the gateway module name will be used.
        string gatewayID = default("auto");
		// 一个T_CQF内可容纳的报文数量
		// int maxNuminCQF = default(10);     
		// 队列切换时间极值
		double timeout @unit(s)= default(0.0002s);  
		// Deadline与周期系数
		double lamda = default(0.6);
		// CAN-FD仲裁传输速率，0.5Mbps
		double tau_arb = default(2e-6);
		// CAN-FD数据传输速率,8Mbps
		double tau_tarn = default(1.25e-7);
	gates:
	    input in;
	    output out;  
}
