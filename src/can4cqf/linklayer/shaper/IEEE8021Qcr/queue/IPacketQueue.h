/*
 * IPacketQueue.h
 *
 *  Created on: 2025年3月18日
 *      Author: lfc84
 */

#ifndef CAN4CQF_LINKLAYER_SHAPER_IEEE8021QCR_QUEUE_IPACKETQUEUE_H_
#define CAN4CQF_LINKLAYER_SHAPER_IEEE8021QCR_QUEUE_IPACKETQUEUE_H_

#include <omnetpp.h>

using namespace omnetpp;

namespace CAN4CQF{
class IPacketQueue {
  public:
    virtual bool isEmpty() = 0;
    virtual int getNumPackets() = 0;
    virtual cPacket* getPacket(int index) = 0;
    virtual cPacket* removePacket(int index) = 0;
};
}


#endif /* CAN4CQF_LINKLAYER_SHAPER_IEEE8021QCR_QUEUE_IPACKETQUEUE_H_ */
