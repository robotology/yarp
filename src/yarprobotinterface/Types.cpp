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

#include "Types.h"
#include "Param.h"

#include <string>
#include <sstream>


#include <yarp/os/LogStream.h>


bool RobotInterface::hasParam(const RobotInterface::ParamList &list, const std::string& name)
{
    for (const auto& param : list) {
        if (name == param.name()) {
            return true;
        }
    }
    return false;
}

std::string RobotInterface::findParam(const RobotInterface::ParamList &list, const std::string& name)
{
    for (const auto& param : list) {
        if (name == param.name()) {
            return param.value();
        }
    }
    yError() << "Param" << name << "not found";
    return {};
}

bool RobotInterface::hasGroup(const RobotInterface::ParamList &list, const std::string& name)
{
    for (const auto& param : list) {
        if (param.isGroup() && name == param.name()) {
            return true;
        }
    }
    return false;
}

std::string RobotInterface::findGroup(const RobotInterface::ParamList &list, const std::string& name)
{
    for (const auto& param : list) {
        if (param.isGroup() && name == param.name()) {
            return param.value();
        }
    }
    yError() << "Param" << name << "not found";
    return {};
}

RobotInterface::ParamList RobotInterface::mergeDuplicateGroups(const RobotInterface::ParamList &list)
{
    RobotInterface::ParamList params = list;
    for (auto it1 = params.begin(); it1 != params.end(); ++it1) {
        RobotInterface::Param &param1 = *it1;
        for (auto it2 = it1 + 1; it2 != params.end(); ) {
            RobotInterface::Param &param2 = *it2;
            if (param1.name() == param2.name()) {
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
    if (phase == "startup") {
        return RobotInterface::ActionPhaseStartup;
    } else if (phase == "run") {
        return RobotInterface::ActionPhaseRun;
    } else if (phase == "interrupt1") {
        return RobotInterface::ActionPhaseInterrupt1;
    } else if (phase == "interrupt2") {
        return RobotInterface::ActionPhaseInterrupt2;
    } else if (phase == "interrupt3") {
        return RobotInterface::ActionPhaseInterrupt3;
    } else if (phase == "shutdown") {
        return RobotInterface::ActionPhaseShutdown;
    }
    return RobotInterface::ActionPhaseReserved;
}

std::string RobotInterface::ActionPhaseToString(RobotInterface::ActionPhase actionphase)
{
    switch (actionphase) {
    case RobotInterface::ActionPhaseStartup:
        return std::string("startup");
    case RobotInterface::ActionPhaseRun:
        return std::string("run");
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
        return {};
    }
}

void RobotInterface::operator>>(const std::stringstream &sstream, RobotInterface::ActionPhase &actionphase)
{
    actionphase = RobotInterface::StringToActionPhase(sstream.str());
}


RobotInterface::ActionType RobotInterface::StringToActionType(const std::string &type)
{
    if (type == "configure") {
        return RobotInterface::ActionTypeConfigure;
    } else if (type == "calibrate") {
        return RobotInterface::ActionTypeCalibrate;
    } else if (type == "attach") {
        return RobotInterface::ActionTypeAttach;
    } else if (type == "abort") {
        return RobotInterface::ActionTypeAbort;
    } else if (type == "detach") {
        return RobotInterface::ActionTypeDetach;
    } else if (type == "park") {
        return RobotInterface::ActionTypePark;
    } else if (type == "custom") {
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
        return {};
    }
}

void RobotInterface::operator>>(const std::stringstream &sstream, RobotInterface::ActionType &actiontype)
{
    actiontype = RobotInterface::StringToActionType(sstream.str());
}
