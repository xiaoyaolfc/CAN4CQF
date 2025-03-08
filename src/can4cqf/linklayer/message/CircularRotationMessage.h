/*
 * CircularRotationMessage.h
 *
 *  Created on: 2024年8月16日
 *      Author: xiaoyao
 */

#ifndef CAN4CQF_LINKLAYER_MESSAGE_CIRCULARROTATIONMESSAGE_H_
#define CAN4CQF_LINKLAYER_MESSAGE_CIRCULARROTATIONMESSAGE_H_
#include<omnetpp.h>
#include "fico4omnet/linklayer/can/messages/CanDataFrame_m.h"
#include "can4cqf/linklayer/message/CircularRotationMessage_m.h"
using namespace omnetpp;
using namespace std;

namespace CAN4CQF{
/*
 * @brief 需要自己定义一系列封装函数
 * */

class CircularRotationMessage : public CircularRotationMessage_Base{
private:
    std::map<FiCo4OMNeT::CanDataFrame*,double> crclrmap;
    void copy(const CircularRotationMessage& other);
public:
    /*
     * @brief 构造函数
     * */
    CircularRotationMessage(const char *Name=nullptr, short kind=0) : CircularRotationMessage_Base(Name,kind){

    }
    /*
     * @brief 拷贝构造函数
     * */
    CircularRotationMessage(const CircularRotationMessage& other):CircularRotationMessage_Base(other.getName()){
        operator = (other);
    }
    /*
     * @breif 复制函数
     * */
    virtual CircularRotationMessage *dup() const{
        return new CircularRotationMessage(*this);
    }
    /*
     * @brief 运算符函数
     * */
    CircularRotationMessage& operator=(const CircularRotationMessage& other){
        if(this==&other) return *this;
        CircularRotationMessage_Base::operator =(other);
        copy(other);
        return *this;
    }
    /*
     * @brief 定义封装函数，存储到达的报文和需要在目的网关等待的驻留时间
     * @parame map<canDataFrame*, double>
     * */
    virtual void encasulateCanDataFrameAndHoldTime(FiCo4OMNeT::CanDataFrame* candataframe, double hold_time);
    /*
     * @brief 解封装函数，并根据每个报文的驻留时间，等待一段时间后，再将其发送
     * */
    std::map<FiCo4OMNeT::CanDataFrame*,double> decapsulateCanDataFrameAndHoldTime();
    /*
        @brief 返回封装报文数量
    */
   int encapsulationCount();
   /*
        @brief 一次性获得map<dataframe,double>中所有元素，后续再调用封装函数，将报文封装入this中
   */
    void setAllCanDataFrameAndHoldTime(std::map<FiCo4OMNeT::CanDataFrame*,double> map);

};
}


#endif /* CAN4CQF_LINKLAYER_MESSAGE_CIRCULARROTATIONMESSAGE_H_ */
