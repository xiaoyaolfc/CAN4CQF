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

#ifndef __CAN4CQF_AGGREGATIONGATEWAYBUFFERING_H_
#define __CAN4CQF_AGGREGATIONGATEWAYBUFFERING_H_

#include <omnetpp.h>
//Std
#include <string>
#include <map>
#include <algorithm>

//SignalsAndGateways
#include "signalsandgateways/base/SignalsAndGateways_Defs.h"

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
 * @brief 根据CAN业务流实现TSN帧封装功能，对于上行链路:CAN->TSN，根据业务流的不同优先级对到达的CAN帧进行HRTF和ARTF两种类型的封装
 * 对于下行链路：TSN->CAN，需要将两种类型的报文拆分成若干CAN报文，并转发到CAN Bus中
 */
class AggregationGatewayBuffering : public cSimpleModule
{
protected:
  virtual void initialize();
  /**
   * @brief 根据约束条件实现帧封装功能。
   */
  virtual void handleMessage(cMessage *msg);
  // @brief:定义参数变化处理函数
  virtual void handleParameterChange(const char *parname) override;
  /*
   * @brief 记录中间产生的日志
   * */
  void logToJsonFile();
  /*
   * @brief 用于结束写入json文件
   * */
  void checkAndAppendBracket(const std::string& filename);
  /*
   * @brief 定义CAN业务流划分函数
   * */
  bool divisionTrafficFlow(cMessage* msg);
  /*
   * @breif 对ARTF流进行封装处理
   * */
  void AdjustEncapsulationCanFrame(cMessage* msg);
  /*
   * @brief 计算当前CanDateFrame的deadline
   * */
  bool CalculateReservationDeadline(std::list<FiCo4OMNeT::CanDataFrame*>);
  /*
   * @brief 存储到达报文的simtime_t
   * */
  void GetArrivalTime(FiCo4OMNeT::CanDataFrame*);
  /*
   * @brief 时延约束阈值
   * */
  double alpha;
  /*
   * @brief 封装数量约束
   * */
  int gamma;
  /*
   * @brief 表示所有CAN报文中最大的周期
   * */
  double Tmax;
  /*
   * @brief 表示CAN业务流优先级划分阈值
   * */
  double sigma;
  /*
   * @brief 定义HRTF判断阈值
   * */
  unsigned int MaxHRTFID;
public:
  // 构造函数
  AggregationGatewayBuffering();
  // 解析函数
  ~AggregationGatewayBuffering();
private:
  /*
   * @brief 使用map函数映射存储<CanID>和Cmsg报文
   * */
  std::map<int, cMessagePointerList*> poolMap;
  /*
   * */
  /**
   * @brief 对于给定的CanID允许的最大等待时间
   */
  std::map<unsigned int,simtime_t> holdUpTimes;

  /**
   * @brief 每个PoolMessage持续event的时间。This map holds the hold up time events for every pool.
   */
  std::map<cMessagePointerList*,cMessage*> scheduledHoldUpTimes;

  /**
   * @brief This maps holds the scheduled times for all pools of the gateway.
   */
  std::map<cMessage*,simtime_t> scheduledTimes;

  /**
   * @brief This map holds the arrival times of the messages which are currently buffered.
   */
  std::map<cMessagePointerList*,std::list<simtime_t>> poolArrivalTimes;

  /**
   * @brief Simsignal for the pool size.
   */
  std::vector<simsignal_t> poolSizeSignals;

  /**
   * @brief Signals that are emitted when a pool is forwarded to the next module.
   */
  std::vector<simsignal_t> poolHoldUpTimeSignals;

  /**
   * @brief Gateway ID in gateway config file. If auto or empty the gateway module name will be used.
   */
  std::string gatewayID;
  // 从XML读取配置
  void readConfigXML();
  /**
   * @brief Returns a list with all messages which are currently in the pool based on a canID.
   * 基于canID返回一个包含当前池中所有消息的列表。
   */
  cMessagePointerList* getPoolList(unsigned int canID);

  /**
   * @brief Returns a list with all messages which are currently in the pool based on a holdUpTimeEvnet.
   * 基于holdUpTimeEvnet返回一个包含当前池中所有消息的列表。
   */
  cMessagePointerList* getPoolList(cMessage* holdUpTimeEvent);

  /**
   * @brief 根据给定的CANID返回对应的hold up time.
   */
  simtime_t getIDHoldUpTime(unsigned int canID);

  /**
   * @brief Returns the event which can be used to get the remaining hold up time for the given pool.
   */
  cMessage* getPoolHoldUpTimeEvent(cMessagePointerList* poolList);

  /**
   * @brief Returns the remaining hold up time for a specific pool to which the event is assigned.
   */
  simtime_t getCurrentPoolHoldUpTime(cMessage* poolHoldUpTimeEvent);

  /**
   * @brief Emits the arrival times of the messages which are forwarded to the transformation module.
   */
  void emitSignals(cMessagePointerList* poolList);

  /**
   * @brief Returns the id of a pool.
   */
  unsigned int getPoolID(cMessagePointerList* poolList);
  /*
   * @brief 使用一个list存储需要转发的CAN报文
   * */
  std::list<FiCo4OMNeT::CanDataFrame*> ARTFList;
  /*
   * @brief 存储CAN报文的到达时间
   * */
  std::map<FiCo4OMNeT::CanDataFrame*,simtime_t> CanArrivalTime;
  /*
   * @brief 用来存储每个CanDataFrame的到达时间和当前仿真时间的差值，即当前的deadline时间
   * cMessageList* 存储CAN报文，simtime_t存储该CAN报文到达时间
   * */
//  std::map<std::list<FiCo4OMNeT::CanDataFrame*>, simtime_t> ReserveDeadline;
  std::map<FiCo4OMNeT::CanDataFrame*, simtime_t> ReserveDeadline;
  /*
   * @brief 将接收到的HRTF报文解封装为CanDataFrame，并转发
   * */
  void HRTFDecapsulate(HRTF* hrtf);
  /*
   * @brief 将接受的ARTF报文解封装，遍历所有报文，按照顺序解封装发送到can节点
   * */
  void ARTFDecapsulate(ARTF* artf);
  /*
   * @brief 根据ID和period进行重新排序
   * */
  std::list<FiCo4OMNeT::CanDataFrame*> SortSequence(std::list<FiCo4OMNeT::CanDataFrame*> ARTFList);
  /*
   * @brief 存储（等待时间-到达时间)/Can->period
   * */
  std::map<FiCo4OMNeT::CanDataFrame*,double> NormalizedAwaitingTime;
};

} //namespace

#endif
