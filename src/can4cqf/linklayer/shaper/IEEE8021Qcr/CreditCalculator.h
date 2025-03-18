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

#ifndef __CAN2CQF_CREDITCALCULATOR_H_
#define __CAN2CQF_CREDITCALCULATOR_H_

#pragma once
#include <omnetpp.h>
#include "can4cqf/linklayer/message/CreditMessage_m.h"
using namespace omnetpp;


namespace CAN4CQF {

/**
 * TODO - Generated class
 */
class CreditCalculator : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
  private:
      double credit;          // 当前信用值（字节）
      double idleSlope;       // 信用增长速率
      double sendSlope;       // 信用消耗速率
      simtime_t lastUpdate;   // 上次更新时间

  public:
    // 函数功能：更新信用值（根据时间差和流量状态）
    // 参数：isSending - 是否正在发送数据
    // 返回值：当前信用值
    double updateCredit(bool isSending);

    template<typename T>
    const T& clamp(const T& value, const T& low, const T& high) {
        return (value < low) ? low : (high < value) ? high : value;
    }
};

} //namespace

#endif
