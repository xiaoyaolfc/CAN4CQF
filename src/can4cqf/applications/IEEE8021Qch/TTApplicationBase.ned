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

package can4cqf.can4cqf.applications.IEEE8021Qch;

import core4inet.applications.base.ApplicationBase;
//
// 用来处理接受的TT流，将其转发给物理层模块
//
simple TTApplicationBase extends ApplicationBase
{
    parameters:
        @class(AbstractSimpleModule);
        @display("i=block/app");
        
    gates:
        //Input gate for the incoming time-triggered messages
        input TTin @directIn @labels(IEEE8021QchTTFlow);
        //Input gate for the incoming rate-constrained messages
        input BEin @directIn @labels(IEEE8021QchBEFlow);    
}
