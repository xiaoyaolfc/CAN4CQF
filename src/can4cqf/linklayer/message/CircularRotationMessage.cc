#include <can4cqf/linklayer/message/CircularRotationMessage.h>
#include <omnetpp.h>
using namespace std;
using namespace FiCo4OMNeT;
using namespace omnetpp;

namespace CAN4CQF{
Register_Class(CircularRotationMessage);

void CircularRotationMessage::copy(const CircularRotationMessage& other){
    for(auto crc:other.crclrmap){
        FiCo4OMNeT::CanDataFrame* dataframe = crc.first->dup();
//        take(dataframe);
        this->crclrmap[dataframe] = crc.second;
    }
}

void CircularRotationMessage::encasulateCanDataFrameAndHoldTime(FiCo4OMNeT::CanDataFrame* canframe, double holdtime){
//    take(canframe);
    this->crclrmap[canframe] = holdtime;
}

std::map<FiCo4OMNeT::CanDataFrame*,double> CircularRotationMessage::decapsulateCanDataFrameAndHoldTime(){
//    std::map<FiCo4OMNeT::CanDataFrame*,double> ret;
//    for(auto &crc:this->crclrmap){
//        drop(crc.first);
//        ret[crc.first] = crc.second;
//    }
//    return ret;
    return this->crclrmap;
}

int CircularRotationMessage::encapsulationCount(){
    return this->crclrmap.size();
}

void CircularRotationMessage::setAllCanDataFrameAndHoldTime(std::map<FiCo4OMNeT::CanDataFrame*,double> crclr){
    this->crclrmap = crclr;
}

}
