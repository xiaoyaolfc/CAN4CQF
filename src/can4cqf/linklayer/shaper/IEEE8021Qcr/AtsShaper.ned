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

package can4cqf.can4cqf.linklayer.shaper.IEEE8021Qcr;
import can4cqf.can4cqf.linklayer.shaper.IEEE8021Qcr.ATSQueue;
import can4cqf.can4cqf.linklayer.shaper.IEEE8021Qcr.CreditCalculator;
import can4cqf.can4cqf.linklayer.shaper.IEEE8021Qcr.ATSTrafficRegulator;
import can4cqf.can4cqf.linklayer.shaper.IEEE8021Qcr.ATSTokenBucket;
import can4cqf.can4cqf.linklayer.shaper.IEEE8021Qcr.ATSFlowEstimator;

// ATSShaper: 基于 IEEE 802.1Qcr 的 ATS 整形复合模块
// 由 ATSQueue、ATSTrafficRegulator、ATSTokenBucket 和 ATSFlowEstimator 组成，
// 支持两个上层输入端口（普通和直通输入）的合并处理。
//
module ATSShaper {
    parameters:
        // 链路与令牌桶相关参数（单位均为纯数字）
        double linkRate @unit(bps) = default(1Gbps);         // 链路速率
        double bucketSize @unit(Mbit) = default(1Mbit);        // 令牌桶容量
        double tokenRate @unit(bps) = default(100Mbps);      // 令牌生成速率
        double maxBurstSize @unit(Kbit) = default(64Kbit);     // 最大突发流量
        double processingDelay @unit(us) = default(10us);     // 处理延迟
        double trafficRegulatorDelay @unit(ms) = default(1ms); // 流量调节延迟
        
        // 队列和流量调节参数
        int numQueues = default(8);
        int queueCapacity = default(100);
        // VLAN 参数
        int untaggedVID = default(0);
        string taggedVIDs = default("0");
        @display("i=block/classifier");

    gates:
        // 两个上层输入端口：普通输入和直通输入
        input in[2] @labels(Packet);
        output out @labels(Packet);

    submodules:
        queue: ATSQueue {
            parameters:
                numQueues = parent.numQueues;
                queueCapacity = parent.queueCapacity;
            @display("p=100,200");
        }
        trafficRegulator: ATSTrafficRegulator {
            parameters:
                maxRate = parent.tokenRate;
                shapingDelay = parent.trafficRegulatorDelay;
            @display("p=250,200");
        }
        tokenBucket: ATSTokenBucket {
            parameters:
                bucketSize = parent.bucketSize;
                tokenRate = parent.tokenRate;
                maxBurstSize = parent.maxBurstSize;
            @display("p=400,200");
        }
        flowEstimator: ATSFlowEstimator {
            parameters:
                linkRate = parent.linkRate;
                processingDelay = parent.processingDelay;
            @display("p=550,200");
        }

    connections:
        // 将 ATSShaper 的两个输入分别连接到 ATSQueue 的不同输入门
        in[0] --> queue.in[0];
        in[1] --> queue.in[1];
        // 数据包处理流程：队列 -> 流量调节 -> 令牌桶 -> 预计传输时间估计 -> 输出
        queue.out --> trafficRegulator.in;
        trafficRegulator.out --> tokenBucket.in;
        tokenBucket.out --> flowEstimator.in;
        flowEstimator.out --> out;
}