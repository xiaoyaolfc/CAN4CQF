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

#include "ResortAggregationGatewayBuffering.h"
//using namespace CoRE4INET;
using namespace FiCo4OMNeT;
using namespace std;
namespace CAN4CQF {

Define_Module(ResortAggregationGatewayBuffering);
ResortAggregationGatewayBuffering::ResortAggregationGatewayBuffering(){

}
ResortAggregationGatewayBuffering::~ResortAggregationGatewayBuffering(){
    cancelAndDelete(continuousFunctionMsg);
}
// 初始化，从.ned文件中读取设置参数
void ResortAggregationGatewayBuffering::initialize()
{
    this->maxNuminCQF = par("maxNuminCQF").intValue();
    this->timeout = par("timeout").doubleValue();
    handleParameterChange(nullptr);
    continuousFunctionMsg = new cMessage("ContinuousFunction");
    scheduleAt(simTime() + timeout, continuousFunctionMsg);
}

// 参数变更修改函数
void ResortAggregationGatewayBuffering::handleParameterChange(const char *parname){
    if (!parname || !strcmp(parname, "gatewayID"))
        {
            this->gatewayID = par("gatewayID").stringValue();
            if(this->gatewayID.empty() || !strcmp(this->gatewayID.c_str(), "auto")){
                //auto create id!
                this->gatewayID = this->getParentModule()->getParentModule()->getName();
            }
        }
}
// 接收CAN frame和封装发送的IEEE8021QchTTFlow，对CAN报文，进行封装补偿和重排序;对于TT流，将其解封装并逐个发送到transformation模块
void ResortAggregationGatewayBuffering::handleMessage(cMessage *msg)
{
    if(msg->isSelfMessage()){ //&& msg->getName() == "ContinuousFunction"
//        if(msg->getFullName() == "ContinuousFunction")
        if(ChecktimeinTCQF()){
            // 继续调度该判断程序
            scheduleAt(simTime() + timeout, msg);
        } else{
            // 将记录的报文顺序CanArrivalTime发送到transformation模块
             this->T_CQF_loopnum = this->T_CQF_loopnum + 1;
            ARTF* artf = Encap_orderinARTF(this->CanArrivalTime);
            if(artf->getordernum() == 0){
                delete artf;
                artf = nullptr;
            }else{
                send(artf,"out");
                this->CanArrivalTime.clear();
            }
            scheduleAt(simTime() + timeout, msg);
        }
    }else if(CanDataFrame* dataFrame = dynamic_cast<CanDataFrame*>(msg)){
        // 记录CAN报文到达数据
        GetArrivalTime(dataFrame);
    } else if (dynamic_cast<inet::EthernetIIFrame*>(msg)) {
        send(msg, gate("out"));
    } else if(dynamic_cast<IEEE8021QchTTFlow*>(msg)){
        send(msg,"out");
    } else if(dynamic_cast<IEEE8021QchBEFlow*>(msg)){
        send(msg,"out");
    } else{
        delete msg;
    }
//    delete msg;
}
// 记录CAN报文到达时间
void ResortAggregationGatewayBuffering::GetArrivalTime(FiCo4OMNeT::CanDataFrame* dataframe){
    this->CanArrivalTime.insert(pair<FiCo4OMNeT::CanDataFrame*, simtime_t>(dataframe,omnetpp::simTime()));
}
// 检查当前时间间隔
bool ResortAggregationGatewayBuffering::ChecktimeinTCQF(){
//    使用微秒显示
    double current_time = static_cast<double>(simTime().inUnit(SIMTIME_US));
    if(T_CQF_loopnum * timeout * 1e6 <= current_time && current_time < (T_CQF_loopnum + 1) * timeout * 1e6){
        return true;
    } else {
        return false;
    }
}
// 封装到达顺序进ARTF
ARTF* ResortAggregationGatewayBuffering::Encap_orderinARTF(std::map<FiCo4OMNeT::CanDataFrame*,simtime_t> CanArrivalTime){
    ARTF* artf = new ARTF();
    for(std::map<FiCo4OMNeT::CanDataFrame*,simtime_t>::iterator it = CanArrivalTime.begin() ;
            it != CanArrivalTime.end() ; ++it){
        artf->encap_order(it->first, it->second);
        artf->encapCanDataFrame(it->first);
    }
    return artf;
}
} //namespace
