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

#ifndef __CAN4CQF_CIRCULARROTATIONBUFFER_H_
#define __CAN4CQF_CIRCULARROTATIONBUFFER_H_

#include <omnetpp.h>
#include <string>
#include <map>
#include<vector>
#include <algorithm>
#include <utility>
//Inet auto-generated messages
#include "inet/linklayer/ethernet/EtherFrame_m.h"
//FiCo4OMNeT auto-generated messages
#include "fico4omnet/linklayer/can/messages/CanDataFrame_m.h"
//CAN4CQF
#include "can4cqf/linklayer/message/PoolMessage_m.h"
#include "can4cqf/linklayer/message/CircularRotationMessage.h"
#include "can4cqf/linklayer/message/IEEE8021QchTTFlow_m.h"
#include "can4cqf/linklayer/message/IEEE8021QchBEFlow_m.h"
using namespace omnetpp;
using namespace FiCo4OMNeT;
using namespace std;
/*
 * @brief:核心思想->每次到达T_CQF时，计算缓冲区中所有报文的剩余Deadline，将remaining deadline<T_CQF的报文封装至TSN frame中，
 * 转发至下游模块，其余报文等待下次T_CQF重新计算
 * @parameter:
 *              T_CQF：队列切换周期，根据ILP求解结果得到
 *              maxNuminCQF：最大TSN frame 封装数量，与T_CQF正相关
 *              lambda:Deadline系数，D_i= lambda * T_i，原则上应该根据优先级升序得到
 * */

namespace CAN4CQF {

/*
 * @brief: The core idea is to, at each T_CQF, calculate the remaining Deadline of all messages in the buffer.
 * Messages with remaining Deadline < T_CQF are encapsulated into TSN frames and forwarded to downstream modules.
 * Other messages wait for the next T_CQF to be recalculated.
 */
class CircularRotationBuffer : public cSimpleModule
{
public:
    /*
      @param:
        canfdmsg canfd的指针句柄
        time_arriving canfd首次到达GW时间
        remaining_Deadline canfd在每一次循环中重新计算后的剩余deadline
        zeta canfd在等待过程中经历的循环次数/zeta（与论文保持一致）
        wcrt canfd在r_e2g和r_g2e中经历的wcrt
        C_i  canfd发送时延
        D_i canfd的deadline
        P_i canfd的优先级
    */
      class canfdInfo{
      public:
        CanDataFrame* canfdmsg;
        simtime_t time_arriving;
        double remaining_Deadline;
        int zeta;
        double wcrt;
        double C_i;
        double D_i;
        int P_i;
      };
  protected:
    // 初始化函数
    virtual void initialize();
    // 消息处理核心函数
    virtual void handleMessage(cMessage *msg);
    /*
     * @brief 记录中间产生的日志
     * */
    void logToJsonFile();
    /*
     * @brief 用于结束写入json文件
     * */
    void checkAndAppendBracket(const std::string& filename);
    /*
     * @brief 结束标识
     * */
    bool finialLogFlag=false;
    // 重要参数

    /*
     * @brief 队列切换周期极值
     * */
    double timeout;
    /*
     * @brief T_CQF循环周期次数
     * */
    double T_CQF_loopnum = 0;
    /*
     * @brief 参数修改函数，并为报文命名
     * */
    virtual void handleParameterChange(const char *parname) override;
    /**
     * @brief Gateway ID in gateway config file. If auto or empty the gateway module name will be used.
     */
    std::string gatewayID;
    /*
        @biref 带宽预留参数/lamda
    */
    double lamda;
    /*
        @brief 仲裁字段传输速率
    */
    double tau_arb;
    /*
        @brief 数据字段传输速率
    */
    double tau_tarn;
    /*
     * @brief 存储计算wcrt必备的报文数据，period和c_i，且这组数据不会清除
     * */
    struct wcrt_data_info{
        double period;
        double c_i;
    }; // 均换算为ms
    /*
     * @brief 使用map存储wcrt计算所需数据
     * */
    map<int, wcrt_data_info> wcrt_data_map;
    /*
        @brief 报文类型
    */
    std::string type;
    /*
     * @brief 检查是否到达调度周期s
     * */
    bool ChecktimeinTCQF();
    /*
     * @breidf 判断是否可调度，可调度:true，不可调度:false
     * */
    bool checkSchedulable(CanDataFrame* newMsg);
    /*
     * @brief 存储到达的CAN-FD message
     * */
    void getCanfdArrivingInfo(CanDataFrame* msg);
    /*
     * @brief：存储CAN-FD信息的数据结构
     * */
    vector<canfdInfo> canfdinfo_vector;
    /*
      * @brief：选择即将到达Deadline的报文封装至TSN frame中，转发至下游模块
    */
    bool EncapsulateTSNFrame();
    /*
        @brief: 计算缓冲区队列中所有报文的剩余等待时间
    */
    void caluculateEndToEndDelay();
    /*
        @brief 计算canfd在源CAN-FD的排队时延
    */
    double calculateWCRT(double c_i, int p_i, double d_i, double b_i);
    /*
        @breif 存储待发送的报文和需要驻留的时间
    */
    map<CanDataFrame*, double> forwarding_send_msg;
    public:
    /*
     * @brief:构造函数
     * */
    CircularRotationBuffer();
    /*
     * @brief：析构函数
     * */
    ~CircularRotationBuffer();
    private:
      /*
       * @brief 存储循环调度的msg指针
       * */
      cMessage *continuousFunctionMsg;
      /*
       * @brief 结构体，存储CAN-FD的相关信息
       * */
      void finish();

};

} //namespace

#endif
