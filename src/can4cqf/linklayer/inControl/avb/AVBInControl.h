/*
 * AVBInControl.h
 *
 *  Created on: 2023年8月6日
 *      Author: xiaoyao
 */

#ifndef CAN4CQF_LINKLAYER_INCONTROL_AVB_AVBINCONTROL_H_
#define CAN4CQF_LINKLAYER_INCONTROL_AVB_AVBINCONTROL_H_

//CoRE4INET
#include "core4inet/utilities/ModuleAccess.h"
#include "core4inet/services/avb/SRP/SRPTable.h"
//CoRE4INET Auto-generated Messages
#include "core4inet/linklayer/ethernet/avb/AVBFrame_m.h"
#include "core4inet/linklayer/ethernet/base/EtherFrameWithQTag_m.h"
//INET Auto-generated Messages
#include "inet/linklayer/ethernet/EtherFrame_m.h"
// 导入CAN4CQF
#include "can4cqf/linklayer/message/IEEE8021QchTTFlow_m.h"
#include "can4cqf/linklayer/message/IEEE8021QchBEFlow_m.h"
using namespace CoRE4INET;
namespace CAN4CQF{
template<class IC>
class AVBInControl : public IC
{
    protected:
        /**
         * @brief Forwards frames to the appropriate incoming module
         *
         * An AVB frame arriving on in-gate is forwarded to the AVBctc module
         * or dropped if there is no module configured.
         *
         * @param msg incoming message
         */
        virtual void handleMessage(cMessage *msg) override;

    private:
        /**
         * @brief Helper function checks whether a Frame is AVB traffic.
         *
         * @param frame Pointer to the frame to check.
         * @return true if frame is critical, else false
         */
        virtual bool isAVB(const inet::EtherFrame *frame) const;
};

template<class IC>
void AVBInControl<IC>::handleMessage(cMessage *msg)
{
    if (msg && msg->arrivedOn("in"))
    {
        inet::EtherFrame *frame = dynamic_cast<inet::EtherFrame*>(msg);

        //Is AVB Frame?
        if (frame && isAVB(frame))
        {
            this->recordPacketReceived(frame);
            cSimpleModule::sendDirect(frame,
                    cModule::getParentModule()->getParentModule()->getSubmodule("avbCTC")->gate("in"));
        }
        else
        {
            IC::handleMessage(msg);
        }
    }
    else
    {
        IC::handleMessage(msg);
    }
}

template<class IC>
bool AVBInControl<IC>::isAVB(const inet::EtherFrame *frame) const
{
    bool result = false;
    if (const inet::EthernetIIFrame* ether2frame = dynamic_cast<const inet::EthernetIIFrame*>(frame))
    {
        if (ether2frame->getEtherType() == 0x8100)
        {
            if (const CoRE4INET::EthernetIIFrameWithQTag* qframe = dynamic_cast<const EthernetIIFrameWithQTag*>(frame))
            {
                SRPTable* srpTable = dynamic_cast<SRPTable*>(CoRE4INET::findModuleWhereverInNode("srpTable", cModule::getParentModule()));
                if (srpTable->containsStream(qframe->getDest(), qframe->getVID()))
                {
                    result = true;
                }
            }
            //##############################添加如下代码###################################
            else if(const IEEE8021QchTTFlow* qframe = dynamic_cast<const IEEE8021QchTTFlow*>(frame)){
                SRPTable* srpTable = dynamic_cast<SRPTable*>(CoRE4INET::findModuleWhereverInNode("srpTable", cModule::getParentModule()));
                if (srpTable->containsStream(qframe->getDest(), qframe->getVID()))
                {
                    result = true;
                }
            }
            else
            {
                throw cRuntimeError("Ethertype is 0x8100 but message type is not EthernetIIFrameWithQTag");
            }
        }
    }
    return result;
}
}



#endif /* CAN4CQF_LINKLAYER_INCONTROL_AVB_AVBINCONTROL_H_ */
