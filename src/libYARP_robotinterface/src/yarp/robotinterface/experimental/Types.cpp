/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/robotinterface/experimental/Types.h>

#include <yarp/robotinterface/experimental/Param.h>

#include <yarp/os/LogStream.h>

#include <sstream>
#include <string>


bool yarp::robotinterface::experimental::hasParam(const yarp::robotinterface::experimental::ParamList& list, const std::string& name)
{
    for (const auto& param : list) {
        if (name == param.name()) {
            return true;
        }
    }
    return false;
}

std::string yarp::robotinterface::experimental::findParam(const yarp::robotinterface::experimental::ParamList& list, const std::string& name)
{
    for (const auto& param : list) {
        if (name == param.name()) {
            return param.value();
        }
    }
    yError() << "Param" << name << "not found";
    return {};
}

bool yarp::robotinterface::experimental::hasGroup(const yarp::robotinterface::experimental::ParamList& list, const std::string& name)
{
    for (const auto& param : list) {
        if (param.isGroup() && name == param.name()) {
            return true;
        }
    }
    return false;
}

std::string yarp::robotinterface::experimental::findGroup(const yarp::robotinterface::experimental::ParamList& list, const std::string& name)
{
    for (const auto& param : list) {
        if (param.isGroup() && name == param.name()) {
            return param.value();
        }
    }
    yError() << "Param" << name << "not found";
    return {};
}

yarp::robotinterface::experimental::ParamList yarp::robotinterface::experimental::mergeDuplicateGroups(const yarp::robotinterface::experimental::ParamList& list)
{
    yarp::robotinterface::experimental::ParamList params = list;
    for (auto it1 = params.begin(); it1 != params.end(); ++it1) {
        yarp::robotinterface::experimental::Param& param1 = *it1;
        for (auto it2 = it1 + 1; it2 != params.end();) {
            yarp::robotinterface::experimental::Param& param2 = *it2;
            if (param1.name() == param2.name()) {
                if (!param1.isGroup() || !param2.isGroup()) {
                    yFatal() << "Duplicate parameter \"" << param1.name() << "\" found and at least one of them is not a group.";
                }
                param1.value() += std::string(" ");
                param1.value() += param2.value();
                it2 = params.erase(it2);
            } else
                it2++;
        }
    }
    return params;
}


yarp::robotinterface::experimental::ActionPhase yarp::robotinterface::experimental::StringToActionPhase(const std::string& phase)
{
    if (phase == "startup") {
        return yarp::robotinterface::experimental::ActionPhaseStartup;
    } else if (phase == "run") {
        return yarp::robotinterface::experimental::ActionPhaseRun;
    } else if (phase == "interrupt1") {
        return yarp::robotinterface::experimental::ActionPhaseInterrupt1;
    } else if (phase == "interrupt2") {
        return yarp::robotinterface::experimental::ActionPhaseInterrupt2;
    } else if (phase == "interrupt3") {
        return yarp::robotinterface::experimental::ActionPhaseInterrupt3;
    } else if (phase == "shutdown") {
        return yarp::robotinterface::experimental::ActionPhaseShutdown;
    }
    return yarp::robotinterface::experimental::ActionPhaseReserved;
}

std::string yarp::robotinterface::experimental::ActionPhaseToString(yarp::robotinterface::experimental::ActionPhase actionphase)
{
    switch (actionphase) {
    case yarp::robotinterface::experimental::ActionPhaseStartup:
        return std::string("startup");
    case yarp::robotinterface::experimental::ActionPhaseRun:
        return std::string("run");
    case yarp::robotinterface::experimental::ActionPhaseInterrupt1:
        return std::string("interrupt1");
    case yarp::robotinterface::experimental::ActionPhaseInterrupt2:
        return std::string("interrupt2");
    case yarp::robotinterface::experimental::ActionPhaseInterrupt3:
        return std::string("interrupt3");
    case yarp::robotinterface::experimental::ActionPhaseShutdown:
        return std::string("shutdown");
    case yarp::robotinterface::experimental::ActionPhaseUnknown:
    default:
        return {};
    }
}

void yarp::robotinterface::experimental::operator>>(const std::stringstream& sstream, yarp::robotinterface::experimental::ActionPhase& actionphase)
{
    actionphase = yarp::robotinterface::experimental::StringToActionPhase(sstream.str());
}


yarp::robotinterface::experimental::ActionType yarp::robotinterface::experimental::StringToActionType(const std::string& type)
{
    if (type == "configure") {
        return yarp::robotinterface::experimental::ActionTypeConfigure;
    } else if (type == "calibrate") {
        return yarp::robotinterface::experimental::ActionTypeCalibrate;
    } else if (type == "attach") {
        return yarp::robotinterface::experimental::ActionTypeAttach;
    } else if (type == "abort") {
        return yarp::robotinterface::experimental::ActionTypeAbort;
    } else if (type == "detach") {
        return yarp::robotinterface::experimental::ActionTypeDetach;
    } else if (type == "park") {
        return yarp::robotinterface::experimental::ActionTypePark;
    } else if (type == "custom") {
        return yarp::robotinterface::experimental::ActionTypeCustom;
    } else {
        return yarp::robotinterface::experimental::ActionTypeUnknown;
    }
}

std::string yarp::robotinterface::experimental::ActionTypeToString(yarp::robotinterface::experimental::ActionType actiontype)
{
    switch (actiontype) {
    case yarp::robotinterface::experimental::ActionTypeConfigure:
        return std::string("configure");
    case yarp::robotinterface::experimental::ActionTypeCalibrate:
        return std::string("calibrate");
    case yarp::robotinterface::experimental::ActionTypeAttach:
        return std::string("attach");
    case yarp::robotinterface::experimental::ActionTypeAbort:
        return std::string("abort");
    case yarp::robotinterface::experimental::ActionTypeDetach:
        return std::string("detach");
    case yarp::robotinterface::experimental::ActionTypePark:
        return std::string("park");
    case yarp::robotinterface::experimental::ActionTypeCustom:
        return std::string("custom");
    case yarp::robotinterface::experimental::ActionTypeUnknown:
    default:
        return {};
    }
}

void yarp::robotinterface::experimental::operator>>(const std::stringstream& sstream, yarp::robotinterface::experimental::ActionType& actiontype)
{
    actiontype = yarp::robotinterface::experimental::StringToActionType(sstream.str());
}
