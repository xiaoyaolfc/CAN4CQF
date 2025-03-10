# 用于CAN-TO-TSN端到端确定性传输实现的代码
## 使用方法
1. 下载Download OMNeT++ 6.0.2    * [https://omnetpp.org/download/](https://omnetpp.org/download/)
2. 安装OMNeT++    * [https://doc.omnetpp.org/omnetpp/InstallGuide.pdf](https://doc.omnetpp.org/omnetpp/InstallGuide.pdf)
3. 安装INET framework 3.8.3    * [https://inet.omnetpp.org/Download.html](https://inet.omnetpp.org/Download.html)，并禁用visualization和依赖visualization的所有功能（PS：properties->OMNeT++->Project Feature中禁用），
4. 安装CoRE4INET*[https://github.com/CoRE-RG/CoRE4INET](https://github.com/CoRE-RG/CoRE4INET)、FiCo4OMNeT *[https://github.com/CoRE-RG/FiCo4OMNeT](https://github.com/CoRE-RG/FiCo4OMNeT)、SignalsAndGateways *[https://github.com/CoRE-RG/SignalsAndGateways](https://github.com/CoRE-RG/SignalsAndGateways)
5. 按照CoRE的readme，正确安装并编译（PS：OMNeT++6.0.2可能会抽风，可以看一下preprocessing.txt中出现的解决方案）
6. 运行/simulations/large_newtork中的omnetpp.ini，就可以正确的玩耍啦！
