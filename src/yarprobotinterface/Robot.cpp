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

#include "Robot.h"
#include "Action.h"
#include "Device.h"
#include "Param.h"

#include <yarp/os/LogStream.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/PolyDriverList.h>

#include <string>
#include <iostream>
#include <algorithm>


std::ostringstream& operator<<(std::ostringstream &oss, const RobotInterface::Robot &t)
{
    oss << "(name = \"" << t.name() << "\"";
    if (!t.params().empty()) {
        oss << ", params = [";
        oss << t.params();
        oss << "]";
    }
    if (!t.devices().empty()) {
        oss << ", devices = [";
        oss << t.devices();
        oss << "]";
    }
    oss << ")";
    return oss;
}


class RobotInterface::Robot::Private
{
public:
    Private(Robot * /*parent*/) :
            build(0),
                currentPhase(ActionPhaseUnknown),
        currentLevel(0)
    {
    }

    // return true if a device with the given name exists
    bool hasDevice(const std::string &name) const;

    // return the device with the given name or <fatal error> if not found
    Device* findDevice(const std::string &name);

    // open all the devices and return true if all the open calls were successful
    bool openDevices();

    // close all the devices and return true if all the close calls were successful
    bool closeDevices();

    // return a vector of levels that have actions in the requested phase
    std::vector<unsigned int> getLevels(ActionPhase phase) const;

    // return a vector of actions for that phase and that level
    std::vector<std::pair<Device, Action> > getActions(ActionPhase phase, unsigned int level) const;


    // run configure action on one device
    bool configure(const Device &device, const ParamList &params);

    // run calibrate action on one device
    bool calibrate(const Device &device, const ParamList &params);

    // run attach action on one device
    bool attach(const Device &device, const ParamList &params);

    // run abort action on one device
    bool abort(const Device &device, const ParamList &params);

    // run detach action on one device
    bool detach(const Device &device, const ParamList &params);

    // run park action on one device
    bool park(const Device &device, const ParamList &params);

    // run custom action on one device
    bool custom(const Device &device, const ParamList &params);

    std::string name;
    unsigned int build;
    std::string portprefix;
    ParamList params;
    DeviceList devices;
    RobotInterface::ActionPhase currentPhase;
    unsigned int currentLevel;
}; // class RobotInterface::Robot::Private

bool RobotInterface::Robot::Private::hasDevice(const std::string &name) const
{
    for (const auto& device : devices) {
        if (name == device.name()) {
            return true;
        }
    }
    return false;
}

RobotInterface::Device* RobotInterface::Robot::Private::findDevice(const std::string &name)
{
    for (auto& device : devices) {
        if (name == device.name()) {
            return &device;
        }
    }
    yFatal() << "Cannot find device" << name;
    return nullptr;
}

bool RobotInterface::Robot::Private::openDevices()
{
    bool ret = true;
    for (auto& device : devices) {
        // yDebug() << device;

        if (!device.open()) {
            yWarning() << "Cannot open device" << device.name();
            ret = false;
        }

    }
    if (ret) {
        // yDebug() << "All devices opened.";
    } else {
        yWarning() << "There was some problem opening one or more devices. Please check the log and your configuration";
    }

    return ret;
}

bool RobotInterface::Robot::Private::closeDevices()
{
    bool ret = true;
    for (auto it = devices.rbegin(); it != devices.rend(); ++it) {
        RobotInterface::Device &device = *it;

        // yDebug() << device;

        if (!device.close()) {
            yWarning() << "Cannot close device" << device.name();
            ret = false;
        }

    }
    if (ret) {
        // yDebug() << "All devices closed.";
    } else {
        yWarning() << "There was some problem closing one or more devices. Please check the log and your configuration";
    }

    return ret;
}

std::vector<unsigned int> RobotInterface::Robot::Private::getLevels(RobotInterface::ActionPhase phase) const
{
    std::vector<unsigned int> levels;
    for (const auto& device : devices) {
        if (device.actions().empty()) {
            continue;
        }

        for (const auto& action : device.actions()) {
            if (action.phase() == phase) {
                levels.push_back(action.level());
            }
        }
    }

    std::sort(levels.begin(), levels.end());
    auto it = std::unique(levels.begin(), levels.end());
    levels.resize(it - levels.begin());

    return levels;
}


std::vector<std::pair<RobotInterface::Device, RobotInterface::Action> > RobotInterface::Robot::Private::getActions(RobotInterface::ActionPhase phase, unsigned int level) const
{
    std::vector<std::pair<RobotInterface::Device, RobotInterface::Action> > actions;
    for (const auto& device : devices) {
        if (device.actions().empty()) {
            continue;
        }

        for (const auto& action : device.actions()) {
            if (action.phase() == phase && action.level() == level) {
                actions.emplace_back(device, action);
            }
        }
    }
    return actions;
}


bool RobotInterface::Robot::Private::configure(const RobotInterface::Device &device, const RobotInterface::ParamList &params)
{
    YARP_FIXME_NOTIMPLEMENTED("configure action")
    return true;
}

bool RobotInterface::Robot::Private::calibrate(const RobotInterface::Device &device, const RobotInterface::ParamList &params)
{
    if (!RobotInterface::hasParam(params, "target")) {
        yError() << "Action \"" << ActionTypeToString(ActionTypeCalibrate) << R"(" requires "target" parameter)";
        return false;
    }
    std::string targetDeviceName = RobotInterface::findParam(params, "target");

    if (!hasDevice(targetDeviceName)) {
        yError() << "Target device" << targetDeviceName << "does not exist.";
        return false;
    }
    Device &targetDevice = *findDevice(targetDeviceName);

    return device.calibrate(targetDevice);
}

bool RobotInterface::Robot::Private::attach(const RobotInterface::Device &device, const RobotInterface::ParamList &params)
{
    int check = 0;
    if (RobotInterface::hasParam(params, "network")) check++;
    if (RobotInterface::hasParam(params, "networks")) check++;
    if (RobotInterface::hasParam(params, "all")) check++;

    if (check>1)
    {
        yError() << "Action \"" << ActionTypeToString(ActionTypeAttach) << R"(" : you can have only one option: "network" , "networks" or "all" )";
        return false;
    }

    yarp::dev::PolyDriverList drivers;

    if (RobotInterface::hasParam(params, "network")) {
        std::string targetNetwork = RobotInterface::findParam(params, "network");

        if (!RobotInterface::hasParam(params, "device")) {
            yError() << "Action \"" << ActionTypeToString(ActionTypeAttach) << R"(" requires "device" parameter)";
            return false;
        }
        std::string targetDeviceName = RobotInterface::findParam(params, "device");

        if (!hasDevice(targetDeviceName)) {
            yError() << "Target device" << targetDeviceName << "(network =" << targetNetwork << ") does not exist.";
            return false;
        }
        Device &targetDevice = *findDevice(targetDeviceName);

        // yDebug() << "Attach device" << device.name() << "to" << targetDevice.name() << "as" << targetNetwork;
        drivers.push(targetDevice.driver(), targetNetwork.c_str());

    }
    else if (RobotInterface::hasParam(params, "all"))
    {
        for (auto& device : devices)
        {
            drivers.push(device.driver(), "all");
        }
    }
    else if (RobotInterface::hasParam(params, "networks")) {
        yarp::os::Value v;
        v.fromString(RobotInterface::findParam(params, "networks").c_str());
        yarp::os::Bottle &targetNetworks = *(v.asList());

        for (size_t i = 0; i < targetNetworks.size(); ++i) {
            std::string targetNetwork = targetNetworks.get(i).toString();

            if (!RobotInterface::hasParam(params, targetNetwork)) {
                yError() << "Action \"" << ActionTypeToString(ActionTypeAttach) << "\" requires one parameter per network. \"" << targetNetwork << "\" parameter is missing.";
                return false;
            }
            std::string targetDeviceName = RobotInterface::findParam(params, targetNetwork);
            if (!hasDevice(targetDeviceName)) {
                yError() << "Target device" << targetDeviceName << "(network =" << targetNetwork << ") does not exist.";
                return false;
            }
            Device &targetDevice = *findDevice(targetDeviceName);

            // yDebug() << "Attach device" << device.name() << "to" << targetDevice.name() << "as" << targetNetwork;
            drivers.push(targetDevice.driver(), targetNetwork.c_str());
        }
    }
    else
    {
        yError() << "Action \"" << ActionTypeToString(ActionTypeAttach) << R"(" requires either "network" or "networks" parameter)";
        return false;
    }

    if (!drivers.size()) {
        yError() << "Action \"" << ActionTypeToString(ActionTypeAttach) << "\" couldn't find any device.";
        return false;
    }

    return device.attach(drivers);
}

bool RobotInterface::Robot::Private::abort(const RobotInterface::Device &device, const RobotInterface::ParamList &params)
{
    YARP_FIXME_NOTIMPLEMENTED("abort action")
    return true;
}


bool RobotInterface::Robot::Private::detach(const RobotInterface::Device &device, const RobotInterface::ParamList &params)
{

    if (!params.empty()) {
        yWarning() << "Action \"" << ActionTypeToString(ActionTypeDetach) << "\" cannot have any parameter. Ignoring them.";
    }

    return device.detach();
}

bool RobotInterface::Robot::Private::park(const RobotInterface::Device &device, const RobotInterface::ParamList &params)
{
    if (!RobotInterface::hasParam(params, "target")) {
        yError() << "Action \"" << ActionTypeToString(ActionTypePark) << R"(" requires "target" parameter)";
        return false;
    }
    std::string targetDeviceName = RobotInterface::findParam(params, "target");

    if (!hasDevice(targetDeviceName)) {
        yError() << "Target device" << targetDeviceName << "does not exist.";
        return false;
    }
    Device &targetDevice = *findDevice(targetDeviceName);

    return device.park(targetDevice);
}

bool RobotInterface::Robot::Private::custom(const RobotInterface::Device &device, const RobotInterface::ParamList &params)
{
    YARP_FIXME_NOTIMPLEMENTED("custom action")
    return true;
}

yarp::os::LogStream operator<<(yarp::os::LogStream dbg, const RobotInterface::Robot &t)
{
    std::ostringstream oss;
    oss << t;
    dbg << oss.str();
    return dbg;
}

RobotInterface::Robot::Robot() :
    mPriv(new Private(this))
{

}

RobotInterface::Robot::Robot(const std::string& name, const RobotInterface::DeviceList& devices) :
    mPriv(new Private(this))
{
    mPriv->name = name;
    mPriv->devices = devices;
}

RobotInterface::Robot::Robot(const RobotInterface::Robot& other) :
    mPriv(new Private(this))
{
    mPriv->name = other.mPriv->name;
    mPriv->build = other.mPriv->build;
    mPriv->portprefix = other.mPriv->portprefix;
    mPriv->currentPhase = other.mPriv->currentPhase;
    mPriv->currentLevel = other.mPriv->currentLevel;
    mPriv->devices = other.mPriv->devices;
    mPriv->params = other.mPriv->params;
}

RobotInterface::Robot& RobotInterface::Robot::operator=(const RobotInterface::Robot& other)
{
    if (&other != this) {
        mPriv->name = other.mPriv->name;
        mPriv->build = other.mPriv->build;
        mPriv->portprefix = other.mPriv->portprefix;
        mPriv->currentPhase = other.mPriv->currentPhase;
        mPriv->currentLevel = other.mPriv->currentLevel;

        mPriv->devices.clear();
        mPriv->devices = other.mPriv->devices;

        mPriv->params.clear();
        mPriv->params = other.mPriv->params;
    }

    return *this;
}

RobotInterface::Robot::~Robot()
{
    delete mPriv;
}

std::string& RobotInterface::Robot::name()
{
    return mPriv->name;
}

unsigned int& RobotInterface::Robot::build()
{
    return mPriv->build;
}

std::string& RobotInterface::Robot::portprefix()
{
    return mPriv->portprefix;
}

void RobotInterface::Robot::setVerbose(bool verbose)
{
    for (auto& device : devices()) {
        ParamList &params = device.params();
        // Do not override "verbose" param if explicitly set in the xml
        if(verbose && !RobotInterface::hasParam(params, "verbose")) {
            device.params().push_back(Param("verbose", "1"));
        }
    }
}

void RobotInterface::Robot::setAllowDeprecatedDevices(bool allowDeprecatedDevices)
{
    for (auto& device : devices()) {
        ParamList &params = device.params();
        // Do not override "allow-deprecated-devices" param if explicitly set in the xml
        if(allowDeprecatedDevices && !RobotInterface::hasParam(params, "allow-deprecated-devices")) {
            device.params().push_back(Param("allow-deprecated-devices", "1"));
        }
    }
}

RobotInterface::ParamList& RobotInterface::Robot::params()
{
    return mPriv->params;
}

RobotInterface::DeviceList& RobotInterface::Robot::devices()
{
    return mPriv->devices;
}

RobotInterface::Device& RobotInterface::Robot::device(const std::string& name)
{
    return *mPriv->findDevice(name);
}

const std::string& RobotInterface::Robot::name() const
{
    return mPriv->name;
}

const unsigned int& RobotInterface::Robot::build() const
{
    return mPriv->build;
}

const std::string& RobotInterface::Robot::portprefix() const
{
    return mPriv->portprefix;
}

const RobotInterface::ParamList& RobotInterface::Robot::params() const
{
    return mPriv->params;
}

const RobotInterface::DeviceList& RobotInterface::Robot::devices() const
{
    return mPriv->devices;
}

const RobotInterface::Device& RobotInterface::Robot::device(const std::string& name) const
{
    return *mPriv->findDevice(name);
}

void RobotInterface::Robot::interrupt()
{
    yInfo() << "Interrupt received. Stopping all running threads.";

    // If we received an interrupt we send a stop signal to all threads
    // from previous phases
    for (auto& device : devices()) {
        device.stopThreads();
    }
}

bool RobotInterface::Robot::enterPhase(RobotInterface::ActionPhase phase)
{
    yInfo() << ActionPhaseToString(phase) << "phase starting...";

    mPriv->currentPhase = phase;
    mPriv->currentLevel = 0;

    // Open all devices
    if (phase == ActionPhaseStartup) {
        if (!mPriv->openDevices()) {
            yError() << "One or more devices failed opening... see previous log messages for more info";
            if (!mPriv->closeDevices()) {
                yError() << "One or more devices failed closing";
            }
            return false;
        }
    }

    // run phase does not accept actions
    if (phase == ActionPhaseRun) {
        if(mPriv->getLevels(phase).size() != 0) {
            yWarning() << "Phase" << ActionPhaseToString(phase) << "does not accept actions. Skipping all actions for this phase";
        }
        return true;
    }

    // Before starting any action we ensure that there are no other
    // threads running from prevoius phases.
    // In interrupt 2 and 3 and this is called by the interrupt callback,
    // and therefore main thread will be blocked and join will never
    // return. Therefore, since we want to start the abort actions we
    // skip this check.
    if (phase != ActionPhaseInterrupt2 && phase != ActionPhaseInterrupt3) {
        for (auto& device : devices()) {
            device.joinThreads();
        }
    }

    std::vector<unsigned int> levels = mPriv->getLevels(phase);

    bool ret = true;
    for (std::vector<unsigned int>::const_iterator lit = levels.begin(); lit != levels.end(); ++lit) {
        // for each level
        const unsigned int level = *lit;

        yInfo() << "Entering action level" << level << "of phase" << ActionPhaseToString(phase);
        mPriv->currentLevel = level;

        // If current phase was changed by some other thread, we should
        // exit the loop and avoid starting further actions.
        if(mPriv->currentPhase != phase) {
            ret = false;
            break;
        }

        std::vector<std::pair<Device, Action> > actions = mPriv->getActions(phase, level);

        for (auto& ait : actions) {
            // for each action in that level
            Device &device = ait.first;
            Action &action = ait.second;

            // If current phase was changed by some other thread, we should
            // exit the loop and avoid starting further actions.
            if(mPriv->currentPhase != phase) {
                ret = false;
                break;
            }

            switch (action.type()) {
            case ActionTypeConfigure:
                if(!mPriv->configure(device, action.params())) {
                    yError() << "Cannot run configure action on device" << device.name();
                    ret = false;
                }
                break;
            case ActionTypeCalibrate:
                if(!mPriv->calibrate(device, action.params())) {
                    yError() << "Cannot run calibrate action on device" << device.name();
                    ret = false;
                }
                break;
            case ActionTypeAttach:
                if (!mPriv->attach(device, action.params())) {
                    yError() << "Cannot run attach action on device" << device.name();
                    ret = false;
                }
                break;
            case ActionTypeAbort:
                if(!mPriv->abort(device, action.params())) {
                    yError() << "Cannot run abort action on device" << device.name();
                    ret = false;
                }
                break;
            case ActionTypeDetach:
                if (!mPriv->detach(device, action.params())) {
                    yError() << "Cannot run detach action on device" << device.name();
                    ret = false;
                }
                break;
            case ActionTypePark:
                if (!mPriv->park(device, action.params())) {
                    yError() << "Cannot run park action on device" << device.name();
                    ret = false;
                }
                break;
            case ActionTypeCustom:
                if (!mPriv->custom(device, action.params())) {
                    yError() << "Cannot run custom action on device" << device.name();
                    ret = false;
                }
                break;
            default:
                yWarning() << "Unhandled action" << ActionTypeToString(action.type());
                ret = false;
                break;
            }
        }

        yInfo() << "All actions for action level" << level << "of" << ActionPhaseToString(phase) << "phase started. Waiting for unfinished actions.";

        // Join parallel threads
        for (auto& device : devices()) {
            device.joinThreads();
            // yDebug() << "All actions for device" << device.name() << "at level()" << level << "finished";
        }

        yInfo() << "All actions for action level" << level << "of" << ActionPhaseToString(phase) << "phase finished.";
    }

    if (!ret) {
        yWarning() << "There was some problem running actions for" << ActionPhaseToString(phase) << "phase . Please check the log and your configuration";
    }

    if (phase == ActionPhaseShutdown) {
        if (!mPriv->closeDevices()) {
            yError() << "One or more devices failed closing";
            return false;
        }
    }

    yInfo() << ActionPhaseToString(phase) << "phase finished.";

    return ret;
}

RobotInterface::ActionPhase RobotInterface::Robot::currentPhase() const
{
    return mPriv->currentPhase;
}

int RobotInterface::Robot::currentLevel() const
{
    return mPriv->currentLevel;
}

bool RobotInterface::Robot::hasParam(const std::string& name) const
{
    return RobotInterface::hasParam(mPriv->params, name);
}

std::string RobotInterface::Robot::findParam(const std::string& name) const
{
    return RobotInterface::findParam(mPriv->params, name);
}
