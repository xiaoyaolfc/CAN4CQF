/*
 * InControls.cc
 *
 *  Created on: 2023年8月6日
 *      Author: xiaoyao
 */
#include "can4cqf/linklayer/inControl/InControls.h"
using namespace CAN4CQF;
using namespace CoRE4INET;
Define_Module(BE_InControl);
BE_InControl::~BE_InControl()
{
}
Define_Module(IEEE8021Qch_InControl);
IEEE8021Qch_InControl::~IEEE8021Qch_InControl()
{
}
Define_Module(AVB_8021Q_InControl);
AVB_8021Q_InControl::~AVB_8021Q_InControl()
{
}
