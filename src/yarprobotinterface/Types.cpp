/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include "Types.h"
#include "Param.h"

#include <string>
#include <sstream>


#include <yarp/os/LogStream.h>


bool RobotInterface::hasParam(const RobotInterface::ParamList &list, const std::string& name)
{
    for (RobotInterface::ParamList::const_iterator it = list.begin(); it != list.end(); ++it) {
        const RobotInterface::Param &param = *it;
        if (!name.compare(param.name())) {
            return true;
        }
    }
    return false;
}

std::string RobotInterface::findParam(const RobotInterface::ParamList &list, const std::string& name)
{
    for (RobotInterface::ParamList::const_iterator it = list.begin(); it != list.end(); ++it) {
        const RobotInterface::Param &param = *it;
        if (!name.compare(param.name())) {
            return param.value();
        }
    }
    yError() << "Param" << name << "not found";
    return std::string();
}

bool RobotInterface::hasGroup(const RobotInterface::ParamList &list, const std::string& name)
{
    for (RobotInterface::ParamList::const_iterator it = list.begin(); it != list.end(); ++it) {
        const RobotInterface::Param &param = *it;
        if (param.isGroup() && !name.compare(param.name())) {
            return true;
        }
    }
    return false;
}

std::string RobotInterface::findGroup(const RobotInterface::ParamList &list, const std::string& name)
{
    for (RobotInterface::ParamList::const_iterator it = list.begin(); it != list.end(); ++it) {
        const RobotInterface::Param &param = *it;
        if (param.isGroup() && !name.compare(param.name())) {
            return param.value();
        }
    }
    yError() << "Param" << name << "not found";
    return std::string();
}

RobotInterface::ParamList RobotInterface::mergeDuplicateGroups(const RobotInterface::ParamList &list)
{
    RobotInterface::ParamList params = list;
    for (RobotInterface::ParamList::iterator it1 = params.begin(); it1 != params.end(); ++it1) {
        RobotInterface::Param &param1 = *it1;
        for (RobotInterface::ParamList::iterator it2 = it1 + 1; it2 != params.end(); ) {
            RobotInterface::Param &param2 = *it2;
            if (param1.name().compare(param2.name()) == 0) {
                if (!param1.isGroup() || !param2.isGroup()) {
                    yFatal() << "Duplicate parameter \"" << param1.name() << "\" found and at least one of them is not a group.";
                }
                param1.value() += std::string(" ");
                param1.value() += param2.value();
                it2 = params.erase(it2);
            }
            else it2++;
        }
    }
    return params;
}


RobotInterface::ActionPhase RobotInterface::StringToActionPhase(const std::string &phase)
{
    if (!phase.compare("startup")) {
        return RobotInterface::ActionPhaseStartup;
    } else if (!phase.compare("interrupt1")) {
        return RobotInterface::ActionPhaseInterrupt1;
    } else if (!phase.compare("interrupt2")) {
        return RobotInterface::ActionPhaseInterrupt2;
    } else if (!phase.compare("interrupt3")) {
        return RobotInterface::ActionPhaseInterrupt3;
    } else if (!phase.compare("shutdown")) {
        return RobotInterface::ActionPhaseShutdown;
    }
    return RobotInterface::ActionPhaseReserved;
}

std::string RobotInterface::ActionPhaseToString(RobotInterface::ActionPhase actionphase)
{
    switch (actionphase) {
    case RobotInterface::ActionPhaseStartup:
        return std::string("startup");
    case RobotInterface::ActionPhaseInterrupt1:
        return std::string("interrupt1");
    case RobotInterface::ActionPhaseInterrupt2:
        return std::string("interrupt2");
    case RobotInterface::ActionPhaseInterrupt3:
        return std::string("interrupt3");
    case RobotInterface::ActionPhaseShutdown:
        return std::string("shutdown");
    case RobotInterface::ActionPhaseUnknown:
    default:
        return std::string();
    }
}

void RobotInterface::operator>>(const std::stringstream &sstream, RobotInterface::ActionPhase &actionphase)
{
    actionphase = RobotInterface::StringToActionPhase(sstream.str());
}


RobotInterface::ActionType RobotInterface::StringToActionType(const std::string &type)
{
    if (!type.compare("configure")) {
        return RobotInterface::ActionTypeConfigure;
    } else if (!type.compare("calibrate")) {
        return RobotInterface::ActionTypeCalibrate;
    } else if (!type.compare("attach")) {
        return RobotInterface::ActionTypeAttach;
    } else if (!type.compare("abort")) {
        return RobotInterface::ActionTypeAbort;
    } else if (!type.compare("detach")) {
        return RobotInterface::ActionTypeDetach;
    } else if (!type.compare("park")) {
        return RobotInterface::ActionTypePark;
    } else if (!type.compare("custom")) {
        return RobotInterface::ActionTypeCustom;
    } else {
        return RobotInterface::ActionTypeUnknown;
    }
}

std::string RobotInterface::ActionTypeToString(RobotInterface::ActionType actiontype)
{
    switch (actiontype) {
    case RobotInterface::ActionTypeConfigure:
        return std::string("configure");
    case RobotInterface::ActionTypeCalibrate:
        return std::string("calibrate");
    case RobotInterface::ActionTypeAttach:
        return std::string("attach");
    case RobotInterface::ActionTypeAbort:
        return std::string("abort");
    case RobotInterface::ActionTypeDetach:
        return std::string("detach");
    case RobotInterface::ActionTypePark:
        return std::string("park");
    case RobotInterface::ActionTypeCustom:
        return std::string("custom");
    case RobotInterface::ActionTypeUnknown:
    default:
        return std::string();
    }
}

void RobotInterface::operator>>(const std::stringstream &sstream, RobotInterface::ActionType &actiontype)
{
    actiontype = RobotInterface::StringToActionType(sstream.str());
}
