#ifndef __CAN4CQF_VARGATEWAYTRANSFORMATION_H_
#define __CAN4CQF_VARGATEWAYTRANSFORMATION_H_

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
#include "can4cqf/linklayer/message/CircularRotationMessage.h"
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
 * @brief 将 CircularRotationBuffer 传输的报文封装，转发即可，无需其他操作
 */
class VarGatewayTransformation : public cSimpleModule
{
public:
    VarGatewayTransformation();
    ~VarGatewayTransformation();
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    /*
     * @brief 处理参数变化
     * */
    void handleParameterChange(const char* parname);
    /*
        @brief 存储封装报文的 map
    */
    std::map<FiCo4OMNeT::CanDataFrame*,double> crcToMap;    
  private:
    /**
     * @brief CAN帧的CRC字段bit长度.
     */
    static const int CANCRCBITLENGTH;
    /*
     * @brief 定义的 timeout
     * */
    double timeout;
    /*
     * @brief 定义信号 -> 每个 time-slot 发送的 CAN 报文数量
     * */
    simsignal_t EncapCanFrameCntSignal;
    /*
     * @brief 定义信号 -> 网络带宽利用率
     * */
    simsignal_t Circular_BandwidthSignal;
    /*
     * @brief 封装 CAN -> CAN 的报文
     * */
    std::list<unsigned int> canToCan;
    /*
        @brief 存储需要驻留传输的报文
    */
    std::map<FiCo4OMNeT::CanDataFrame*,double> canfdToMap;
    /*
        @brief 存储需要驻留传输的报文
    */
    std::list<FiCo4OMNeT::CanDataFrame*> canfdNeedResendList;

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
     * @brief 定义网关 ID
     * */
    std::string gatewayID;
    /**
     * @brief 从 XML 文件中读取网关配置信息
     */
    void readConfigXML();
    /*
     * @brief 用来创建一个表示当前 gateway 信息的函数
     * */
    std::string createMessageName(const char* additionalInformation);

    /*
      @brief 将报文从 buffer 模块来的数据包 crclmsg 重新封装为 TSN frame
    */
    IEEE8021QchTTFlow* transformcrclmsgToTTFlow(CircularRotationMessage* crclmsg);
    /*
      @brief 将报文从 TSN frame 中解封装为若干 CAN-FD message，并主动驻留若干时间才发送
    */
    void transformTTFlowToCan(IEEE8021QchTTFlow* ttflow);

};

} //namespace

#endif
