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

#ifndef __CAN4CQF_RESORTAGGREGATIONGATEWAYBUFFERING_H_
#define __CAN4CQF_RESORTAGGREGATIONGATEWAYBUFFERING_H_

#include <omnetpp.h>

using namespace omnetpp;
//Std
#include <string>
#include <map>
#include <algorithm>
#include <utility>
//Inet auto-generated messages
#include "inet/linklayer/ethernet/EtherFrame_m.h"
//FiCo4OMNeT auto-generated messages
#include "fico4omnet/linklayer/can/messages/CanDataFrame_m.h"
//CAN4CQF
#include "can4cqf/linklayer/message/PoolMessage_m.h"
#include "can4cqf/linklayer/message/HRTF_m.h"
#include "can4cqf/linklayer/message/ARTF.h"
#include "can4cqf/linklayer/message/IEEE8021QchTTFlow_m.h"
#include "can4cqf/linklayer/message/IEEE8021QchBEFlow_m.h"

namespace CAN4CQF {
using namespace omnetpp;
/**
 * @brief 采用EDF思想，应用order={f_i,f_j,...}存储一个T_CQF的切换周期，判断其中是否存在连续到达子序列，对于连续到达子序列order^SR={f_i,...}，统计其中报文长度
 * 对order^SR中任意流，将该流之前的报文长度和作为在CAN内的排队时延;对于非连续到达子序列，则将该流认为未经过排队。通过上述方法，得到其在CAN内的排队时延，再用流周期减去WCRT和计算的排队时延
 * f_i.period - f_i.WCRT - f_i.queue 最小的作为发送聚合报文的头，其余按升序排列
 */
class ResortAggregationGatewayBuffering : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    /*
     * @brief 一个队列切换周期内可存储的最大值
     * */
    int maxNuminCQF;
    /*
     * @breif 队列切换周期极值
     * */
    double timeout;
    /*
     * @brief 参数修改函数，并为报文命名
     * */
    virtual void handleParameterChange(const char *parname) override;
    /*
     * @brief 存储CAN报文的到达时间
     * */
    std::map<FiCo4OMNeT::CanDataFrame*,simtime_t> CanArGetArrivalTimerivalTime;
    /*
     * @brief 存储CAN报文的到达时间
     * */
    std::map<FiCo4OMNeT::CanDataFrame*,simtime_t> CanArrivalTime;

    /**
     * @brief Gateway ID in gateway config file. If auto or empty the gateway module name will be used.
     */
    std::string gatewayID;

    /*
     * @brief 存储到达报文的simtime_t
     * */
    void GetArrivalTime(FiCo4OMNeT::CanDataFrame*);
    /*
     * @brief 判断当前时间是否为一个T_CQF，当时间已满时将当前的排队顺序打包发送到transformation模块
     * */
    bool ChecktimeinTCQF();
    /*
     * @brief 将收集到的到达顺序封装进ARTF
     * */
    ARTF* Encap_orderinARTF(std::map<FiCo4OMNeT::CanDataFrame*,simtime_t> CanArrivalTime);
  public:
    /*
     * @brief 构造函数
     * */
    ResortAggregationGatewayBuffering();
    /*
     * @brief 析构函数
     * */
    ~ResortAggregationGatewayBuffering();
  private:
    /*
     * @brief 存储循环调度的msg指针
     * */
    cMessage *continuousFunctionMsg;
    /*
     * @brief 存储到达报文顺序，暂时感觉用不上
     * */
//    std::map<FiCo4OMNeT::CanDataFrame*, simtime_t> order_GW;
    /*
     * @brief T_CQF循环次数
     * */
    double T_CQF_loopnum = 0;
};

} //namespace

#endif
