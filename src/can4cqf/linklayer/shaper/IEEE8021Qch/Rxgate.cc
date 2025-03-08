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

#include "Rxgate.h"
// 导入std
#include<limits>
// 导入CORE4INET
#include "core4inet/utilities/ConfigFunctions.h"
#include "core4inet/linklayer/shaper/IEEE8021Qbv/gate/IEEE8021QbvGate.h"
#include "core4inet/scheduler/SchedulerMessage_m.h"
/*
 * Rxgate的功能是根据.ned文件给的GCL，将后续来的报文发送到相应的队列中，所以大致实现思路如下：
 * 1.从.ned中读取GCL
 * 2.判断GCL的门控情况，重构为（门控状态，持续时间）
 * 3.根据2中的二元组，将报文发送到Buffer中
 * */
namespace CAN4CQF {

Define_Module(Rxgate);

Rxgate::Rxgate(){// 设置构造函数的初值
    this->numGates=2;
    this->configNo=0;
}
// 从.ned文件中读取门控列表
void Rxgate::setGateControlList(string gcl){
    this->par("controlList").setStringValue(gcl);
    this->handleParameterChange("controlList");// 调用处理函数，将controlList划分为二元组
}

void Rxgate::initialize(int stage)// 分阶段初始化
{
    if(stage==0){
        WATCH(this->configNo);
        WATCH(this->timerEventName);
    }
}
int Rxgate::numInitStages() const{
    return 2;
}

void Rxgate::handleParameterChange(const char* parname){
// 当GCL发生变化时，对其进行处理，判断是否为'o' OR 'C'，并根据状态发送到相应队列中
    CoRE4INET::Scheduled::handleParameterChange(parname);
    if (!parname || !strcmp(parname, "numGates"))//判断是否是"numGates"，如果是则更新
        {
            this->numGates = static_cast<unsigned int>(CoRE4INET::parameterULongCheckRange(par("numGates"), 1, std::numeric_limits<int>::max()));
        }
    if (!parname || !strcmp(parname, "controlList"))//判断是否是"controlList"。如果是则更新
    {
        gateControlList.clear();
        vector<string> controlRows = cStringTokenizer(par("controlList"), ";").asVector();//用";"分隔controlList为门控状态元组和时间元组
        for (vector<string>::const_iterator controlRow = controlRows.begin(); controlRow != controlRows.end(); ++controlRow)
        {
            vector<string> controlRowTupel = cStringTokenizer((*controlRow).c_str(), ":").asVector();//用":"分隔为门控状态
            vector<string> controlRowGates = cStringTokenizer(controlRowTupel[0].c_str(), ",").asVector();
            if (controlRowGates.size() != this->numGates)
            {
                throw cRuntimeError("controlList row size is not %d", this->numGates);
            }
            for (size_t i=0; i<controlRowGates.size(); i++)
            {
                if (strcmp(controlRowGates[i].c_str(), "o") && strcmp(controlRowGates[i].c_str(), "C"))
                {
                    throw cRuntimeError("controlList contains unexpected character \'%s\'. Allowed are \'o\' (OPEN) or \'C\' (CLOSED).", controlRowGates[i].c_str());
                }
            }
            double controlRowTime = stod(controlRowTupel[1]);//创建一个新的controlRowTime存储正确的门控时间援助
            gateControlList.push_back(make_pair(controlRowGates, controlRowTime));// 将门控状态元组和对应的时间元组匹配创建一个新的门控列表
        }
        if (this->gateControlList.size() > 0)//更新相关参数
        {
            this->switchToFirstGateControlElement();
            this->configNo++;
            this->timerEventName = "Gate Control List Config " + to_string(this->configNo) + " Scheduler Event";
            this->scheduleCurrentGateControlElementTime(false);
        }
    }

}
void Rxgate::handleMessage(cMessage *msg)
{
    // 根据GCL判断报文发向哪个门
    if (msg->arrivedOn("schedulerIn") && msg->getKind() == CoRE4INET::ACTION_TIME_EVENT)
        {
            if (!strcmp(this->timerEventName.c_str(), msg->getName()))
            {
//                this->propagateGateControlElement(this->gateControlElement->first);
                if (this->gateControlList.size() > 1)
                {
                    this->switchToNextGateControlElement();
                    this->scheduleCurrentGateControlElementTime(false);
                }
            } delete msg;
        } else if(msg->arrivedOn("rxin")){
            // 根据GCL发送报文
            if(IEEE8021QchTTFlow* ttflow = dynamic_cast<IEEE8021QchTTFlow*>(msg)){
                ForwardTTFlowToQueue(ttflow);
            }
        }else{
            delete msg;
        }
//    delete msg;
}
void Rxgate::scheduleCurrentGateControlElementTime(bool nextCycle)
{// 根据当前的门控元件，安排下一个门控事件。
    CoRE4INET::SchedulerActionTimeEvent* actionTimeEvent = new CoRE4INET::SchedulerActionTimeEvent(this->timerEventName.c_str(), CoRE4INET::ACTION_TIME_EVENT);
    uint32_t actionTime = static_cast<uint32_t>(ceil(this->gateControlElement->second / this->getOscillator()->getPreciseTick()));
    if (actionTime >= this->getPeriod()->getCycleTicks())
    {
        throw cRuntimeError("The send window (%d ticks) starts outside of the period (%d ticks)",
                actionTime, this->getPeriod()->getCycleTicks());
    }
    actionTimeEvent->setAction_time(actionTime);
    actionTimeEvent->setNext_cycle(nextCycle);
    actionTimeEvent->setDestinationGate(this->gate("schedulerIn"));
    this->getPeriod()->registerEvent(actionTimeEvent);
}

void Rxgate::switchToFirstGateControlElement()
{
    if (this->gateControlList.size() > 0)
    {
        this->gateControlElement = this->gateControlList.begin();
        simtime_t currentTimeInPeriod = this->getPeriod()->getTicks() * this->getOscillator()->getPreciseTick();
        for (auto candidateGateControlElement = this->gateControlList.begin(); candidateGateControlElement != this->gateControlList.end(); ++candidateGateControlElement)
        {
            if (candidateGateControlElement->second >= currentTimeInPeriod.dbl())
            {
                this->gateControlElement = candidateGateControlElement;
                break;
            }
        }
        if (this->gateControlElement->second != currentTimeInPeriod.dbl())
        {
            vector<pair<vector<string>, double>>::iterator preGateControlElement;
            if (this->gateControlElement == this->gateControlList.begin())
            {
                preGateControlElement = this->gateControlList.end();
            }
            else
            {
                preGateControlElement = this->gateControlElement;
            }
            --preGateControlElement;
//            this->propagateGateControlElement(preGateControlElement->first);
        }
    }
}

void Rxgate::switchToNextGateControlElement()
{
    ++(this->gateControlElement);
    if (this->gateControlElement == this->gateControlList.end())
    {
        this->gateControlElement = this->gateControlList.begin();
    }
}

// ###############################################
// 函数逻辑：从gateControlList的value遍历，找到对应的time-slot，读取当前Rxgate门控情况，将报文发送到gate-state=C的队列中
void Rxgate::ForwardTTFlowToQueue(IEEE8021QchTTFlow* ttflow){
    double currentTime = simTime().dbl();
    bool loopCompleted = false;
    for(std::vector<pair<vector<string>, double>>::iterator it1=(this->gateControlList).begin();
            it1!=(this->gateControlList).end() ; ++it1){
        if(gateControlList.size()==1){
            send(ttflow,"rxout",0);
            loopCompleted = true;
            break;
        } else{
            if(currentTime > (*it1).second){
                continue;
            } else{
                if((*it1).first.front() == "o"){
                    send(ttflow, "rxout", 0);
                    loopCompleted = true;
                    break;
                } else{
                    send(ttflow, "rxout", 1);
                    loopCompleted = true;
                    break;
                }
            }
        }
    }
    if(!loopCompleted){
        send(ttflow,"rxout",1);
    }
}

} //namespace
