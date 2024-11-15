/*
 * ARTF.h
 *
 *  Created on: 2023年8月7日
 *      Author: xiaoyao
 */

#ifndef CAN4CQF_LINKLAYER_MESSAGE_ARTF_H_
#define CAN4CQF_LINKLAYER_MESSAGE_ARTF_H_

#include <omnetpp.h>
#include<list>
#include "can4cqf/linklayer/message/ARTF_m.h"
#include "fico4omnet/linklayer/can/messages/CanDataFrame_m.h"

using namespace omnetpp;
namespace CAN4CQF{
/*
 * @breif 使用自定函数实现多CAN包封装功能
 * */
class ARTF : public ARTF_Base{
  private:
    std::list<FiCo4OMNeT::CanDataFrame*> ARTFList;
    void copy(const ARTF& other);
  public:
    /*
     * @brief ARTF的构造函数
     * */
    ARTF(const char *Name=nullptr, short kind=0) : ARTF_Base(Name,kind){
    }
    /*
     * @brief copy函数
     * */
//    ARTF(const ARTF& other) : ARTF_Base(other){
//        copy(other);
//    }
    ARTF(const ARTF& other) : ARTF_Base(other.getName()){
        operator = (other);
    }
    /**
     * @brief ARTF duplication
     */
    virtual ARTF *dup() const
    {
        return new ARTF(*this);
    }
    /*
     * @brief Assignment operator
     * */
    ARTF& operator=(const ARTF& other){
        if(this==&other) return *this;
        ARTF_Base::operator =(other);
        copy(other);
        return *this;
    }
    /*
     * @brief 封装CAN帧
     * */
    virtual void encapCanDataFrame(FiCo4OMNeT::CanDataFrame*);
    /*
     * @brief 解封装
     * */
    FiCo4OMNeT::CanDataFrame* decapARTF();
    /*
     * @brief 返回封装的ARTFList
     * */
    std::list<FiCo4OMNeT::CanDataFrame*> getEncapUnits() const;
    /*
     * @brief 返回封装报文的个数
     * */
    virtual size_t getEncapCnt();
    /*
     * @brief 存储到达顺序
     * */
    std::map<FiCo4OMNeT::CanDataFrame*, simtime_t> order_GW;
    /*
     * @brief 存储到达顺序的函数
     * */
    void encap_order(FiCo4OMNeT::CanDataFrame*, simtime_t);
    /*
     * @brief 解封装到达顺序的时间戳
     * */
    simtime_t decap_order_arrtime(FiCo4OMNeT::CanDataFrame*);
    /*
     * @brief 返回ARTF封装的order中CAN报文数量
     * */
    size_t getordernum();
};
}



#endif /* CAN4CQF_LINKLAYER_MESSAGE_ARTF_H_ */
