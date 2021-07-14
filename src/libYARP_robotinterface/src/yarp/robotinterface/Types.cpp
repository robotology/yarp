/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/robotinterface/Types.h>

#include <yarp/robotinterface/Param.h>

#include <yarp/os/LogStream.h>

#include <sstream>
#include <string>


bool yarp::robotinterface::hasParam(const yarp::robotinterface::ParamList& list, const std::string& name)
{
    for (const auto& param : list) {
        if (name == param.name()) {
            return true;
        }
    }
    return false;
}

std::string yarp::robotinterface::findParam(const yarp::robotinterface::ParamList& list, const std::string& name)
{
    for (const auto& param : list) {
        if (name == param.name()) {
            return param.value();
        }
    }
    yError() << "Param" << name << "not found";
    return {};
}

bool yarp::robotinterface::hasGroup(const yarp::robotinterface::ParamList& list, const std::string& name)
{
    for (const auto& param : list) {
        if (param.isGroup() && name == param.name()) {
            return true;
        }
    }
    return false;
}

std::string yarp::robotinterface::findGroup(const yarp::robotinterface::ParamList& list, const std::string& name)
{
    for (const auto& param : list) {
        if (param.isGroup() && name == param.name()) {
            return param.value();
        }
    }
    yError() << "Param" << name << "not found";
    return {};
}

yarp::robotinterface::ParamList yarp::robotinterface::mergeDuplicateGroups(const yarp::robotinterface::ParamList& list)
{
    yarp::robotinterface::ParamList params = list;
    for (auto it1 = params.begin(); it1 != params.end(); ++it1) {
        yarp::robotinterface::Param& param1 = *it1;
        for (auto it2 = it1 + 1; it2 != params.end();) {
            yarp::robotinterface::Param& param2 = *it2;
            if (param1.name() == param2.name()) {
                if (!param1.isGroup() || !param2.isGroup()) {
                    yFatal() << "Duplicate parameter \"" << param1.name() << "\" found and at least one of them is not a group.";
                }
                param1.value() += std::string(" ");
                param1.value() += param2.value();
                it2 = params.erase(it2);
            } else {
                it2++;
            }
        }
    }
    return params;
}


yarp::robotinterface::ActionPhase yarp::robotinterface::StringToActionPhase(const std::string& phase)
{
    if (phase == "startup") {
        return yarp::robotinterface::ActionPhaseStartup;
    } else if (phase == "run") {
        return yarp::robotinterface::ActionPhaseRun;
    } else if (phase == "interrupt1") {
        return yarp::robotinterface::ActionPhaseInterrupt1;
    } else if (phase == "interrupt2") {
        return yarp::robotinterface::ActionPhaseInterrupt2;
    } else if (phase == "interrupt3") {
        return yarp::robotinterface::ActionPhaseInterrupt3;
    } else if (phase == "shutdown") {
        return yarp::robotinterface::ActionPhaseShutdown;
    }
    return yarp::robotinterface::ActionPhaseReserved;
}

std::string yarp::robotinterface::ActionPhaseToString(yarp::robotinterface::ActionPhase actionphase)
{
    switch (actionphase) {
    case yarp::robotinterface::ActionPhaseStartup:
        return std::string("startup");
    case yarp::robotinterface::ActionPhaseRun:
        return std::string("run");
    case yarp::robotinterface::ActionPhaseInterrupt1:
        return std::string("interrupt1");
    case yarp::robotinterface::ActionPhaseInterrupt2:
        return std::string("interrupt2");
    case yarp::robotinterface::ActionPhaseInterrupt3:
        return std::string("interrupt3");
    case yarp::robotinterface::ActionPhaseShutdown:
        return std::string("shutdown");
    case yarp::robotinterface::ActionPhaseUnknown:
    default:
        return {};
    }
}

void yarp::robotinterface::operator>>(const std::stringstream& sstream, yarp::robotinterface::ActionPhase& actionphase)
{
    actionphase = yarp::robotinterface::StringToActionPhase(sstream.str());
}


yarp::robotinterface::ActionType yarp::robotinterface::StringToActionType(const std::string& type)
{
    if (type == "configure") {
        return yarp::robotinterface::ActionTypeConfigure;
    } else if (type == "calibrate") {
        return yarp::robotinterface::ActionTypeCalibrate;
    } else if (type == "attach") {
        return yarp::robotinterface::ActionTypeAttach;
    } else if (type == "abort") {
        return yarp::robotinterface::ActionTypeAbort;
    } else if (type == "detach") {
        return yarp::robotinterface::ActionTypeDetach;
    } else if (type == "park") {
        return yarp::robotinterface::ActionTypePark;
    } else if (type == "custom") {
        return yarp::robotinterface::ActionTypeCustom;
    } else {
        return yarp::robotinterface::ActionTypeUnknown;
    }
}

std::string yarp::robotinterface::ActionTypeToString(yarp::robotinterface::ActionType actiontype)
{
    switch (actiontype) {
    case yarp::robotinterface::ActionTypeConfigure:
        return std::string("configure");
    case yarp::robotinterface::ActionTypeCalibrate:
        return std::string("calibrate");
    case yarp::robotinterface::ActionTypeAttach:
        return std::string("attach");
    case yarp::robotinterface::ActionTypeAbort:
        return std::string("abort");
    case yarp::robotinterface::ActionTypeDetach:
        return std::string("detach");
    case yarp::robotinterface::ActionTypePark:
        return std::string("park");
    case yarp::robotinterface::ActionTypeCustom:
        return std::string("custom");
    case yarp::robotinterface::ActionTypeUnknown:
    default:
        return {};
    }
}

void yarp::robotinterface::operator>>(const std::stringstream& sstream, yarp::robotinterface::ActionType& actiontype)
{
    actiontype = yarp::robotinterface::StringToActionType(sstream.str());
}
