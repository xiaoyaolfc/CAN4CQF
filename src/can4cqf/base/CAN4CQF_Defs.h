/*
 * CAN4CQF_Defs.h
 *
 *  Created on: 2023年7月13日
 *      Author: xiaoyao
 */

#ifndef CAN4CQF_BASE_CAN4CQF_DEFS_H_
#define CAN4CQF_BASE_CAN4CQF_DEFS_H_
//OMNeT++
#include <omnetpp.h>
//INET
#include "inet/common/INETDefs.h"

#include "../features.h"

/*
 * @brief 定义CAN帧格式
 * */
#define START_OF_FRAME 1
#define IDENTIFIER 11
#define REMOTE_TRANSMISSION_REQUEST 11
#define IDENTIFIER_EXTENSION 1
#define RESERVED 4
#define DATA 64
#define CYCLIC_REDUNDANCY_CHECK 16
#define ACKNOWLEDGES 2
#define END_OF_FRAME 7
#define INTERFRAME_SPACE 7
/**
 * Check for minimal OMNeT++ Version requirements
 */
#if OMNETPP_VERSION < 0x0500
#  error At least OMNeT++/OMNEST version 5.0 required
#endif

/**
 * Definition of CAN4CQF version
 */
#define CAN4CQF_VERSION 0x0100
#endif /* CAN4CQF_BASE_CAN4CQF_DEFS_H_ */
