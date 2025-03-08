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

#ifndef __CAN4CQF_TIMEOUTGATEWAYTRANSFORMATION_H_
#define __CAN4CQF_TIMEOUTGATEWAYTRANSFORMATION_H_

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
 * TODO - Generated class
 */
class TimeoutGatewayTransformation : public cSimpleModule
{
public:
    TimeoutGatewayTransformation();
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    void handleParameterChange(const char* parname);
  private:
  private:
    /**
     * @brief CAN帧的CRC字段bit长度.
     */
    static const int CANCRCBITLENGTH;
    /*
     * @brief 定义的timeout
     * */
    double timeout;
    /*
     * @brief 定义信号->每个time-slot发送的CAN报文数量
     * */
    simsignal_t EncapCanFrameCntSignal;
    /*
     * @brief 定义信号->网络带宽利用率
     * */
    simsignal_t RAE_BandwidthSignal;
    /*
     * @brief 从ARTF中解封装得到的CAN报文队列
     * */
    std::list<FiCo4OMNeT::CanDataFrame*> ARTFList;
    /*
     * @brief 封装CAN->CAN的报文
     * */
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
    /*
     * @brief 定义网关ID
     * */
    std::string gatewayID;
    /**
     * @brief 从XML文件中读取网关配置信息
     */
    void readConfigXML();
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
};

} //namespace

#endif
