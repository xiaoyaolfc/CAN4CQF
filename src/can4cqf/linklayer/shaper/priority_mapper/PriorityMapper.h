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

#ifndef __CAN2CQF_PRIORITYMAPPER_H_
#define __CAN2CQF_PRIORITYMAPPER_H_

#include <omnetpp.h>
#include "inet/common/Compat.h"          // 包含SocketInd标签定义
#include "core4inet/linklayer/ethernet/AS6802/CTFrame_m.h"
#include "core4inet/linklayer/contract/ExtendedIeee802Ctrl_m.h"


using namespace omnetpp;
using namespace CoRE4INET;

namespace CAN4CQF {

/**
 * TODO - Generated class
 */
class PriorityMapper : public cSimpleModule {
  protected:
    virtual void handleMessage(cMessage *msg) override;

  public:
    // 函数功能：将DSCP值映射到TSN优先级（0-7）
    // 参数：dscp - IP头中的DSCP字段
    // 返回值：TSN优先级（整数）
    int mapDscpToPriority(int dscp);
};

} //namespace

#endif
