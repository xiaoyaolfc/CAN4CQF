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

#ifndef __CAN4CQF_TRANSFORMATION_H_
#define __CAN4CQF_TRANSFORMATION_H_

#include <omnetpp.h>
#include <map>
//Auto-generated messages
#include "core4inet/linklayer/ethernet/base/EtherFrameWithQTag_m.h"
//FiCo4OMNeT auto-generated messages
#include "fico4omnet/linklayer/can/messages/CanDataFrame_m.h"
// 导入CAN4CQF生成的Message
#include "can4cqf/linklayer/message/CircularRotationMessage.h"
#include "can4cqf/linklayer/message/IEEE8021QchTTFlow_m.h"


using namespace omnetpp;
using namespace std;

namespace CAN4CQF {

/**
 * TODO - Generated class
 */
class Transformation : public cSimpleModule
{
  protected:
   virtual void initialize() override;
   virtual void handleMessage(cMessage *msg) override;
   /*
     @brief 将报文从 TSN frame 中解封装为若干 CAN-FD message，并主动驻留若干时间才发送
   */
   void transformTTFlowToCan(IEEE8021QchTTFlow* ttflow);
   void handleExternalMessage(cMessage *msg);
   void handleCanFeedback(cMessage *msg);
   /*
       @brief 存储需要驻留传输的报文
   */
   std::map<FiCo4OMNeT::CanDataFrame*,double> canfdToMap;
   /*
       @brief 存储需要驻留传输的报文
   */
   std::list<FiCo4OMNeT::CanDataFrame*> canfdNeedResendList;
   /*
    * @brief 定义的 timeout
    * */
   double timeout;
   void handleSelfMessage(cMessage *msg);

};

} //namespace

#endif
