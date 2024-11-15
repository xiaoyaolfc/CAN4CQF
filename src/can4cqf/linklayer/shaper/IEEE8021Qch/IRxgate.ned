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

package can4cqf.can4cqf.linklayer.shaper.IEEE8021Qch;

import core4inet.utilities.classes.IScheduled;
// IRxgate是Rxgate的接口模块
moduleinterface IRxgate extends IScheduled
{
    parameters:
        @display("i=block/star");
        // 定义门控状态，原来是string类型
        int state;
//		string state;

	gates:    
	    // 从queueing中接收数据
        input rxin[2] @labels(EtherFrame);
        // 根据门控状态确定需要转发的数据     
        output rxout[2] @labels(EtherFrame);
}