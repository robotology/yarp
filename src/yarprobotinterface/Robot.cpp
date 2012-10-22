/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "Robot.h"
#include "Action.h"
#include "Device.h"
#include "Param.h"

#include <debugStream/Debug.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Wrapper.h>
#include <yarp/dev/CalibratorInterfaces.h>
#include <yarp/dev/ControlBoardInterfaces.h>

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





#include "Thread.h"

namespace RobotInterface {
class ParallelCalibrator;
} // namespace RobotInterface




class RobotInterface::ParallelCalibrator: public RobotInterface::Thread
{
public:
    ParallelCalibrator(yarp::dev::ICalibrator *calibrator, yarp::dev::DeviceDriver *target) :
        calibrator(calibrator),
        target(target)
    {
    }

    void run()
    {
        yDebug() << "Starting calibration";
        if (calibrator && target) {
            calibrator->calibrate(target);
            yDebug() << "Calibration done";
        } else {
            yError() << "Skipping, calibrator or target not set";
        }
    }

    void abort()
    {
        yDebug() << "Killing calibration";
        calibrator->quitCalibrate();
    }

private:
    yarp::dev::ICalibrator *calibrator;
    yarp::dev::DeviceDriver *target;
};


//BEGIN Robot

class RobotInterface::Robot::Private
{
public:
    Private(Robot * /*parent*/) {}

    // return true if a device with the given name exists
    bool hasDevice(const std::string &name) const;

    // return the device with the given name or <fatal error> if not found
    Device* findDevice(const std::string &name);

    // open all the devices and return true if all the open calls were succesful
    bool openDevices();

    // close all the devices and return true if all the close calls were succesful
    bool closeDevices();

    // return a vector of levels that have actions in the requested phase
    std::vector<unsigned int> getLevels(ActionPhase phase) const;

    // return a vector of actions for that phase and that level
    std::vector<std::pair<Device, Action> > getActions(ActionPhase phase, unsigned int level) const;


    // run configure action on one device
    bool configure(const Device &device, const ParamList &params);

    // run calibrate action on one device
    RobotInterface::Thread* calibrate(const Device &device, const ParamList &params);

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
    ParamList params;
    DeviceList devices;
};

bool RobotInterface::Robot::Private::hasDevice(const std::string &name) const
{
    for (DeviceList::const_iterator it = devices.begin(); it != devices.end(); it++) {
        if (!name.compare(it->name())) {
            return true;
        }
    }
    return false;
}

RobotInterface::Device* RobotInterface::Robot::Private::findDevice(const std::string &name)
{
    for (DeviceList::iterator it = devices.begin(); it != devices.end(); it++) {
        if (!name.compare(it->name())) {
            return &(*it);
        }
    }
    yFatal() << "Cannot find device" << name;
    return NULL;
}

bool RobotInterface::Robot::Private::openDevices()
{
    bool ret = true;
    for (RobotInterface::DeviceList::iterator it = devices.begin(); it != devices.end(); it++) {
        RobotInterface::Device &device = *it;

        yDebug() << device;

        if (!device.open()) {
            yWarning() << "Cannot open device" << device.name();
            ret = false;
        }

    }
    if (ret) {
        yDebug() << "All devices opened.";
    } else {
        yWarning() << "There was some problem opening one or more devices. Please check the log and your configuration";
    }

    return ret;
}

bool RobotInterface::Robot::Private::closeDevices()
{
    bool ret = true;
    for (RobotInterface::DeviceList::iterator it = devices.begin(); it != devices.end(); it++) {
        RobotInterface::Device &device = *it;

        yDebug() << device;

        if (!device.close()) {
            yWarning() << "Cannot close device" << device.name();
            ret = false;
        }

    }
    if (ret) {
        yDebug() << "All devices closed.";
    } else {
        yWarning() << "There was some problem closing one or more devices. Please check the log and your configuration";
    }

    return ret;
}

std::vector<unsigned int> RobotInterface::Robot::Private::getLevels(RobotInterface::ActionPhase phase) const
{
    std::vector<unsigned int> levels;
    for (DeviceList::const_iterator dit = devices.begin(); dit != devices.end(); dit++) {
        const Device &device = *dit;
        if (device.actions().empty()) {
            continue;
        }

        for (ActionList::const_iterator ait = device.actions().begin(); ait != device.actions().end(); ait++) {
            const Action &action = *ait;
            if (action.phase() == phase) {
                levels.push_back(action.level());
            }
        }
    }

    std::sort(levels.begin(), levels.end());

    return levels;
}


std::vector<std::pair<RobotInterface::Device, RobotInterface::Action> > RobotInterface::Robot::Private::getActions(RobotInterface::ActionPhase phase, unsigned int level) const
{
    std::vector<std::pair<RobotInterface::Device, RobotInterface::Action> > actions;
    for (DeviceList::const_iterator dit = devices.begin(); dit != devices.end(); dit++) {
        const Device &device = *dit;
        if (device.actions().empty()) {
            continue;
        }

        for (ActionList::const_iterator ait = device.actions().begin(); ait != device.actions().end(); ait++) {
            const Action &action = *ait;
            if (action.phase() == phase && action.level() == level) {
                actions.push_back(std::make_pair<RobotInterface::Device, RobotInterface::Action>(device, action));
            }
        }
    }
    return actions;
}


bool RobotInterface::Robot::Private::configure(const RobotInterface::Device &device, const RobotInterface::ParamList &params)
{
    YFIXME_NOTIMPLEMENTED
    return true;
}

RobotInterface::Thread* RobotInterface::Robot::Private::calibrate(const RobotInterface::Device &device, const RobotInterface::ParamList &params)
{
    yarp::dev::ICalibrator *calibrator;
    if (!device.driver()->view(calibrator)) {
        yError() << device.name() << "is not a calibrator, therefore it cannot have" << ActionTypeToString(ActionTypeCalibrate) << "actions";
        return NULL;
    }

    if (!RobotInterface::hasParam(params, "target")) {
        yError() << "Action \"" << ActionTypeToString(ActionTypeCalibrate) << "\" requires \"target\" parameter";
        return NULL;
    }
    std::string targetDeviceName = RobotInterface::findParam(params, "target");

    if (!hasDevice(targetDeviceName)) {
        yError() << "Target device" << targetDeviceName << "does not exist.";
        return NULL;
    }
    Device &targetDevice = *findDevice(targetDeviceName);

    yarp::dev::IControlCalibration2 *controlCalibrator;
    if (!targetDevice.driver()->view(controlCalibrator)) {
        yError() << targetDevice.name() << "is not a calibrator, therefore it cannot have" << ActionTypeToString(ActionTypeCalibrate) << "actions";
        return NULL;
    }

    controlCalibrator->setCalibrator(calibrator);

    return new ParallelCalibrator(calibrator, targetDevice.driver());
}

bool RobotInterface::Robot::Private::attach(const RobotInterface::Device &device, const RobotInterface::ParamList &params)
{
    yarp::dev::IMultipleWrapper *wrapper;
    if (!device.driver()->view(wrapper)) {
        yError() << device.name() << "is not a wrapper, therefore it cannot have" << ActionTypeToString(ActionTypeAttach) << "actions";
        return false;
    }

    if (!(RobotInterface::hasParam(params, "network") || RobotInterface::hasParam(params, "networks"))) {
        yError() << "Action \"" << ActionTypeToString(ActionTypeAttach) << "\" requires either \"network\" or \"networks\" parameter";
        return false;
    }

    if (RobotInterface::hasParam(params, "network") && RobotInterface::hasParam(params, "networks")) {
        yError() << "Action \"" << ActionTypeToString(ActionTypeAttach) << "\" cannot have both \"network\" and \"networks\" parameters";
        return false;
    }

    yarp::dev::PolyDriverList drivers;

    if (RobotInterface::hasParam(params, "network")) {
        std::string targetNetwork = RobotInterface::findParam(params, "network");

        if (!RobotInterface::hasParam(params, "device")) {
            yError() << "Action \"" << ActionTypeToString(ActionTypeAttach) << "\" requires \"device\" parameter";
            return false;
        }
        std::string targetDeviceName = RobotInterface::findParam(params, "device");

        if (!hasDevice(targetDeviceName)) {
            yError() << "Target device" << targetDeviceName << "(network =" << targetNetwork << ") does not exist.";
            return false;
        }
        Device &targetDevice = *findDevice(targetDeviceName);

        yDebug() << "Attach device" << device.name() << "to" << targetDevice.name() << "as" << targetNetwork;
        drivers.push(targetDevice.driver(), targetNetwork.c_str());

    } else {
        yarp::os::Value v;
        v.fromString(RobotInterface::findParam(params, "networks").c_str());
        yarp::os::Bottle &targetNetworks = *(v.asList());

        for (int i = 0; i < targetNetworks.size(); i++) {
            std::string targetNetwork = targetNetworks.get(i).toString().c_str();

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

            yDebug() << "Attach device" << device.name() << "to" << targetDevice.name() << "as" << targetNetwork;
            drivers.push(targetDevice.driver(), targetNetwork.c_str());
        }
    }

    if (!wrapper->attachAll(drivers)) {
        yError() << "Cannot execute" << ActionTypeToString(ActionTypeAttach) << "on device" << device.name();
        return false;
    }

    return true;
}

bool RobotInterface::Robot::Private::abort(const RobotInterface::Device &device, const RobotInterface::ParamList &params)
{
    YFIXME_NOTIMPLEMENTED
    return true;
}


bool RobotInterface::Robot::Private::detach(const RobotInterface::Device &device, const RobotInterface::ParamList &params)
{
    yarp::dev::IMultipleWrapper *wrapper;
    if (!device.driver()->view(wrapper)) {
        yError() << device.name() << "is not a wrapper, therefore it cannot have" << ActionTypeToString(ActionTypeDetach) << "actions";
        return false;
    }

    if (!params.empty()) {
        yWarning() << "Action \"" << ActionTypeToString(ActionTypeDetach) << "\" cannot have any parameter. Ignoring them.";
    }

    if (!wrapper->detachAll()) {
        yError() << "Cannot execute" << ActionTypeToString(ActionTypeDetach) << "on device" << device.name();
        return false;
    }

    return true;
}

bool RobotInterface::Robot::Private::park(const RobotInterface::Device &device, const RobotInterface::ParamList &params)
{
    YFIXME_NOTIMPLEMENTED
    return true;
}

bool RobotInterface::Robot::Private::custom(const RobotInterface::Device &device, const RobotInterface::ParamList &params)
{
    YFIXME_NOTIMPLEMENTED
    return true;
}

DebugStream::Debug operator<<(DebugStream::Debug dbg, const RobotInterface::Robot &t)
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
    mPriv->devices = other.mPriv->devices;
    mPriv->params = other.mPriv->params;
}

RobotInterface::Robot& RobotInterface::Robot::operator=(const RobotInterface::Robot& other)
{
    if (&other != this) {
        mPriv->name = other.mPriv->name;

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

bool RobotInterface::Robot::enterPhase(RobotInterface::ActionPhase phase)
{
    yDebug() << "Entering" << ActionPhaseToString(phase) << "phase";

    if (phase == ActionPhaseStartup) {
        mPriv->openDevices();
    }

    std::vector<unsigned int> levels = mPriv->getLevels(phase);

    bool ret = true;
    for (std::vector<unsigned int>::const_iterator lit = levels.begin(); lit != levels.end(); lit++) {
        // for each level
        const unsigned int level = *lit;
        std::vector<std::pair<Device, Action> > actions = mPriv->getActions(phase, level);
        std::vector<RobotInterface::Thread*> threads;

        for (std::vector<std::pair<Device, Action> >::iterator ait = actions.begin(); ait != actions.end(); ait++) {
            // for each action in that level
            Device &device = ait->first;
            Action &action = ait->second;

            switch (action.type()) {
            case ActionTypeConfigure:
                if(!mPriv->configure(device, action.params())) {
                    yError() << "Cannot run configure action on device" << device.name();
                    ret = false;
                }
                break;
            case ActionTypeCalibrate:
            {
                RobotInterface::Thread *calibratorThread = mPriv->calibrate(device, action.params());
                if(!calibratorThread) {
                    yError() << "Cannot run calibrate action on device" << device.name();
                    ret = false;
                } else {
                    threads.push_back(calibratorThread);
                    calibratorThread->start();
                }
                break;
            }
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

        // Join parallel threads
        for (std::vector<RobotInterface::Thread*>::iterator tit = threads.begin(); tit != threads.end(); tit++) {
            RobotInterface::Thread* thread = *tit;
            thread->stop();
        }
    }

    if (ret) {
        yDebug() << "All actions for phase" << ActionPhaseToString(phase) << "executed.";
    } else {
        yWarning() << "There was some problem running actions for phase" << ActionPhaseToString(phase) << ". Please check the log and your configuration";
    }

    if (phase == ActionPhaseShutdown) {
        mPriv->closeDevices();
    }

    return ret;
}


bool RobotInterface::Robot::hasParam(const std::string& name) const
{
    return RobotInterface::hasParam(mPriv->params, name);
}

std::string RobotInterface::Robot::findParam(const std::string& name) const
{
    return RobotInterface::findParam(mPriv->params, name);
}



//END Robot
