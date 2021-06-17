/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/robotinterface/experimental/Robot.h>

#include <yarp/robotinterface/experimental/Action.h>
#include <yarp/robotinterface/experimental/Device.h>
#include <yarp/robotinterface/experimental/Param.h>

#include <yarp/os/LogStream.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/PolyDriverList.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_set>


yarp::os::LogStream operator<<(yarp::os::LogStream dbg, const yarp::robotinterface::experimental::Robot& t)
{
    dbg << "(name = \"" << t.name() << "\"";
    if (!t.params().empty()) {
        dbg << ", params = [";
        dbg << t.params();
        dbg << "]";
    }
    if (!t.devices().empty()) {
        dbg << ", devices = [";
        dbg << t.devices();
        dbg << "]";
    }
    dbg << ")";
    return dbg;
}


class yarp::robotinterface::experimental::Robot::Private
{
public:
    Private(Robot* /*parent*/) :
            build(0),
            currentPhase(ActionPhaseUnknown),
            currentLevel(0)
    {
    }

    // return true if a device with the given name exists
    bool hasDevice(const std::string& name) const;

    // return the device with the given name or <fatal error> if not found
    Device* findDevice(const std::string& name);

    // return true if a device with the given name exists
    // considering also the provided external devices
    bool hasDeviceIncludingExternal(const std::string& name) const;

    // return the device with the given name or nullptr if not found,
    // considering also the provided external devices
    yarp::dev::PolyDriverDescriptor findDeviceIncludingExternal(const std::string& name);

    // check if there is no external devices that has the same name of an internal device
    // return true if there is a conflict, false otherwise
    bool checkForNamingConflictsInExternalDevices(const yarp::dev::PolyDriverList& newExternalDevicesList);

    // open all the devices and return true if all the open calls were successful
    bool openDevices();

    // close all the devices and return true if all the close calls were successful
    bool closeDevices();

    // return a vector of levels that have actions in the requested phase
    std::vector<unsigned int> getLevels(ActionPhase phase) const;

    // return a vector of actions for that phase and that level
    std::vector<std::pair<Device, Action>> getActions(ActionPhase phase, unsigned int level) const;


    // run configure action on one device
    bool configure(const Device& device, const ParamList& params);

    // run calibrate action on one device
    bool calibrate(const Device& device, const ParamList& params);

    // run attach action on one device
    bool attach(const Device& device, const ParamList& params);

    // run abort action on one device
    bool abort(const Device& device, const ParamList& params);

    // run detach action on one device
    bool detach(const Device& device, const ParamList& params);

    // run park action on one device
    bool park(const Device& device, const ParamList& params);

    // run custom action on one device
    bool custom(const Device& device, const ParamList& params);

    std::string name;
    unsigned int build;
    std::string portprefix;
    ParamList params;
    DeviceList devices;
    yarp::dev::PolyDriverList externalDevices;
    yarp::robotinterface::experimental::ActionPhase currentPhase;
    unsigned int currentLevel;
}; // class yarp::robotinterface::experimental::Robot::Private

bool yarp::robotinterface::experimental::Robot::Private::hasDevice(const std::string& name) const
{
    for (const auto& device : devices) {
        if (name == device.name()) {
            return true;
        }
    }
    return false;
}

yarp::robotinterface::experimental::Device* yarp::robotinterface::experimental::Robot::Private::findDevice(const std::string& name)
{
    for (auto& device : devices) {
        if (name == device.name()) {
            return &device;
        }
    }
    return nullptr;
}

bool yarp::robotinterface::experimental::Robot::Private::hasDeviceIncludingExternal(const std::string& name) const
{
    if (hasDevice(name)) {
        return true;
    } else {
        for (int i = 0; i < externalDevices.size(); i++) {
            const yarp::dev::PolyDriverDescriptor* externalDevice = externalDevices[i];
            if (name == externalDevice->key) {
                return true;
            }
        }
    }
    return false;
}

yarp::dev::PolyDriverDescriptor
yarp::robotinterface::experimental::Robot::Private::findDeviceIncludingExternal(const std::string& name)
{
    yarp::dev::PolyDriverDescriptor deviceFound;
    deviceFound.poly = nullptr;
    deviceFound.key = "";

    for (auto& device : devices) {
        if (name == device.name()) {
            deviceFound.poly = device.driver();
            deviceFound.key = device.name();
            return deviceFound;
        }
    }

    for (int i = 0; i < externalDevices.size(); i++) {
        const yarp::dev::PolyDriverDescriptor* externalDevice = externalDevices[i];
        if (name == externalDevice->key) {
            deviceFound = *externalDevice;
        }
    }

    return deviceFound;
}

bool yarp::robotinterface::experimental::Robot::Private::checkForNamingConflictsInExternalDevices(const yarp::dev::PolyDriverList& externalDevicesList)
{
    std::unordered_set<std::string> externalDevicesNames;

    for (int i = 0; i < externalDevicesList.size(); i++) {
        const yarp::dev::PolyDriverDescriptor* externalDevice = externalDevicesList[i];
        externalDevicesNames.insert(externalDevice->key);
    }

    for (auto& device : devices) {
        if (externalDevicesNames.find(device.name()) != externalDevicesNames.end()) {
            yError() << "Device name " << device.name() << " is used for both an internal and external device.";
            return true;
        }
    }

    return false;
}


bool yarp::robotinterface::experimental::Robot::Private::openDevices()
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

bool yarp::robotinterface::experimental::Robot::Private::closeDevices()
{
    bool ret = true;
    for (auto it = devices.rbegin(); it != devices.rend(); ++it) {
        yarp::robotinterface::experimental::Device& device = *it;

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

std::vector<unsigned int> yarp::robotinterface::experimental::Robot::Private::getLevels(yarp::robotinterface::experimental::ActionPhase phase) const
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


std::vector<std::pair<yarp::robotinterface::experimental::Device, yarp::robotinterface::experimental::Action>> yarp::robotinterface::experimental::Robot::Private::getActions(yarp::robotinterface::experimental::ActionPhase phase, unsigned int level) const
{
    std::vector<std::pair<yarp::robotinterface::experimental::Device, yarp::robotinterface::experimental::Action>> actions;
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


bool yarp::robotinterface::experimental::Robot::Private::configure(const yarp::robotinterface::experimental::Device& device, const yarp::robotinterface::experimental::ParamList& params)
{
    YARP_FIXME_NOTIMPLEMENTED("configure action")
    return true;
}

bool yarp::robotinterface::experimental::Robot::Private::calibrate(const yarp::robotinterface::experimental::Device& device,
                                                                   const yarp::robotinterface::experimental::ParamList& params)
{
    if (!yarp::robotinterface::experimental::hasParam(params, "target")) {
        yError() << "Action \"" << ActionTypeToString(ActionTypeCalibrate) << R"(" requires "target" parameter)";
        return false;
    }
    std::string targetDeviceName = yarp::robotinterface::experimental::findParam(params, "target");

    if (!hasDeviceIncludingExternal(targetDeviceName)) {
        yError() << "Target device" << targetDeviceName << "does not exist.";
        return false;
    }
    yarp::dev::PolyDriverDescriptor targetDevice = findDeviceIncludingExternal(targetDeviceName);

    return device.calibrate(targetDevice);
}

bool yarp::robotinterface::experimental::Robot::Private::attach(const yarp::robotinterface::experimental::Device& device,
                                                                const yarp::robotinterface::experimental::ParamList& params)
{
    int check = 0;
    if (yarp::robotinterface::experimental::hasParam(params, "network"))
        check++;
    if (yarp::robotinterface::experimental::hasParam(params, "networks"))
        check++;
    if (yarp::robotinterface::experimental::hasParam(params, "all"))
        check++;

    if (check > 1) {
        yError() << "Action \"" << ActionTypeToString(ActionTypeAttach) << R"(" : you can have only one option: "network" , "networks" or "all" )";
        return false;
    }

    yarp::dev::PolyDriverList drivers;

    if (yarp::robotinterface::experimental::hasParam(params, "device")) {
        std::string targetDeviceName = yarp::robotinterface::experimental::findParam(params, "device");

        std::string targetNetwork = "...";
        if (yarp::robotinterface::experimental::hasParam(params, "network")) {
            targetNetwork = yarp::robotinterface::experimental::findParam(params, "network");
        }

        if (!hasDeviceIncludingExternal(targetDeviceName)) {
            yError() << "Target device" << targetDeviceName << "(network =" << targetNetwork << ") does not exist.";
            return false;
        }
        yarp::dev::PolyDriverDescriptor targetDevice = findDeviceIncludingExternal(targetDeviceName);

        // yDebug() << "Attach device" << device.name() << "to" << targetDevice.name() << "as" << targetNetwork;
        drivers.push(targetDevice.poly, targetNetwork.c_str());

    } else if (yarp::robotinterface::experimental::hasParam(params, "all")) {
        for (auto& device : devices) {
            drivers.push(device.driver(), "all");
        }

        for (int i = 0; i < externalDevices.size(); i++) {
            const yarp::dev::PolyDriverDescriptor* externalDevice = externalDevices[i];
            drivers.push(externalDevice->poly, "all");
        }
    } else if (yarp::robotinterface::experimental::hasParam(params, "networks")) {
        yarp::os::Value v;
        v.fromString(yarp::robotinterface::experimental::findParam(params, "networks").c_str());
        yarp::os::Bottle& targetNetworks = *(v.asList());

        for (size_t i = 0; i < targetNetworks.size(); ++i) {
            std::string targetNetwork = targetNetworks.get(i).toString();

            if (!yarp::robotinterface::experimental::hasParam(params, targetNetwork)) {
                yError() << "Action \"" << ActionTypeToString(ActionTypeAttach) << "\" requires one parameter per network. \"" << targetNetwork << "\" parameter is missing.";
                return false;
            }
            std::string targetDeviceName = yarp::robotinterface::experimental::findParam(params, targetNetwork);
            if (!hasDeviceIncludingExternal(targetDeviceName)) {
                yError() << "Target device" << targetDeviceName << "(network =" << targetNetwork << ") does not exist.";
                return false;
            }
            yarp::dev::PolyDriverDescriptor targetDevice = findDeviceIncludingExternal(targetDeviceName);

            // yDebug() << "Attach device" << device.name() << "to" << targetDevice.name() << "as" << targetNetwork;
            drivers.push(targetDevice.poly, targetNetwork.c_str());
        }
    } else {
        yError() << "Action \"" << ActionTypeToString(ActionTypeAttach) << R"(" requires either "network" or "networks" parameter)";
        return false;
    }

    if (!drivers.size()) {
        yError() << "Action \"" << ActionTypeToString(ActionTypeAttach) << "\" couldn't find any device.";
        return false;
    }

    return device.attach(drivers);
}

bool yarp::robotinterface::experimental::Robot::Private::abort(const yarp::robotinterface::experimental::Device& device, const yarp::robotinterface::experimental::ParamList& params)
{
    YARP_FIXME_NOTIMPLEMENTED("abort action")
    return true;
}


bool yarp::robotinterface::experimental::Robot::Private::detach(const yarp::robotinterface::experimental::Device& device, const yarp::robotinterface::experimental::ParamList& params)
{

    if (!params.empty()) {
        yWarning() << "Action \"" << ActionTypeToString(ActionTypeDetach) << "\" cannot have any parameter. Ignoring them.";
    }

    return device.detach();
}

bool yarp::robotinterface::experimental::Robot::Private::park(const yarp::robotinterface::experimental::Device& device,
                                                              const yarp::robotinterface::experimental::ParamList& params)
{
    if (!yarp::robotinterface::experimental::hasParam(params, "target")) {
        yError() << "Action \"" << ActionTypeToString(ActionTypePark) << R"(" requires "target" parameter)";
        return false;
    }
    std::string targetDeviceName = yarp::robotinterface::experimental::findParam(params, "target");

    if (!hasDeviceIncludingExternal(targetDeviceName)) {
        yError() << "Target device" << targetDeviceName << "does not exist.";
        return false;
    }
    yarp::dev::PolyDriverDescriptor targetDevice = findDeviceIncludingExternal(targetDeviceName);

    return device.park(targetDevice);
}

bool yarp::robotinterface::experimental::Robot::Private::custom(const yarp::robotinterface::experimental::Device& device, const yarp::robotinterface::experimental::ParamList& params)
{
    YARP_FIXME_NOTIMPLEMENTED("custom action")
    return true;
}

yarp::robotinterface::experimental::Robot::Robot() :
        mPriv(new Private(this))
{
}

yarp::robotinterface::experimental::Robot::Robot(const std::string& name, const yarp::robotinterface::experimental::DeviceList& devices) :
        mPriv(new Private(this))
{
    mPriv->name = name;
    mPriv->devices = devices;
}

yarp::robotinterface::experimental::Robot::Robot(const yarp::robotinterface::experimental::Robot& other) :
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

yarp::robotinterface::experimental::Robot& yarp::robotinterface::experimental::Robot::operator=(const yarp::robotinterface::experimental::Robot& other)
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

yarp::robotinterface::experimental::Robot::~Robot()
{
    delete mPriv;
}

std::string& yarp::robotinterface::experimental::Robot::name()
{
    return mPriv->name;
}

unsigned int& yarp::robotinterface::experimental::Robot::build()
{
    return mPriv->build;
}

std::string& yarp::robotinterface::experimental::Robot::portprefix()
{
    return mPriv->portprefix;
}

void yarp::robotinterface::experimental::Robot::setVerbose(bool verbose)
{
    for (auto& device : devices()) {
        ParamList& params = device.params();
        // Do not override "verbose" param if explicitly set in the xml
        if (verbose && !yarp::robotinterface::experimental::hasParam(params, "verbose")) {
            device.params().push_back(Param("verbose", "1"));
        }
    }
}

void yarp::robotinterface::experimental::Robot::setAllowDeprecatedDevices(bool allowDeprecatedDevices)
{
    for (auto& device : devices()) {
        ParamList& params = device.params();
        // Do not override "allow-deprecated-devices" param if explicitly set in the xml
        if (allowDeprecatedDevices && !yarp::robotinterface::experimental::hasParam(params, "allow-deprecated-devices")) {
            device.params().push_back(Param("allow-deprecated-devices", "1"));
        }
    }
}

yarp::robotinterface::experimental::ParamList& yarp::robotinterface::experimental::Robot::params()
{
    return mPriv->params;
}

yarp::robotinterface::experimental::DeviceList& yarp::robotinterface::experimental::Robot::devices()
{
    return mPriv->devices;
}

yarp::robotinterface::experimental::Device& yarp::robotinterface::experimental::Robot::device(const std::string& name)
{
    return *mPriv->findDevice(name);
}

const std::string& yarp::robotinterface::experimental::Robot::name() const
{
    return mPriv->name;
}

const unsigned int& yarp::robotinterface::experimental::Robot::build() const
{
    return mPriv->build;
}

const std::string& yarp::robotinterface::experimental::Robot::portprefix() const
{
    return mPriv->portprefix;
}

const yarp::robotinterface::experimental::ParamList& yarp::robotinterface::experimental::Robot::params() const
{
    return mPriv->params;
}


const yarp::robotinterface::experimental::DeviceList& yarp::robotinterface::experimental::Robot::devices() const
{
    return mPriv->devices;
}

bool yarp::robotinterface::experimental::Robot::hasDevice(const std::string& name) const
{
    return mPriv->hasDevice(name);
}

const yarp::robotinterface::experimental::Device& yarp::robotinterface::experimental::Robot::device(const std::string& name) const
{
    return *mPriv->findDevice(name);
}

void yarp::robotinterface::experimental::Robot::interrupt()
{
    yInfo() << "Interrupt received. Stopping all running threads.";

    // If we received an interrupt we send a stop signal to all threads
    // from previous phases
    for (auto& device : devices()) {
        device.stopThreads();
    }
}

bool yarp::robotinterface::experimental::Robot::setExternalDevices(const yarp::dev::PolyDriverList& list)
{
    bool nameConflict = mPriv->checkForNamingConflictsInExternalDevices(list);
    if (nameConflict) {
        return false;
    }

    mPriv->externalDevices = list;
    return true;
}

bool yarp::robotinterface::experimental::Robot::enterPhase(yarp::robotinterface::experimental::ActionPhase phase)
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
        if (mPriv->getLevels(phase).size() != 0) {
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
        if (mPriv->currentPhase != phase) {
            ret = false;
            break;
        }

        std::vector<std::pair<Device, Action>> actions = mPriv->getActions(phase, level);

        for (auto& ait : actions) {
            // for each action in that level
            Device& device = ait.first;
            Action& action = ait.second;

            // If current phase was changed by some other thread, we should
            // exit the loop and avoid starting further actions.
            if (mPriv->currentPhase != phase) {
                ret = false;
                break;
            }

            switch (action.type()) {
            case ActionTypeConfigure:
                if (!mPriv->configure(device, action.params())) {
                    yError() << "Cannot run configure action on device" << device.name();
                    ret = false;
                }
                break;
            case ActionTypeCalibrate:
                if (!mPriv->calibrate(device, action.params())) {
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
                if (!mPriv->abort(device, action.params())) {
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

yarp::robotinterface::experimental::ActionPhase yarp::robotinterface::experimental::Robot::currentPhase() const
{
    return mPriv->currentPhase;
}

int yarp::robotinterface::experimental::Robot::currentLevel() const
{
    return mPriv->currentLevel;
}

bool yarp::robotinterface::experimental::Robot::hasParam(const std::string& name) const
{
    return yarp::robotinterface::experimental::hasParam(mPriv->params, name);
}

std::string yarp::robotinterface::experimental::Robot::findParam(const std::string& name) const
{
    return yarp::robotinterface::experimental::findParam(mPriv->params, name);
}
