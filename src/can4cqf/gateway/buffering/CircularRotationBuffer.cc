#include "CircularRotationBuffer.h"
#include <fstream>
#include <iomanip> // for std::setw
#include "can4cqf/json/json.hpp" // 使用 JSON 库
using json = nlohmann::json; // 使用 nlohmann::json 简化命名using namespace FiCo4OMNeT;
using namespace std;
namespace CAN4CQF {

Define_Module(CircularRotationBuffer);

CircularRotationBuffer::CircularRotationBuffer(){
    const std::string filename = "crc_buffer_log.json";

    // Check if the file exists
    std::ifstream file(filename);
    if (file.good()) {
        file.close(); // Close the file before deleting
        std::remove(filename.c_str());
    }
}

CircularRotationBuffer::~CircularRotationBuffer(){
    cancelAndDelete(continuousFunctionMsg);
    // Delete any remaining CanDataFrame messages
    for (auto& canfdinfo : canfdinfo_vector) {
        delete canfdinfo.canfdmsg;
    }
    canfdinfo_vector.clear();

    // Also delete any messages in forwarding_send_msg
    for (auto& pair : forwarding_send_msg) {
        delete pair.first;
    }
    forwarding_send_msg.clear();
    string file_name = "crc_buffer_log.json";
    checkAndAppendBracket(file_name);
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

// 参数判断函数
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
        }else{
            // 需要筛选那些报文应该送入transformation模块中
            if(EncapsulateTSNFrame()){
                logToJsonFile();
                CircularRotationMessage* crcTSNframe = new CircularRotationMessage();
                crcTSNframe->setAllCanDataFrameAndHoldTime(this->forwarding_send_msg);
                send(crcTSNframe,gate("out"));
            }
            this->forwarding_send_msg.clear();
            scheduleAt(simTime() + this->timeout, msg);
        }
    }else if(CanDataFrame* dataFrame = dynamic_cast<CanDataFrame*>(msg)){
        // 将到达的CAN-FD数据
        getCanfdArrivingInfo(dataFrame);
        // Do not delete msg here, ownership transferred to canfdinfo_vector
    }else if (dynamic_cast<inet::EthernetIIFrame*>(msg)) {
        send(msg, gate("out"));
    } else if(dynamic_cast<IEEE8021QchTTFlow*>(msg)){
        send(msg,"out");
    } else if(dynamic_cast<IEEE8021QchBEFlow*>(msg)){
        send(msg,"out");
    } else{
        delete msg;
    }
}

// 检查时间间隔是否合适
bool CircularRotationBuffer::ChecktimeinTCQF(){
    double current_time = static_cast<double>(simTime().inUnit(SIMTIME_US));
    if((T_CQF_loopnum - 0.5) * timeout * 1e6 <= current_time && current_time < (T_CQF_loopnum + 1.5) * timeout * 1e6){
        return true;
    } else {
        return false;
    }
}

// 存储CAN-FD的到达信息
void CircularRotationBuffer::getCanfdArrivingInfo(CanDataFrame* msg){
    canfdInfo canfdinfo_new;
    canfdinfo_new.canfdmsg = msg; // Ownership of msg is now with canfdinfo_vector
    canfdinfo_new.time_arriving = simTime();
    // 在缓冲区中等待的时间，初次接收应该为0，之后每次经过T_CQF zeta就增加1，计算结果相当于 (zeta+1)*T_CQF
    canfdinfo_new.zeta = 0;
    canfdinfo_new.C_i = (msg->getBitLength()*this->tau_tarn+32*this->tau_arb) * 1e3;// 单位ms
    canfdinfo_new.D_i = this->lamda * canfdinfo_new.canfdmsg->getPeriod() * 1e3;// msg->getPeriod()返回的单位是s，需要转换为ms
    canfdinfo_new.P_i = msg->getCanID();
    this->canfdinfo_vector.push_back(canfdinfo_new);
    // 存储计算所需的数据，单位为ms
    if(wcrt_data_map.find(canfdinfo_new.P_i) == wcrt_data_map.end()){
        wcrt_data_map[canfdinfo_new.P_i] = {canfdinfo_new.canfdmsg->getPeriod()*1e3, canfdinfo_new.C_i};
    }
    double Bi = canfdinfo_new.C_i;
    // b_i计算存在问题
    for(auto wcrt_data:this->wcrt_data_map){
        Bi = max(Bi, wcrt_data.second.c_i);
    }
    // 使用ms计算WCRT
    canfdinfo_new.wcrt = calculateWCRT(canfdinfo_new.C_i, canfdinfo_new.P_i, canfdinfo_new.D_i, Bi);
    printf("canfd_id:%d, canfd_period:%f, canfd_wcrt:%f\n",canfdinfo_new.P_i,msg->getPeriod(),canfdinfo_new.wcrt);

    // Update the canfdInfo in the vector (since canfdinfo_new is local)
    this->canfdinfo_vector.back().wcrt = canfdinfo_new.wcrt;
}

double CircularRotationBuffer::calculateWCRT(double c_i,int p_i, double d_i, double b_i) {
    /*
     * @brief 使用latex的wcrt计算公式：
     * w_{i}^{n+1}=\displaystyle B_i+
        \sum\limits_{m_{j}\in hp(m_{i})}\left\lceil
        \frac{w_{i}^{n}+\tau_{tran}}{T_{j}}\right\rceil C_{j}
        @param:
            c_i: Execution time of message i
            p_i：Priority of message i (CAN-FD ID)
            d_i：Deadline of message i
            b_i：Blocking time
        return:计算后的wcrt
     * */
    int iteration = 0;
    int maxIterations = 200;
    double w_i = c_i;
    double epsilon = 1e-6;
    while(iteration < maxIterations){
        double sum = 0.0;
        for(const auto& entry: wcrt_data_map){
            int msgId = entry.first;
            const wcrt_data_info& msgData = entry.second;
            if(msgId < p_i){
                double temp = (w_i + this->tau_tarn) / msgData.period;
                double contribution = std::ceil(temp) * msgData.c_i;
                sum += contribution;
            }
        }
        double w_i_n = b_i + sum;
        // 检查停止条件
        if (w_i_n >= d_i || std::fabs(w_i_n - w_i) < epsilon) {
            w_i = w_i_n;
            break;
        }
        w_i = w_i_n;
        iteration++;
    }
    return w_i;
}

bool CircularRotationBuffer::EncapsulateTSNFrame(){
    /*
        @param: 已有参数,this->canfdinfo_vector，存储了所有到达的CAN-FD数据
        @target: 封装成TSN帧，并发送出去
        @return: true->有报文需要发送
                    false->无报文发送
    */
    caluculateEndToEndDelay();
    vector<pair<CanDataFrame*,double>> canfdinfo_vector_sort;
    for (auto it = this->canfdinfo_vector.begin(); it != this->canfdinfo_vector.end(); ) {
        if(it->canfdmsg->getCanID() == 153){
            EV<<"153"<<endl;
        }
        if (it->remaining_Deadline >= 0 && it->remaining_Deadline <= this->timeout * 1e3) {
            // Message is within acceptable remaining deadline,and use T_CQF-remaining_Deadline as the final time
            canfdinfo_vector_sort.push_back(make_pair(it->canfdmsg, this->timeout - it->remaining_Deadline *1e-3));
            it = this->canfdinfo_vector.erase(it);  // Remove from vector
        } else if (it->remaining_Deadline < 0) {
            // Message has missed its deadline, discard it
            EV_WARN << "Message with ID " << it->P_i << " has negative remaining_Deadline (" << it->remaining_Deadline << " ms), discarding.\n";
            delete it->canfdmsg;  // Delete the message to prevent memory leaks
            it = this->canfdinfo_vector.erase(it);  // Remove from vector
        } else {
            // Message needs to wait for the next cycle
            it->zeta++;
            ++it;
        }
    }
    // 因为我选择封装的是T_CQF-remaining_Deadline，因此需要选择元素中较大的作为头部
    sort(canfdinfo_vector_sort.begin(), canfdinfo_vector_sort.end() , [](const pair<CanDataFrame*,double>& a, const pair<CanDataFrame*,double>& b){
        return a.second > b.second;
    });
    for(const auto& pair_canfdinfo: canfdinfo_vector_sort){
        this->forwarding_send_msg[pair_canfdinfo.first]=pair_canfdinfo.second;
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
        // 写入数组起始的方括号
        if (first_entry) {
            log_file << "[";  // 写入数组起始的 [
            first_entry = false;
        } else {
            log_file << ",";  // 在每个条目之间写入逗号
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
        for (const auto& msg_id : forwarding_send_msg) {
            json forwardingMsgJson;
            forwardingMsgJson["id"] = msg_id.first->getCanID();
            forwardingMsgJson["waiting_tiime"] = msg_id.second;
            log_entry["forwarding_send_msg"].push_back(forwardingMsgJson);
        }

        log_file << log_entry.dump(4);  // 写入格式化的 JSON
        log_file.close();
    } else {
        EV_ERROR << "Failed to open log file for writing." << std::endl;
    }
}

// 在程序结束时，添加一个关闭方括号
void CircularRotationBuffer::checkAndAppendBracket(const std::string& filename) {
    std::ifstream file(filename, std::ios::in | std::ios::ate);
    if (!file.is_open()) {
        return;
    }

    // Move to the end of the file and check the last character
    file.seekg(-1, std::ios::end);
    char lastChar;
    file.get(lastChar);
    file.close();

    // Check if the last character is ']'
    if (lastChar != ']') {
        // Append ']' if it's missing
        std::ofstream outFile(filename, std::ios::app);
        if (outFile.is_open()) {
            outFile << "]";
            outFile.close();
        }
    }
}


} //namespace
