//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include <can4cqf/applications/TSNCANHost/Transformation.h>
#include "fico4omnet/linklayer/can/messages/CanDataFrame_m.h"
using namespace FiCo4OMNeT;
using namespace inet;

namespace CAN4CQF {

Define_Module(Transformation);

// 初始化参数和验证门连接
void Transformation::initialize() {
    timeout = par("timeout");  // 必须读取NED参数

    // 验证门连接完整性
    if (!gate("extGate$i")->isConnected() ||   // 明确检查输入方向
            !gate("canGate$i")->isConnected()) {  // 同理处理canGate
            throw cRuntimeError("必须同时连接extGate和canGate门!");
        }
}

// 核心消息处理逻辑
void Transformation::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage()) {
        handleSelfMessage(msg);
    } else if (msg->arrivedOn("extGate$i")) {  // 来自外部网络的TSN流量
        handleExternalMessage(msg);
    } else if (msg->arrivedOn("canGate$i")) {  // 来自CAN节点的反馈
        handleCanFeedback(msg);
    } else {
        delete msg;  // 防御性编程
    }
}

// 自消息处理（定时发送CAN帧）
void Transformation::handleSelfMessage(cMessage *msg) {
    std::string targetID = msg->getName();
    bool found = false;

    for (auto it = canfdNeedResendList.begin(); it != canfdNeedResendList.end();) {
        if (std::to_string((*it)->getCanID()) == targetID) {
            // 克隆消息避免指针冲突
            FiCo4OMNeT::CanDataFrame* canFrame = (*it)->dup();
            EV << "CQF周期触发发送CAN ID: " << canFrame->getCanID() << endl;

            // 通过canGate发送到CAN节点
            send(canFrame, "canGate$o");

            // 清理原消息
            if ((*it)->getOwner() == this) delete *it;
            it = canfdNeedResendList.erase(it);

            found = true;
            break;
        } else {
            ++it;
        }
    }

    if (!found) {
        EV_WARN << "未找到匹配CAN ID: " << targetID << "，重新调度" << endl;
        scheduleAt(simTime() + timeout, msg);
    } else {
        delete msg;  // 清理自消息
    }
}

// 处理外部TSN流量
void Transformation::handleExternalMessage(cMessage *msg) {
    if (auto ttFlow = dynamic_cast<IEEE8021QchTTFlow*>(msg)) {
        transformTTFlowToCan(ttFlow);
        delete ttFlow;  // 已处理完毕
    } else {
        EV_ERROR << "收到非TTFlow消息: " << msg->getName() << endl;
        delete msg;
    }
}

// 处理来自CAN节点的反馈
void Transformation::handleCanFeedback(cMessage *msg) {
    if (auto canFrame = dynamic_cast<FiCo4OMNeT::CanDataFrame*>(msg)) {
        EV << "将CAN ID " << canFrame->getCanID() << " 封装回TSN流" << endl;

        // 创建TSN封装（示例需实现具体封装逻辑）
        IEEE8021QchTTFlow* tsnFrame = new IEEE8021QchTTFlow("TSN_Feedback");
        CircularRotationMessage* crMsg = new CircularRotationMessage();
        crMsg->encapsulate(canFrame->dup());
        tsnFrame->encapsulate(crMsg);

        // 通过extGate发回网络
        send(tsnFrame, "extGate$o");
    }
    delete msg;  // 原始消息已处理
}

// TSN到CAN的转换核心逻辑
void Transformation::transformTTFlowToCan(IEEE8021QchTTFlow* ttflow) {
    CircularRotationMessage* crMsg = dynamic_cast<CircularRotationMessage*>(ttflow->getEncapsulatedPacket());
    if (!crMsg) {
        EV_ERROR << "无效的TTFlow封装结构!" << endl;
        return;
    }

    // 解封装CAN帧（假设已有解封装方法）
    auto canFrames = crMsg->decapsulateCanDataFrameAndHoldTime();
    EV << "解封装出" << canFrames.size() << "个CAN帧" << endl;

    for (auto& pair : canFrames) {
            FiCo4OMNeT::CanDataFrame* canFrame = pair.first;
            double holdTime = pair.second;

            if (holdTime < 0) {
                EV_WARN << "忽略负保持时间: " << holdTime << "s (ID:" << canFrame->getCanID() << ")" << endl;
                delete canFrame;
                continue;
            }

            canfdNeedResendList.push_back(canFrame->dup()); // 存储拷贝
            delete canFrame; // 删除原始指针

            cMessage* timer = new cMessage(std::to_string(canFrame->getCanID()).c_str());
            scheduleAt(simTime() + holdTime, timer);
        }
}

} // namespace CAN4CQF

