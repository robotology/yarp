/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef YARP_ROBOTINTERFACE_TYPES_H
#define YARP_ROBOTINTERFACE_TYPES_H

#include <vector>
#include <iosfwd>
#include <list>
#include <string>

#include <yarp/robotinterface/api.h>


namespace yarp { namespace os { class Thread; } }
namespace yarp { namespace os { class LogStream; } }

namespace yarp {
namespace robotinterface {

class Param;
class Action;
class Device;
class Robot;

typedef std::vector<robotinterface::Param> ParamList;
typedef std::vector<robotinterface::Action> ActionList;
typedef std::vector<robotinterface::Device> DeviceList;
typedef std::list<yarp::os::Thread*> ThreadList;

bool hasParam(const robotinterface::ParamList &list, const std::string& name);
std::string findParam(const robotinterface::ParamList &list, const std::string& name);
bool hasGroup(const robotinterface::ParamList &list, const std::string& name);
std::string findGroup(const robotinterface::ParamList &list, const std::string& name);
robotinterface::ParamList mergeDuplicateGroups(const robotinterface::ParamList &list);

enum ActionPhase
{
    ActionPhaseUnknown = 0,
    ActionPhaseStartup,
    ActionPhaseRun,
    ActionPhaseInterrupt1,
    ActionPhaseInterrupt2,
    ActionPhaseInterrupt3,
    ActionPhaseShutdown,

    ActionPhaseReserved = 0xFF
};

robotinterface::ActionPhase YARP_robotinterface_API StringToActionPhase(const std::string &phase);
std::string YARP_robotinterface_API ActionPhaseToString(robotinterface::ActionPhase actionphase);
// Required by TiXmlElement::QueryValueAttribute<robotinterface::ActionPhase>
void YARP_robotinterface_API operator>>(const std::stringstream &sstream, robotinterface::ActionPhase &actionphase);


enum ActionType
{
    ActionTypeUnknown = 0,

    ActionTypeConfigure,
    ActionTypeCalibrate,
    ActionTypeAttach,
    ActionTypeAbort,
    ActionTypeDetach,
    ActionTypePark,

    ActionTypeCustom = 0xFF
};

robotinterface::ActionType YARP_robotinterface_API StringToActionType(const std::string &type);
std::string YARP_robotinterface_API ActionTypeToString(robotinterface::ActionType actiontype);
// Required by TiXmlElement::QueryValueAttribute<robotinterface::ActionType>
void YARP_robotinterface_API operator>>(const std::stringstream &sstream, robotinterface::ActionType &actiontype);

} // namespace robotinterface
} // namespace yarp

#endif // YARP_ROBOTINTERFACE_TYPES_H
