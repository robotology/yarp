/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_ROBOTINTERFACE_TYPES_H
#define YARP_ROBOTINTERFACE_TYPES_H

#include <yarp/robotinterface/api.h>

#include <iosfwd>
#include <list>
#include <string>
#include <vector>

namespace yarp {

namespace os {
class Thread;
class LogStream;
} // namespace os

namespace robotinterface {
namespace experimental {

class Param;
class Action;
class Device;
class Robot;

typedef std::vector<robotinterface::experimental::Param> ParamList;
typedef std::vector<robotinterface::experimental::Action> ActionList;
typedef std::vector<robotinterface::experimental::Device> DeviceList;
typedef std::list<yarp::os::Thread*> ThreadList;

YARP_robotinterface_API bool hasParam(const robotinterface::experimental::ParamList& list, const std::string& name);
YARP_robotinterface_API std::string findParam(const robotinterface::experimental::ParamList& list, const std::string& name);
YARP_robotinterface_API bool hasGroup(const robotinterface::experimental::ParamList& list, const std::string& name);
YARP_robotinterface_API std::string findGroup(const robotinterface::experimental::ParamList& list, const std::string& name);
YARP_robotinterface_API robotinterface::experimental::ParamList mergeDuplicateGroups(const robotinterface::experimental::ParamList& list);

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

YARP_robotinterface_API robotinterface::experimental::ActionPhase StringToActionPhase(const std::string& phase);
YARP_robotinterface_API std::string ActionPhaseToString(robotinterface::experimental::ActionPhase actionphase);
// Required by TiXmlElement::QueryValueAttribute<robotinterface::ActionPhase>
YARP_robotinterface_API void operator>>(const std::stringstream& sstream, robotinterface::experimental::ActionPhase& actionphase);


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

YARP_robotinterface_API robotinterface::experimental::ActionType StringToActionType(const std::string& type);
YARP_robotinterface_API std::string ActionTypeToString(robotinterface::experimental::ActionType actiontype);
// Required by TiXmlElement::QueryValueAttribute<robotinterface::ActionType>
YARP_robotinterface_API void operator>>(const std::stringstream& sstream, robotinterface::experimental::ActionType& actiontype);

} // namespace experimental
} // namespace robotinterface
} // namespace yarp

#endif // YARP_ROBOTINTERFACE_TYPES_H
