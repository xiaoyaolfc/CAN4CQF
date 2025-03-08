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

#ifndef __CAN4CQF_RXGATE_H_
#define __CAN4CQF_RXGATE_H_

#include <omnetpp.h>
// 倒入调度时钟包
#include "core4inet/utilities/classes/Scheduled.h"
#include "can4cqf/linklayer/message/IEEE8021QchTTFlow_m.h"
#include "can4cqf/linklayer/message/IEEE8021QchBEFlow_m.h"
using namespace omnetpp;
using namespace std;
// 使用自己的namespce
namespace CAN4CQF {

/**
 * @brief 为了实现cqf中Rx_gate的头文件,需要存储gcl并且根据不同的gcl选择将报文发送到不同的队列中
 */
class Rxgate : public virtual CoRE4INET::Scheduled
{
protected:
//    virtual void initialize();
//    virtual void handleMessage(cMessage *msg);
  private:
    /*
     *@brief 用来存储gcl的vector
     * */
    vector<string> initGatesStatus;
    /**
     * @brief 定义用来存储配置好的GCL的数据结构
     */
    vector<pair<vector<string>, double>> gateControlList;
    /**
     * @brief GCL数据结构的迭代器，存储当前GCL的指针，保存当前list元素
     */
    vector<pair<vector<string>, double>>::iterator gateControlElement;
    /*
     * @bried 定义Rxgate连接的队列数量
     * */
    unsigned int numGates;
    /**
     * @brief 定义
     */
    unsigned int configNo;
    /**
     * @brief 定义运行时间
     */
    string timerEventName;
  public:
    Rxgate();// 定义构造函数
    /**
     * 更新当前gcl为新的GCL。该函数调用处理参数变化来执行变化。
     * @param gcl the new gate control list to set.
     */
    virtual void setGateControlList (string gcl);

  protected:
    /**
     * @brief 初始化该模块
     *
     * @param stage :stage变量表示初始化的阶段，从stage=0开始
     */
    virtual void initialize(int stage);
    /**
     * @brief 返回该模块初始化阶段的stage数值
     *
     * @return returns 2
     */
    virtual int numInitStages() const;
    /**
     * @brief 表明一个参数发生了变化。
     *
     * @param parname 如果多个参数发生变化，则更改参数的名称或修改为nullptr。
     */
    virtual void handleParameterChange(const char* parname);
    /**
     * @brief 接收来自调度器的SchedulerTimerEvent消息，指示当前控制元件的到期时间。
     *
     * 切换到下一个控制元件，传播其门状态并调度下一个SchedulerTimerEvent消息。
     *
     * @param msg the incoming message
     */
    virtual void handleMessage(cMessage *msg);
    /**
     * @brief 传播当前控制的门的状态
     * Propagate the gate states of the current control element.
     */
//    virtual void propagateGateControlElement(vector<string> gateStates);
    /*
     * @brief 当接收到TT流时，将报文转发到队列中。
     * */
    void ForwardTTFlowToQueue(IEEE8021QchTTFlow* ttflow);

  private:
    /**
     * @brief 调度下一个 SchedulerTimerEvent message.
     */
    void scheduleCurrentGateControlElementTime(bool nextCycle);
    /**
     * @brief Switch to the first control element of the control list.
     */
    void switchToFirstGateControlElement();
    /**
     * @brief Switch to next control element of the control list.
     */
    void switchToNextGateControlElement();
};

} //namespace

#endif
