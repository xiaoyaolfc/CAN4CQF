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

#ifndef __CAN4CQF_TTAPPLICATIONBASE_H_
#define __CAN4CQF_TTAPPLICATIONBASE_H_

#include <omnetpp.h>
//Std
#include <unordered_map>

//CoRE4INET
#include "core4inet/applications/base/ApplicationBase.h"
#include "core4inet/buffer/base/Buffer.h"

using namespace omnetpp;
using namespace CoRE4INET;

namespace CAN4CQF {

/**
 * @brief IEEE8021Qch APP模块的基础部分，包含对buffer的映射
 */
class TTApplicationBase : public virtual ApplicationBase
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    /*
     * @brief 存储TT流发送buffer
     * */
    std::unordered_map<uint16_t, std::list<Buffer*> > buffer;
    /*
     * @brief 表明参数发生变化
     * */
    virtual void handleParameterChange(const char* parname) override;
  public:
    /*
     * @brief 构造函数
     * */
    TTApplicationBase();
    /*
     * @breif 析构函数
     * */
    ~TTApplicationBase();

};

} //namespace

#endif
