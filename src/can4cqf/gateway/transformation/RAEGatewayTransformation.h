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

#ifndef __CAN4CQF_RAEGATEWAYTRANSFORMATION_H_
#define __CAN4CQF_RAEGATEWAYTRANSFORMATION_H_

#include <omnetpp.h>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include <iterator>
//Auto-generated messages
#include "core4inet/linklayer/ethernet/base/EtherFrameWithQTag_m.h"
//FiCo4OMNeT auto-generated messages
#include "fico4omnet/linklayer/can/messages/CanDataFrame_m.h"
// 导入CAN4CQF生成的Message
#include "can4cqf/linklayer/message/ARTF.h"
#include "can4cqf/linklayer/message/HRTF_m.h"
#include "can4cqf/linklayer/message/IEEE8021QchTTFlow_m.h"
#include "can4cqf/linklayer/message/IEEE8021QchBEFlow_m.h"
//CoRE4INET
#include "core4inet/base/CoRE4INET_Defs.h"
// CAN4INET
#include "can4cqf/base/CAN4CQF_Defs.h"

using namespace omnetpp;
using namespace std;

namespace CAN4CQF {

/**
 * @brief   上行链路：CAN->TSN，目标将多个CAN报文封装为一个TSN
 *          下行链路：TSN->CAN，将TSN报文解封装为多个CAN报文，并逐个转发到目的CAN中
 * buffering发送的ARTF包含CAN时间戳，使用EDF:sort(fi.period - re2g)
 */
class RAEGatewayTransformation : public cSimpleModule
{
  public:
    RAEGatewayTransformation();
  protected:
    virtual void initialize();
    /*
     * @brief 根据上下行链路不同，对到达报文进行处理
     * */
    virtual void handleMessage(cMessage *msg);
    /*
     * @brief 处理参数变化
     * */
    void handleParameterChange(const char* parname);
  private:
    /*
     * @brief 一个队列切换周期内可存储的最大值
     * */
    int maxNuminCQF;
    /*
     * @breif 队列切换周期极值
     * */
    double timeout;
    /*
     * @brief 用户指定确定性保证流ID
     * */
    string Deterministic_flow;
    /*
     * @brief 定义信号->每个time-slot发送的CAN报文数量
     * */
    simsignal_t EncapCanFrameCntSignal;
    /*
     * @brief 定义信号->网络带宽利用率
     * */
    simsignal_t RAE_BandwidthSignal;
    /*
     * @brief 保存该节点需要处理的所有CAN报文
     */
    std::list<unsigned int> canToCan;

    /**
     * @brief Holds the information to which BE ethernet destination CAN frames with the corresponding ID should be forwarded.
     */
    std::map<unsigned int, std::list<std::string> > canToBEEthernet;

    /**
     * @brief Holds the information from which destination be ethernet frames should be forwarded to CAN.
     */
    std::list<std::string> beEthernetToCan;

    /**
     * @brief Holds the information to which IEEE802.1Q ethernet destination CAN frames with the corresponding ID should be forwarded.
     */
    class QInfo{
        public:
            std::string mac;
            uint16_t vid;
            uint8_t pcp;
    };
    std::map<unsigned int, std::list<QInfo> > canToQEthernet;

    /**
     * @brief Holds the information from which destination IEEE802.1Q ethernet frames should be forwarded to CAN.
     */
    std::list<std::string> qEthernetToCan;
    /**
     * @brief CAN帧的CRC字段bit长度.
     */
    static const int CANCRCBITLENGTH;
    std::string gatewayID;
    /**
     * @brief 从XML文件中读取网关配置信息
     */
    void readConfigXML();

    /*
     * @brief 将buffer发送的hrtf封装为TT流发送
     * */
    void transformhrtfToTTFlow(HRTF* hrtf);
    /*
     * @brief 将buffer发送的artf封装为TT流发送
     * */
    void transformartfToTTFlow(ARTF* artf);
    /*
     * @brief 用来创建一个表示当前gateway信息的函数
     * */
    std::string createMessageName(const char* additionalInformation);
    /*
     * @brief 对于下行链路的解封装:TSN->CAN，由于我们在buffer封装的过程中已经进一步细分ARTF的类型，所以在transformation模块中，我们只需要解封并
     * 按照解封后的顺序依次转发到对应的buffer模块中即可
     * */
    void transformTTFlowToCanDataFrame(IEEE8021QchTTFlow* ttflow);
    /*
     * @brief 专门用于ARTF解封装为std::list<FiCo4OMNeT::CanDataFrame*>
     * */
//    std::list<FiCo4OMNeT::CanDataFrame*> ARTFDecapsulateToARTFList(ARTF* artf);
    /*
     * @brief 存储到达顺序 CanDataFrame和时间戳double arr_time
     * */
    std::map<FiCo4OMNeT::CanDataFrame*, simtime_t> order_GW;
    /*
     * @brief 从ARTF中解封装到达顺序 CanDataFrame和时间戳double arr_time
     * */
    std::map<FiCo4OMNeT::CanDataFrame*, simtime_t> Decap_orderfromeARTF(ARTF* artf);
    /*
     * @brief 从ARTF中解封装得到的CAN报文队列
     * */
    std::list<FiCo4OMNeT::CanDataFrame*> ARTFList;
  public:
    /*
     * @brief 存储CAN报文在到达顺序中的相关信息：
     * dataframe: CAN报文指针
     * arr_time: CAN到达gateway时间戳
     * interval_time: 相邻CAN报文到达时间间隔
     * issucc: 是否为连续到达顺序
     * index: 最终发送顺序
     * */
    class order_info{
        public:
            FiCo4OMNeT::CanDataFrame* dataframe;
            simtime_t arr_time;
            double interval_time;
            double queue_time;
            bool issucc;
            int index;
            double get_queue_time() const {return queue_time;}
    };
    /*
     * @brief 调整
     * */
    std::vector<order_info> order_list;
    /*
     * @breif 处理重排函数
     * */
    ARTF* Resort_order(std::map<FiCo4OMNeT::CanDataFrame*, simtime_t>, double T_CQF, int encap_num);
};

} //namespace

#endif
