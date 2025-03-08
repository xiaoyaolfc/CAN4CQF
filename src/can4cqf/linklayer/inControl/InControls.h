/*
 * InControls.h
 *
 *  Created on: 2023年8月6日
 *      Author: xiaoyao
 */

#ifndef CAN4CQF_LINKLAYER_INCONTROL_INCONTROLS_H_
#define CAN4CQF_LINKLAYER_INCONTROL_INCONTROLS_H_

//CoRE4INET
#include "core4inet/linklayer/inControl/base/BaseInControl.h"
#include "core4inet/linklayer/inControl/base/BEInControl.h"
//CoRE4INET
#include "core4inet/linklayer/inControl/AS6802/CTInControl.h"
//CoRE4INET
//#include "core4inet/linklayer/inControl/avb/AVBInControl.h"
//CoRE4INET
//#include "core4inet/linklayer/inControl/IEEE8021Q/IEEE8021QInControl.h"
// can4cqf
#include "can4cqf/linklayer/message/IEEE8021QchTTFlow_m.h"
#include "can4cqf/linklayer/message/IEEE8021QchBEFlow_m.h"
#include "can4cqf/linklayer/inControl/avb/AVBInControl.h"
#include "can4cqf/linklayer/inControl/IEEE8021Qch_InControl/IEEE8021QchInControl.h"

namespace CAN4CQF {

/**
 * @brief Class representing the BE_InControl module
 *
 * This module only handles incoming best-effort traffic.
 *
 * @see BaseInControl
 *
 * @author Till Steinbach
 */
class BE_InControl : public BEInControl<BaseInControl>
{
    public:
        virtual ~BE_InControl();
};

/**
 * @brief Class representing the BE_InControl module
 *
 * This module only handles incoming best-effort traffic.
 *
 * @see BaseInControl
 *
 * @author Till Steinbach
 */
class IEEE8021Qch_InControl : public IEEE8021QchInControl<BaseInControl>
{
    public:
        virtual ~IEEE8021Qch_InControl();
};

/**
 * @brief Class representing the AVB_8021Q_InControl
 *
 * This module handles incoming AVB and IEEE 802.1Q traffic
 *
 * @see BaseInControl, IEEE8021Q_InControl
 *
 * @author Philipp Meyer
 */
class AVB_8021Q_InControl : public AVBInControl<IEEE8021QchInControl<BaseInControl> >
{
    public:
        virtual ~AVB_8021Q_InControl();
};
}
#endif /* CAN4CQF_LINKLAYER_INCONTROL_INCONTROLS_H_ */
