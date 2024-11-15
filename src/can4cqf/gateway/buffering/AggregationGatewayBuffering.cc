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

#include "AggregationGatewayBuffering.h"
using namespace FiCo4OMNeT;
using namespace std;

namespace CAN4CQF {

Define_Module(AggregationGatewayBuffering);
AggregationGatewayBuffering::AggregationGatewayBuffering(){

}
AggregationGatewayBuffering::~AggregationGatewayBuffering(){
    for (map<cMessage*, simtime_t>::iterator it = scheduledTimes.begin();
                it != scheduledTimes.end(); ++it) {
            if (it->first) {
                cancelAndDelete(it->first);
            }
        }
}

void AggregationGatewayBuffering::initialize()
{
    //  为约束阈值赋初值
    this->alpha = par("alpha").doubleValue();
    //  同上
    this->gamma = par("gamma").intValue();
    // CAN业务流划分
    this->sigma = par("sigma").doubleValue();
    this->MaxHRTFID = par("MaxHRTFID").intValue();
    this->Tmax = par("Tmax").doubleValue();
    handleParameterChange(nullptr);
    readConfigXML();

    size_t numPools = scheduledHoldUpTimes.size();
    for (size_t i = 0; i < numPools; i++) {
        string strBufHoldUpTime = "pool" + to_string(i) + "HoldUpTime";
        string strBufPoolSize = "pool" + to_string(i) + "MessageSize";

        simsignal_t signalHoldUpTime = registerSignal(strBufHoldUpTime.c_str());
        cProperty *statisticTemplateHoldUpTime = getProperties()->get(
                "statisticTemplate", "poolHoldUpTime");
        getEnvir()->addResultRecorders(this, signalHoldUpTime, strBufHoldUpTime.c_str(),
                statisticTemplateHoldUpTime);
        poolHoldUpTimeSignals.push_back(signalHoldUpTime);

        simsignal_t signalPoolSize = registerSignal(strBufPoolSize.c_str());
        cProperty *statisticTemplatePoolSize = getProperties()->get(
                "statisticTemplate", "poolMessageSize");
        getEnvir()->addResultRecorders(this, signalPoolSize, strBufPoolSize.c_str(),
                statisticTemplatePoolSize);
        poolSizeSignals.push_back(signalPoolSize);
    }
}

void AggregationGatewayBuffering::handleParameterChange(const char *parname){
    if (!parname || !strcmp(parname, "gatewayID"))
        {
            this->gatewayID = par("gatewayID").stringValue();
            if(this->gatewayID.empty() || !strcmp(this->gatewayID.c_str(), "auto")){
                //auto create id!
                this->gatewayID = this->getParentModule()->getParentModule()->getName();
            }
        }
}

void AggregationGatewayBuffering::readConfigXML(){
    cXMLElement* xmlDoc = par("configXML").xmlValue();
    string gatewayName = this->gatewayID;
    string xpath = "/config/gateway[@id='" + gatewayName + "']/buffering";
    cXMLElement* xmlBuffering = xmlDoc->getElementByPath(xpath.c_str(), xmlDoc);
    if (xmlBuffering) {
        cXMLElementList xmlPools = xmlBuffering->getChildrenByTagName("pool");
        for (size_t i = 0; i < xmlPools.size(); i++) {
            cMessagePointerList* poolList = new cMessagePointerList();
            cMessage* holdUpTimeEvent = new cMessage();
            scheduledHoldUpTimes.insert(
                    pair<cMessagePointerList*, cMessage*>(poolList,
                            holdUpTimeEvent));
            scheduledTimes.insert(
                    pair<cMessage*, simtime_t>(holdUpTimeEvent, 0));
            poolArrivalTimes.insert(
                    pair<cMessagePointerList*, list<simtime_t>>(poolList,
                            list<simtime_t>()));
            cXMLElementList xmlHoldUpTimes = xmlPools[i]->getChildren();
            for (size_t j = 0; j < xmlHoldUpTimes.size(); j++) {
                simtime_t holdUpTime = SimTime::parse(
                        xmlHoldUpTimes[j]->getAttribute("time"));
                cXMLElementList xmlPoolMessages =
                        xmlHoldUpTimes[j]->getChildren();
                for (size_t k = 0; k < xmlPoolMessages.size(); k++) {
                    unsigned int canID = static_cast<unsigned int>(atoi(
                            xmlPoolMessages[k]->getAttribute("canId")));
                    poolMap.insert(
                            pair<unsigned int, cMessagePointerList*>(canID,
                                    poolList));
                    holdUpTimes.insert(
                            pair<unsigned int, simtime_t>(canID, holdUpTime));
                }
            }
        }
    }
}
void AggregationGatewayBuffering::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        cMessagePointerList* poolList = getPoolList(msg);
        unsigned int poolID = getPoolID(poolList);
        emitSignals(poolList);
        PoolMessage* poolMsg = new PoolMessage(("Pool " + to_string(poolID)).c_str());
        poolMsg->setPool(*poolList);
        poolList->clear();
//        send(poolMsg, gate("out"));
    } else if (CanDataFrame* dataFrame = dynamic_cast<CanDataFrame*>(msg)) {
        // 存储到达的Can报文时间
        GetArrivalTime(dataFrame);
//        ######################################################        //
        // 进入业务流判断函数
        if(divisionTrafficFlow(dataFrame)){
            AdjustEncapsulationCanFrame(dataFrame);
        }else{
            // 如果是HRTF，应该是直接打包发送
            HRTF* hrtf = new HRTF();
            CanArrivalTime.erase(dataFrame);
            hrtf->setEncapnum(1);
            hrtf->encapsulate(dataFrame);
            send(hrtf, "out");
        }
//         ###################################################      //
    } else if (dynamic_cast<inet::EthernetIIFrame*>(msg)) {
        send(msg, gate("out"));
    }
    else if(dynamic_cast<IEEE8021QchTTFlow*>(msg)){
        send(msg,"out");
    } else if(dynamic_cast<IEEE8021QchBEFlow*>(msg)){
        send(msg,"out");
    }
}
cMessagePointerList* AggregationGatewayBuffering::getPoolList(
        unsigned int canID) {
    return poolMap[canID];
}
cMessagePointerList* AggregationGatewayBuffering::getPoolList(
        cMessage* holdUpTimeEvent) {
    map<cMessagePointerList*, cMessage*>::iterator it;
    for (it = scheduledHoldUpTimes.begin(); it != scheduledHoldUpTimes.end();
            ++it) {
        if (it->second == holdUpTimeEvent) {
            return it->first;
        }
    }
    throw 0;
}

simtime_t AggregationGatewayBuffering::getIDHoldUpTime(unsigned int canID) {
    return holdUpTimes[canID];
}

cMessage* AggregationGatewayBuffering::getPoolHoldUpTimeEvent(
        cMessagePointerList* poolList) {
    return scheduledHoldUpTimes[poolList];
}

simtime_t AggregationGatewayBuffering::getCurrentPoolHoldUpTime(
        cMessage* poolHoldUpTimeEvent) {
    simtime_t scheduledTime = scheduledTimes[poolHoldUpTimeEvent];
    return scheduledTime - simTime();
}

void AggregationGatewayBuffering::emitSignals(cMessagePointerList* poolList) {
    if (simTime() > getSimulation()->getWarmupPeriod()) {
        unsigned int poolID = getPoolID(poolList);
        list<simtime_t>::iterator it;
        for (it = poolArrivalTimes[poolList].begin();
                it != poolArrivalTimes[poolList].end(); ++it) {
            simtime_t holdUpTime = simTime() - *it;
            emit(poolHoldUpTimeSignals[poolID], holdUpTime);
        }
        poolArrivalTimes[poolList].clear();
        emit(poolSizeSignals[poolID], static_cast<unsigned long>(poolList->size()));
    }
}

unsigned int AggregationGatewayBuffering::getPoolID(
        cMessagePointerList* poolList) {
    map<cMessagePointerList*, cMessage*>::iterator it1;
    unsigned int poolID = 0;
    for (it1 = scheduledHoldUpTimes.begin(); it1 != scheduledHoldUpTimes.end();
            ++it1) {
        if (it1->first == poolList) {
            break;
        }
        poolID++;
    }
    return poolID;
}
// ###################################################################################### //
// 划分优先级函数，输入参数应为CAN msg，输出参数为类型判断变量，用一个bool类型表示即可，0表示HRTF，1表示ARTF
bool AggregationGatewayBuffering::divisionTrafficFlow(cMessage* msg){
//    double Tmax = par("Tmax").doubleValue();
    if(CanDataFrame* canFrame = dynamic_cast<CanDataFrame*>(msg)){
        if(canFrame->getPeriod() > this->Tmax * sigma){
            // 周期>HRTF阈值，则判断为ARTF
            return true;
        }else{
            // 当CANID优先级较高，且周期较小时，才认为是HRTF
            if(canFrame->getCanID() <= MaxHRTFID){
                // 判断为HRTF
                return false;
            }// 否则仍然是ARTF
            return true;
        }
    }
    // 默认返回0,表示HRTF
    return false;
}
// 根据约束条件，进行动态封装，需要根据CanDataFrame的业务流类型选择封装的顺序，对于ARTF进一步划分为四个优先级
void AggregationGatewayBuffering::AdjustEncapsulationCanFrame(cMessage* msg){
    CanDataFrame* dataFrame = dynamic_cast<CanDataFrame*>(msg);
    cMessagePointerList* poolList;
    poolList = getPoolList(dataFrame->getCanID());
    if (poolList != nullptr) {
        cMessage* poolHoldUpTimeEvent = getPoolHoldUpTimeEvent(poolList);
        simtime_t IDHoldUpTime = getIDHoldUpTime(dataFrame->getCanID());
        if (poolList->empty()
                || (IDHoldUpTime
                        < getCurrentPoolHoldUpTime(poolHoldUpTimeEvent))) {
            simtime_t eventTime = IDHoldUpTime + simTime();
            cancelEvent(poolHoldUpTimeEvent);
            scheduleAt(eventTime, poolHoldUpTimeEvent);
            scheduledTimes[poolHoldUpTimeEvent] = eventTime;
        }
    }
//      对当前到达的报文进行判断，遍历所有ARTF，检查是否有报文的deadline > 时延约束，如果有则停止封装，直接发送到out门
    if(ARTFList.empty()){
        ARTFList.push_back(dataFrame);
    } else{
        if(CalculateReservationDeadline(ARTFList)){
            ARTFList.push_back(dataFrame);
        } else{
            ARTFList.clear();
        }
    }

}

void AggregationGatewayBuffering::GetArrivalTime(FiCo4OMNeT::CanDataFrame* dataframe){
    this->CanArrivalTime.insert(pair<FiCo4OMNeT::CanDataFrame*, simtime_t>(dataframe,omnetpp::simTime()));
}

bool AggregationGatewayBuffering::CalculateReservationDeadline(std::list<FiCo4OMNeT::CanDataFrame*> ARTFList){
    for(std::list<FiCo4OMNeT::CanDataFrame*>::iterator it=ARTFList.begin();
            it!=ARTFList.end();++it){
        FiCo4OMNeT::CanDataFrame* canDataFrame = *it;
        simtime_t arrivalTime = CanArrivalTime[canDataFrame];
        simtime_t ReserveTime = omnetpp::simTime()- arrivalTime;
        // 将上述报文添加到ReserveDeadline中
        ReserveDeadline.insert(pair<FiCo4OMNeT::CanDataFrame*, simtime_t>(canDataFrame,ReserveTime));
//        ReserveDeadline[canDataFrame] = omnetpp::simTime()- arrivalTime;
        // 当前CAN报文的deadline = 当前仿真时间-CAN报文到达时间
        if(ReserveDeadline[canDataFrame].dbl() < (alpha * canDataFrame->getPeriod())){
            continue;
        }else{
            ARTF* artf = new ARTF();
            // 进入else后应该对以封装的list<CanDataFrame>进行重新排序
//            artf->setEncapnum(ARTFList.size());
            std::list<FiCo4OMNeT::CanDataFrame*> ARTFListdup = SortSequence(ARTFList);
            for(std::list<FiCo4OMNeT::CanDataFrame*>::iterator it = ARTFListdup.begin() ;
                    it != ARTFListdup.end() ; ++it){
                artf->encapCanDataFrame((*it));
            }
            send(artf,"out");
            CanArrivalTime.clear();
            ReserveDeadline.clear();
            return false;
        }
    }
    return true;
}
/*
void AggregationGatewayBuffering::HRTFDecapsulate(HRTF* hrtf){
    FiCo4OMNeT::CanDataFrame* DataFrame = dynamic_cast<CanDataFrame*>(hrtf->getPayload());
    send(DataFrame,"out");
}

void AggregationGatewayBuffering::ARTFDecapsulate(ARTF* artf){
    for(std::list<FiCo4OMNeT::CanDataFrame*>::iterator it = artf->getPayload().begin();
            it !=  artf->getPayload().end() ; ++it){
       send((*it),"out");
    }
}
*/
/*
 * 函数逻辑：首先遍历ARTFList，用map<CanDataFrame*,simtime_t>-CanArrivalTime 存储当前所有list中的等待时间。
 * 其次，根据（等待时间/Can->period）升序排列，再根据大类业务流划分出LRTF，将其直接放在队列最后。
 * 然后使用动态规划的思想，比较前后几段是否为同一个数量级，如果是，则继续比较其ID，将ID小的放前面。
 * 最后将LRTF放到末尾即可。
 * */
std::list<FiCo4OMNeT::CanDataFrame*> AggregationGatewayBuffering::SortSequence(std::list<FiCo4OMNeT::CanDataFrame*> ARTFList){
    std::list<FiCo4OMNeT::CanDataFrame*> ARTFcopy;
    for(std::list<FiCo4OMNeT::CanDataFrame*>::iterator it = ARTFList.begin() ; it != ARTFList.end() ; ++it){
        double NAT = SIMTIME_DBL(ReserveDeadline[(*it)])/(*it)->getPeriod();
        if(NormalizedAwaitingTime.empty()){
            NormalizedAwaitingTime.insert(pair<FiCo4OMNeT::CanDataFrame*, double>((*it), NAT));
            ARTFcopy.push_back(*it);
        } else{ //当为非空的时候就需要逐个检测比较大小
            for(std::list<FiCo4OMNeT::CanDataFrame*>::iterator it1 = ARTFcopy.begin() ; it1 != ARTFList.end() ; ++it){
                double nowNAT = NormalizedAwaitingTime.find(*it1)->second/NAT;
                if(nowNAT > 1){
                    continue;
                } else{
                    ARTFcopy.insert(it1,*it);
                }
            }
            NormalizedAwaitingTime.insert(pair<FiCo4OMNeT::CanDataFrame*, double>((*it), NAT));
        }
    }
    NormalizedAwaitingTime.clear();
    return ARTFcopy;
}


} //namespace
