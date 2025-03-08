/*
 * ARTF.cc
 *
 *  Created on: 2023年8月7日
 *      Author: xiaoyao
 */
#include<can4cqf/linklayer/message/ARTF.h>
#include <omnetpp.h>
using namespace std;
using namespace FiCo4OMNeT;
using namespace omnetpp;

namespace CAN4CQF{
Register_Class(ARTF);
void ARTF::copy(const ARTF& other){
    for(std::list<FiCo4OMNeT::CanDataFrame*>::const_iterator it = other.ARTFList.begin() ;
            it != other.ARTFList.end() ; ++it){
        FiCo4OMNeT::CanDataFrame* dataframe = (*it)->dup();
        take(dataframe);
        this->ARTFList.push_back(dataframe);
    }
}
// 封装函数
void ARTF::encapCanDataFrame(FiCo4OMNeT::CanDataFrame* dataframe){
    take(dataframe);
//    FiCo4OMNeT::CanDataFrame* DataFrame = dataframe;  // 使用 dup() 创建对象的副本
    setByteLength(getByteLength() + dataframe->getByteLength());
    ARTFList.push_back(dataframe);
}
// 解封装函数
FiCo4OMNeT::CanDataFrame* ARTF::decapARTF(){
    FiCo4OMNeT::CanDataFrame* dataframe = nullptr;
    if(ARTFList.size()){
        dataframe = ARTFList.front();
        setByteLength(getByteLength() - dataframe->getByteLength());
        ARTFList.pop_front();
        drop(dataframe);
    }
    return dataframe;
}
std::list<FiCo4OMNeT::CanDataFrame*> ARTF::getEncapUnits() const{
    return this->ARTFList;
}
size_t ARTF::getEncapCnt(){
    return this->ARTFList.size();
}
void ARTF::encap_order(FiCo4OMNeT::CanDataFrame* dataframe, simtime_t arr_time){
    setByteLength(getByteLength() + dataframe->getByteLength());
    this->order_GW.insert(pair<FiCo4OMNeT::CanDataFrame*, simtime_t>(dataframe->dup(), arr_time));
}
simtime_t ARTF::decap_order_arrtime(FiCo4OMNeT::CanDataFrame* dataframe){
    for(std::map<FiCo4OMNeT::CanDataFrame*, simtime_t>::iterator it = order_GW.begin();
            it != order_GW.end(); ++it){
        if(it->first->getCanID() == dataframe->getCanID()){
            return it->second;
        }else{
            continue;
        }
    }
    return 0;
}
size_t ARTF::getordernum(){
    return order_GW.size();
}
}

/*
 * void ARTF::encapCanDataFrame(FiCo4OMNeT::CanDataFrame* dataframe){
    take(dataframe->dup());
    FiCo4OMNeT::CanDataFrame* dupDataFrame = dataframe->dup();  // 使用 dup() 创建对象的副本
    setByteLength(getByteLength() + dataframe->getByteLength());
    ARTFList.push_back(dupDataFrame);
}
 *
 * */
