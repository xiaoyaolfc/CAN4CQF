#include <can4cqf/linklayer/message/CircularRotationMessage.h>
#include "CircularRotationMessage_m.h"
#include <omnetpp.h>
using namespace std;
using namespace FiCo4OMNeT;
using namespace omnetpp;

namespace CAN4CQF{

Register_Class(CircularRotationMessage);

void CircularRotationMessage::copy(const CircularRotationMessage& other){
    // 清空当前 crclrmap，如果需要的话
    this->crclrmap.clear();
    for(auto crc : other.crclrmap){
        // 复制每个 CanDataFrame，并取得其所有权
        FiCo4OMNeT::CanDataFrame* dataframe = crc.first->dup();
        take(dataframe);
        this->crclrmap[dataframe] = crc.second;
    }
}

void CircularRotationMessage::encasulateCanDataFrameAndHoldTime(FiCo4OMNeT::CanDataFrame* canframe, double holdtime){
    // 转移消息所有权到当前对象
    take(canframe);
    this->crclrmap[canframe] = holdtime;
}

std::map<FiCo4OMNeT::CanDataFrame*, double> CircularRotationMessage::decapsulateCanDataFrameAndHoldTime() {
    std::map<FiCo4OMNeT::CanDataFrame*, double> ret;
    for (auto &crc : this->crclrmap) {
        // 只有当当前对象是该消息的所有者时才调用 drop()
        if (crc.first->getOwner() == this)
            drop(crc.first);
        ret[crc.first] = crc.second;
    }
    this->crclrmap.clear();
    return ret;
}


int CircularRotationMessage::encapsulationCount(){
    return this->crclrmap.size();
}

void CircularRotationMessage::setAllCanDataFrameAndHoldTime(std::map<FiCo4OMNeT::CanDataFrame*,double> crclr){
    // 注意这里，如果需要转移所有权，可能要先 drop 原来的消息
    // 此处假设 crclr 中的消息所有权已经转移过来了
    this->crclrmap = crclr;
}

std::list<FiCo4OMNeT::CanDataFrame*> CircularRotationMessage::getCanDataFrameList(){
    std::list<FiCo4OMNeT::CanDataFrame*> crcTolist;
    for(auto crc:this->crclrmap){
        crcTolist.push_back(crc.first->dup());
    }
    return crcTolist;
}


} // namespace CAN4CQF
