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
// 实现等待封装RAE中重排功能，将收到的CAN报文存储，计算其可能的源CAN排队时延，再根据EDF调整封装顺序，发送到transformation模块
//
simple ResortAggregationGatewayBuffering like IGatewayBuffering
{
    parameters:
        @display("i=block/buffer");
        @signal[pool*MessageSize](type=unsigned long);
        @signal[pool*HoldUpTime](type=simtime_t);

        //Statistic about the poolMessage size which are forwarded to the next module
        @statisticTemplate[poolMessageSize](title="pool size ($name)"; record=count,sum,mean,min,max,"vector(last)"; interpolationmode=linear);
        //Statistic about the time each frame within a pool was hold back.
        @statisticTemplate[poolHoldUpTime](title="Pool-Hold-Up-Time ($name)"; record=mean,max,min,vector; interpolationmode=sample-hold);
        @class("ResortAggregationGatewayBuffering");
        //Path of the gateway configuration xml file
        xml configXML;

        //Gateway ID in gateway config file. If auto or empty the gateway module name will be used.
        string gatewayID = default("auto");
        // 一个T_CQF内可容纳的报文数量
        int maxNuminCQF = default(10);
        // 队列切换时间极值，单位s
        double timeout @unit(s) = default(0.0004s);
        // 仲裁字段canfd 发送速率
        double tau_arb @unit(s) = default(0.0005s);
        // 数据字段canfd 传输速率
        double tau_tran @unit(s) = default(0.000125s);
    gates:
        input in;
        output out;
}
