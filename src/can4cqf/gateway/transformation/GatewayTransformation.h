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

#ifndef __CAN4CQF_GATEWAYTRANSFORMATION_H_
#define __CAN4CQF_GATEWAYTRANSFORMATION_H_

#include <omnetpp.h>
//Std
#include <string>
#include <map>
#include <list>
#include <vector>
// 导入CAN4CQF生成的Message
#include "can4cqf/linklayer/message/ARTF.h"
#include "can4cqf/linklayer/message/HRTF_m.h"
#include "can4cqf/linklayer/message/IEEE8021QchTTFlow_m.h"
#include "can4cqf/linklayer/message/IEEE8021QchBEFlow_m.h"
//CoRE4INET
#include "core4inet/base/CoRE4INET_Defs.h"


namespace CAN4CQF {
using namespace omnetpp;

/**
 * @brief transformation模块从buffer和switch接收ARTF,HRTF,Ethernet报文，对于上行链路：CAN->TSN，将ARTF，HRTF打包成TT流和BE流
 * 对于下行链路：TSN->CAN，将TT流和BE流解封装为HRTF和ARTF，对于HRTF，进一步解封为CAN帧转发到buffer中，对于ARTF，可能需要应该用MAT方法进行转发
 */
class GatewayTransformation : public cSimpleModule
{
  protected:
    virtual void initialize();
    /*
     * @brief 上行链路：从buffer模块发送过来的ARTF，HRTF包封装为对应TT流;
     * 下行链路：要将从TSN到达的TT流解封为对应的ARTF，HRTF，转发到buffer中进行下一步转发
     * */
    virtual void handleMessage(cMessage *msg);
    /*
     * @brief 处理参数变化
     * */
    void handleParameterChange(const char* parname);
  private:
    /**
     * @brief 保存该节点需要处理的所有CAN报文
     */
    std::list<unsigned int> canToCan;
    /*
     * @brief 定义信号->每个time-slot发送的CAN报文数量
     * */
    simsignal_t EncapCanFrameCntSignal;

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
    /*
     * @brief 从ARTF中解封装得到的CAN报文队列
     * */
    std::list<FiCo4OMNeT::CanDataFrame*> ARTFList;
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
    std::list<FiCo4OMNeT::CanDataFrame*> ARTFDecapsulateToARTFList(ARTF* artf);

};

} //namespace

#endif
