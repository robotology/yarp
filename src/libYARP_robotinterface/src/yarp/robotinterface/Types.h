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

YARP_robotinterface_API bool hasParam(const robotinterface::ParamList &list, const std::string& name);
YARP_robotinterface_API std::string findParam(const robotinterface::ParamList &list, const std::string& name);
YARP_robotinterface_API bool hasGroup(const robotinterface::ParamList &list, const std::string& name);
YARP_robotinterface_API std::string findGroup(const robotinterface::ParamList &list, const std::string& name);
YARP_robotinterface_API robotinterface::ParamList mergeDuplicateGroups(const robotinterface::ParamList &list);

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

YARP_robotinterface_API robotinterface::ActionPhase StringToActionPhase(const std::string &phase);
YARP_robotinterface_API std::string ActionPhaseToString(robotinterface::ActionPhase actionphase);
// Required by TiXmlElement::QueryValueAttribute<robotinterface::ActionPhase>
YARP_robotinterface_API void operator>>(const std::stringstream &sstream, robotinterface::ActionPhase &actionphase);


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

YARP_robotinterface_API robotinterface::ActionType StringToActionType(const std::string &type);
YARP_robotinterface_API std::string ActionTypeToString(robotinterface::ActionType actiontype);
// Required by TiXmlElement::QueryValueAttribute<robotinterface::ActionType>
YARP_robotinterface_API void operator>>(const std::stringstream &sstream, robotinterface::ActionType &actiontype);

} // namespace robotinterface
} // namespace yarp

#endif // YARP_ROBOTINTERFACE_TYPES_H
