#include "CircularRotationBuffer.h"
#include <fstream>
#include <iomanip> // for std::setw
#include <cstdio>      // for std::remove
#include "can4cqf/json/json.hpp" // 使用 JSON 库
using json = nlohmann::json;
using namespace FiCo4OMNeT;
using namespace std;

namespace CAN4CQF {

Define_Module(CircularRotationBuffer);

CircularRotationBuffer::CircularRotationBuffer(){
    const std::string filename = "crc_buffer_log.json";
    // 检查日志文件是否存在，存在则删除
    std::ifstream file(filename);
    if (file.good()) {
        file.close(); // Close the file before deleting
        std::remove(filename.c_str());
    }
}

CircularRotationBuffer::~CircularRotationBuffer(){
    cancelAndDelete(continuousFunctionMsg);

    // 删除 canfdinfo_vector 中仍由本模块拥有的 CanDataFrame 对象
    for (auto& info : canfdinfo_vector) {
        if (info.canfdmsg && info.canfdmsg->getOwner() == this)
            delete info.canfdmsg;
    }
    canfdinfo_vector.clear();

    // 删除 forwarding_send_msg 中仍由本模块拥有的 CanDataFrame 对象
    for (auto& entry : forwarding_send_msg) {
        if (entry.first && entry.first->getOwner() == this)
            delete entry.first;
    }
    forwarding_send_msg.clear();
    string file_name = "crc_buffer_log.json";
    checkAndAppendBracket(file_name);
}

void CircularRotationBuffer::finish() {

    // 删除 canfdinfo_vector 中尚未发送且仍由本模块拥有的消息
    for (auto& info : canfdinfo_vector) {
        if (info.canfdmsg && info.canfdmsg->getOwner() == this)
            delete info.canfdmsg;
    }
    canfdinfo_vector.clear();

    // 删除 forwarding_send_msg 中的消息
    for (auto& entry : forwarding_send_msg) {
        if (entry.first && entry.first->getOwner() == this)
            delete entry.first;
    }
    forwarding_send_msg.clear();

    // 确保 JSON 日志文件闭合
    checkAndAppendBracket("crc_buffer_log.json");
}

void CircularRotationBuffer::initialize()
{
    this->timeout = par("timeout").doubleValue();
    this->tau_arb = par("tau_arb").doubleValue();
    this->tau_tarn = par("tau_tarn").doubleValue();
    this->lamda = par("lamda").doubleValue();
    handleParameterChange(nullptr);
    continuousFunctionMsg = new cMessage("ContinuousFunction");
    scheduleAt(simTime() + timeout, continuousFunctionMsg);
}

void CircularRotationBuffer::handleParameterChange(const char *parname){
    if (!parname || !strcmp(parname, "gatewayID"))
    {
        this->gatewayID = par("gatewayID").stringValue();
        if(this->gatewayID.empty() || !strcmp(this->gatewayID.c_str(), "auto")){
            //auto create id!
            this->gatewayID = this->getParentModule()->getParentModule()->getName();
        }
    }
}

void CircularRotationBuffer::handleMessage(cMessage *msg)
{
    if(msg->isSelfMessage()){
        // 将当前T_CQF循环次数加一
        this->T_CQF_loopnum++;
        if(!ChecktimeinTCQF()){
            scheduleAt(simTime() + this->timeout, msg);
        } else {
            // 需要筛选那些报文应该送入transformation模块中
            if(EncapsulateTSNFrame()){
                logToJsonFile();
                CircularRotationMessage* crcTSNframe = new CircularRotationMessage();
                crcTSNframe->setAllCanDataFrameAndHoldTime(this->forwarding_send_msg);
                send(crcTSNframe, gate("out"));
            }
            this->forwarding_send_msg.clear();
            scheduleAt(simTime() + this->timeout, msg);
        }
    } else if(CanDataFrame* dataFrame = dynamic_cast<CanDataFrame*>(msg)){
        // 将到达的CAN-FD数据
        getCanfdArrivingInfo(dataFrame);
        // Do not delete msg here, ownership transferred to canfdinfo_vector
    } else if (dynamic_cast<inet::EthernetIIFrame*>(msg)) {
        send(msg, gate("out"));
    } else if(dynamic_cast<IEEE8021QchTTFlow*>(msg)){
        EV << "Received IEEE8021QchTTFlow message, sending out...\n";
        send(msg, gate("out"));
        bubble("IEEE8021QchTTFlow message sent!");
    } else if(dynamic_cast<IEEE8021QchBEFlow*>(msg)){
        send(msg, gate("out"));
    } else {
        delete msg;
    }
}

bool CircularRotationBuffer::ChecktimeinTCQF(){
    double current_time = static_cast<double>(simTime().inUnit(SIMTIME_US));
    if((T_CQF_loopnum - 0.5) * timeout * 1e6 <= current_time &&
       current_time < (T_CQF_loopnum + 1.5) * timeout * 1e6){
        return true;
    } else {
        return false;
    }
}

void CircularRotationBuffer::getCanfdArrivingInfo(CanDataFrame* msg)
{
    canfdInfo canfdinfo_new;
    canfdinfo_new.canfdmsg = msg; // 将 msg 的所有权转移给 canfdinfo_vector
    canfdinfo_new.time_arriving = simTime();
    // 初始等待次数为 0，每经过一个 T_CQF 循环，zeta 自增
    canfdinfo_new.zeta = 0;
    // 计算执行时间 C_i（单位：ms）
    canfdinfo_new.C_i = (msg->getBitLength() * this->tau_tarn + 32 * this->tau_arb) * 1e3;
    // 修改：将 Deadline 加上一个偏置（这里加上 timeout），保证 D_i 足够大（单位：ms）
    canfdinfo_new.D_i = (this->lamda * msg->getPeriod() + this->timeout) * 1e3;
    canfdinfo_new.P_i = msg->getCanID();
    this->canfdinfo_vector.push_back(canfdinfo_new);

    // 存储计算所需的数据，单位：ms
    if(wcrt_data_map.find(canfdinfo_new.P_i) == wcrt_data_map.end()){
        wcrt_data_map[canfdinfo_new.P_i] = {msg->getPeriod() * 1e3, canfdinfo_new.C_i};
    }
    double Bi = canfdinfo_new.C_i;
    // 取所有相关报文的最大 C_i 作为阻塞时间 B_i
    for(auto wcrt_data : this->wcrt_data_map){
        Bi = max(Bi, wcrt_data.second.c_i);
    }
    // 使用 ms 计算 WCRT，即端到端排队时延 R_i = C_i + interference
    double computedR = calculateWCRT(canfdinfo_new.C_i, canfdinfo_new.P_i, canfdinfo_new.D_i, Bi);
    // 输出调试信息
    printf("canfd_id:%d, canfd_period:%f, computed wcrt:%f\n", canfdinfo_new.P_i, msg->getPeriod(), computedR);

    // 更新当前报文的 wcrt 值
    this->canfdinfo_vector.back().wcrt = computedR;
}

double CircularRotationBuffer::calculateWCRT(double c_i, int p_i, double d_i, double b_i)
{
    int iteration = 0;
    const int maxIterations = 200;
    double w_i = 0.0; // 初始化干扰部分为 0
    const double epsilon = 1e-6;

    // 迭代求解干扰部分（响应时间 R_i = C_i + B_i + interference）
    while (iteration < maxIterations) {
        double sum = 0.0;
        for (const auto& entry : wcrt_data_map) {
            int msgId = entry.first;
            const wcrt_data_info& msgData = entry.second;
            if (msgId < p_i) { // 仅考虑高优先级任务
                double temp = (w_i + this->tau_tarn) / msgData.period;
                double contribution = std::ceil(temp) * msgData.c_i;
                sum += contribution;
            }
        }
        double w_i_next = b_i + sum; // 干扰部分（B_i + 干扰）
        if (fabs(w_i_next - w_i) < epsilon) {
            w_i = w_i_next;
            break;
        }
        w_i = w_i_next;
        iteration++;
    }
    // 最终响应时间 = C_i + 干扰部分
    double R_i = c_i + w_i;
    // 如果计算得到的响应时间超过 Deadline，则饱和为 Deadline（防止超出）
    if (R_i > d_i)
        R_i = d_i;
    return R_i;
}


bool CircularRotationBuffer::EncapsulateTSNFrame(){
    /*
        封装缓冲区中所有到达的CAN-FD数据到TSN帧中
        返回true表示有报文需要发送
    */
    caluculateEndToEndDelay();
    vector<pair<CanDataFrame*, double>> canfdinfo_vector_sort;
    for (auto it = this->canfdinfo_vector.begin(); it != this->canfdinfo_vector.end(); ) {
        if (it->remaining_Deadline >= 0 && it->remaining_Deadline <= this->timeout * 1e3) {
            // 使用 T_CQF - remaining_Deadline (转换为秒) 作为等待时间
            double waiting_time = this->timeout * 1e3 - it->remaining_Deadline ; // 单位ms
            canfdinfo_vector_sort.push_back(make_pair(it->canfdmsg, waiting_time));
            it = this->canfdinfo_vector.erase(it);  // 从向量中删除该元素
        } else if (it->remaining_Deadline < 0) {
            EV_WARN << "Message with ID " << it->P_i << " has negative remaining_Deadline ("
                    << it->remaining_Deadline << " ms), discarding.\n";
            delete it->canfdmsg;  // 删除消息以防内存泄漏
            it = this->canfdinfo_vector.erase(it);
        } else {
            // 报文等待下一周期，zeta 自增
            it->zeta++;
            ++it;
        }
    }
    // 根据等待时间降序排序（等待时间越大，优先级越高）
    sort(canfdinfo_vector_sort.begin(), canfdinfo_vector_sort.end(),
         [](const pair<CanDataFrame*, double>& a, const pair<CanDataFrame*, double>& b) {
             return a.second > b.second;
         });
    for(const auto& pair_canfdinfo : canfdinfo_vector_sort){
        this->forwarding_send_msg[pair_canfdinfo.first] = pair_canfdinfo.second;
    }
    return !this->forwarding_send_msg.empty();
}

void CircularRotationBuffer::caluculateEndToEndDelay(){
    for(auto& canfdinfo_vec : this->canfdinfo_vector){
        canfdinfo_vec.remaining_Deadline = canfdinfo_vec.D_i - canfdinfo_vec.wcrt - (canfdinfo_vec.zeta + 3) * this->timeout * 1e3;
    }
}

void CircularRotationBuffer::logToJsonFile() {
    static bool first_entry = true;
    std::ofstream log_file("crc_buffer_log.json", std::ios::app);
    if (log_file.is_open()) {
        if (first_entry) {
            log_file << "[";
            first_entry = false;
        } else {
            log_file << ",";
        }
        json log_entry;
        log_entry["current_time"] = simTime().dbl();
        log_entry["canfdinfo_vector"] = json::array();
        for (const auto& canfdinfo : canfdinfo_vector) {
            json canfdinfoJson;
            canfdinfoJson["canfd_id"] = canfdinfo.canfdmsg->getCanID();
            canfdinfoJson["remaining_time"] = canfdinfo.remaining_Deadline;
            log_entry["canfdinfo_vector"].push_back(canfdinfoJson);
        }
        log_entry["forwarding_send_msg"] = json::array();
        for (const auto& entry : forwarding_send_msg) {
            json forwardingMsgJson;
            forwardingMsgJson["id"] = entry.first->getCanID();
            forwardingMsgJson["waiting_time"] = entry.second;
            log_entry["forwarding_send_msg"].push_back(forwardingMsgJson);
        }
        log_file << log_entry.dump(4);
        log_file.close();
    } else {
        EV_ERROR << "Failed to open log file for writing." << std::endl;
    }
}

void CircularRotationBuffer::checkAndAppendBracket(const std::string& filename) {
    std::ifstream file(filename, std::ios::in | std::ios::ate);
    if (!file.is_open()) {
        return;
    }
    file.seekg(-1, std::ios::end);
    char lastChar;
    file.get(lastChar);
    file.close();
    if (lastChar != ']') {
        std::ofstream outFile(filename, std::ios::app);
        if (outFile.is_open()) {
            outFile << "]";
            outFile.close();
        }
    }
}

} // namespace CAN4CQF
