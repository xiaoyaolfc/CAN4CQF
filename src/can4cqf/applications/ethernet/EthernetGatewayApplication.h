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

#ifndef __CAN4CQF_ETHERNETGATEWAYAPPLICATION_H_
#define __CAN4CQF_ETHERNETGATEWAYAPPLICATION_H_

#include <omnetpp.h>
#include "signalsandgateways/base/SignalsAndGateways_Defs.h"
// 导入TTApplication
#include "can4cqf/applications/IEEE8021Qch/TTApplicationBase.h"

using namespace omnetpp;
using namespace CoRE4INET;

namespace CAN4CQF {

/**
 * @brief 在TSN端的Gateway application，该模块连接了gateway和TSN
 */
class EthernetGatewayApplication : public virtual CAN4CQF::TTApplicationBase
{
  protected:
    virtual void initialize();
    /*
     * @breif 消息处理模块
     * 从TSN到达的message处理发送到gateway中
     * 从gateway到达的message处理发送到TSN中
     * */
    virtual void handleMessage(cMessage *msg);
  public:
    // 构造函数
    EthernetGatewayApplication();
    // 析构函数
    ~EthernetGatewayApplication();
  private:
      /**
       * Map映射每个CanID对应发送的门
       */
//      std::unordered_map<unsigned int, std::vector<cGate*>> _canIdSubMap;
};

} //namespace

#endif
