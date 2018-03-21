/*
 * Copyright (C) 2012 Istituto Italiano di Tecnologia (IIT)
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
 */

#ifndef YARP_YARPROBOTINTERFACE_TYPES_H
#define YARP_YARPROBOTINTERFACE_TYPES_H

#include <vector>
#include <iosfwd>
#include <list>

namespace yarp { namespace os { class Thread; } }
namespace yarp { namespace os { class LogStream; } }

namespace RobotInterface
{
class Param;
class Action;
class Device;
class Robot;

typedef std::vector<RobotInterface::Param> ParamList;
typedef std::vector<RobotInterface::Action> ActionList;
typedef std::vector<RobotInterface::Device> DeviceList;
typedef std::list<yarp::os::Thread*> ThreadList;

bool hasParam(const RobotInterface::ParamList &list, const std::string& name);
std::string findParam(const RobotInterface::ParamList &list, const std::string& name);
bool hasGroup(const RobotInterface::ParamList &list, const std::string& name);
std::string findGroup(const RobotInterface::ParamList &list, const std::string& name);
RobotInterface::ParamList mergeDuplicateGroups(const RobotInterface::ParamList &list);

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

RobotInterface::ActionPhase StringToActionPhase(const std::string &phase);
std::string ActionPhaseToString(RobotInterface::ActionPhase actionphase);
// Required by TiXmlElement::QueryValueAttribute<RobotInterface::ActionPhase>
void operator>>(const std::stringstream &sstream, RobotInterface::ActionPhase &actionphase);


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

RobotInterface::ActionType StringToActionType(const std::string &type);
std::string ActionTypeToString(RobotInterface::ActionType actiontype);
// Required by TiXmlElement::QueryValueAttribute<RobotInterface::ActionType>
void operator>>(const std::stringstream &sstream, RobotInterface::ActionType &actiontype);

}

#endif // YARP_YARPROBOTINTERFACE_TYPES_H
