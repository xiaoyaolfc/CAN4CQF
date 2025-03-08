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

#include "TTApplicationBase.h"
//CoRE4INET
#include "core4inet/base/CoRE4INET_Defs.h"
//#include "core4inet/buffer/AS6802/RCBuffer.h"
#include "core4inet/utilities/customWatch.h"
#include "core4inet/utilities/ConfigFunctions.h"

//INET
#include "inet/common/ModuleAccess.h"

namespace CAN4CQF {

Define_Module(TTApplicationBase);
TTApplicationBase::TTApplicationBase(){

}
TTApplicationBase::~TTApplicationBase(){
    buffer.clear();
}

void TTApplicationBase::initialize()
{
    WATCH_LISTUMAP(buffer);
    CoRE4INET::ApplicationBase::initialize();
}

void TTApplicationBase::handleMessage(cMessage *msg)
{
    ApplicationBase::handleMessage(msg);
}
void TTApplicationBase::handleParameterChange(const char* parname){
    ApplicationBase::handleParameterChange(parname);
    if (!parname || !strcmp(parname, "buffers"))
    {
        buffer.clear();
        std::vector<cModule*> modules = parameterToModuleList(par("buffers"), DELIMITERS);
        for (std::vector<cModule*>::const_iterator module = modules.begin(); module != modules.end(); ++module)
        {
            if (inet::findContainingNode(*module) != inet::findContainingNode(this))
            {
                throw cRuntimeError(
                        "Configuration problem of parameter buffers in module %s: Module: %s is not in node %s! Maybe a copy-paste problem?",
                        this->getFullName(), (*module)->getFullName(), inet::findContainingNode(this)->getFullName());
            }
            if (Buffer *buffers = dynamic_cast<Buffer*>(*module))
            {
                buffer[buffers->getId()].push_back(buffers);
            }
        }
    }
}
} //namespace
