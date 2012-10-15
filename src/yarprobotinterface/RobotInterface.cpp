/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>

#include <yarp/os/Property.h>
#include <yarp/os/Value.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Wrapper.h>
#include <yarp/dev/CalibratorInterfaces.h>
#include <yarp/dev/ControlBoardInterfaces.h>

#include "RobotInterface.h"
#include <Debug.h>


namespace {

inline static RobotInterface::ActionPhase StringToActionPhase(const std::string &phase)
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

inline static std::string ActionPhaseToString(RobotInterface::ActionPhase actionphase)
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

inline static RobotInterface::ActionType StringToActionType(const std::string &type)
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

inline static std::string ActionTypeToString(RobotInterface::ActionType actiontype)
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

inline static bool hasParam(const RobotInterface::ParamList &list, const std::string& name)
{
    for (RobotInterface::ParamList::const_iterator it = list.begin(); it != list.end(); it++) {
        const RobotInterface::Param &param = *it;
        if (!name.compare(param.name())) {
            return true;
        }
    }
    return false;
}

inline static std::string findParam(const RobotInterface::ParamList &list, const std::string& name)
{
    for (RobotInterface::ParamList::const_iterator it = list.begin(); it != list.end(); it++) {
        const RobotInterface::Param &param = *it;
        if (!name.compare(param.name())) {
            return param.value();
        }
    }
    yError() << "Param" << name << "not found";
    return std::string();
}

inline static bool hasGroup(const RobotInterface::ParamList &list, const std::string& name)
{
    for (RobotInterface::ParamList::const_iterator it = list.begin(); it != list.end(); it++) {
        const RobotInterface::Param &param = *it;
        if (param.isGroup() && !name.compare(param.name())) {
            return true;
        }
    }
    return false;
}

inline static std::string findGroup(const RobotInterface::ParamList &list, const std::string& name)
{
    for (RobotInterface::ParamList::const_iterator it = list.begin(); it != list.end(); it++) {
        const RobotInterface::Param &param = *it;
        if (param.isGroup() && !name.compare(param.name())) {
            return param.value();
        }
    }
    yError() << "Param" << name << "not found";
    return std::string();
}

inline static RobotInterface::ParamList mergeDuplicateGroups(const RobotInterface::ParamList &list)
{
    RobotInterface::ParamList params = list;
    for (RobotInterface::ParamList::iterator it1 = params.begin(); it1 != params.end(); it1++) {
        RobotInterface::Param &param1 = *it1;
        for (RobotInterface::ParamList::iterator it2 = it1 + 1; it2 != params.end(); it2++) {
            RobotInterface::Param &param2 = *it2;
            if (param1.name().compare(param2.name()) == 0) {
                if (!param1.isGroup() || !param2.isGroup()) {
                    yFatal() << "Duplicate parameter \"" << param1.name() << "\" found and at least one of them is not a group.";
                }
                param1.value() += std::string(" ");
                param1.value() += param2.value();
                params.erase(it2);
            }
        }
    }
    return params;
}

} // namespace

std::ostringstream& operator<<(std::ostringstream &oss, const RobotInterface::Param &t)
{
    oss << "(\"" << t.name() << "\"" << (t.isGroup() ? " [group]" : "") << " = \"" << t.value() << "\")";
    return oss;
}

std::ostringstream& operator<<(std::ostringstream &oss, const RobotInterface::Action &t)
{
    oss << "(\"" << ActionPhaseToString(t.phase()) << ":" << ActionTypeToString(t.type()) << ":" << t.level() << "\"";
    if (!t.params().empty()) {
        oss << ", params = [";
        oss << t.params();
        oss << "]";
    }
    oss << ")";
    return oss;
}

std::ostringstream& operator<<(std::ostringstream &oss, const RobotInterface::Device &t)
{
    oss << "(name = \"" << t.name() << "\", type = \"" << t.type() << "\"";
    if (!t.params().empty()) {
        oss << ", params = [";
        oss << t.params();
        oss << "]";
    }
    if (!t.actions().empty()) {
        oss << ", actions = [";
        oss << t.actions();
        oss << "]";
    }
    oss << ")";
    return oss;
}

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


//BEGIN Param

class RobotInterface::Param::Private
{
public:
    Private(Param * /*parent*/) {}

    std::string name;
    std::string value;
    bool isGroup;
};

DebugStream::Debug operator<<(DebugStream::Debug dbg, const RobotInterface::Param &t)
{
    std::ostringstream oss;
    oss << t;
    dbg << oss.str();
    return dbg;
}

RobotInterface::Param::Param(bool isGroup) :
    mPriv(new Private(this))
{
    mPriv->isGroup = isGroup;
}

RobotInterface::Param::Param(const std::string &name, const std::string &value, bool isGroup) :
    mPriv(new Private(this))
{
    mPriv->name = name;
    mPriv->value = value;
    mPriv->isGroup = isGroup;
}

RobotInterface::Param::Param(const ::RobotInterface::Param& other) :
    mPriv(new Private(this))
{
    mPriv->name = other.mPriv->name;
    mPriv->value = other.mPriv->value;
    mPriv->isGroup = other.mPriv->isGroup;
}

RobotInterface::Param& RobotInterface::Param::operator=(const RobotInterface::Param& other)
{
    if (&other != this) {
        mPriv->name = other.mPriv->name;
        mPriv->value = other.mPriv->value;
        mPriv->isGroup = other.mPriv->isGroup;
    }

    return *this;
}

RobotInterface::Param::~Param()
{
    delete mPriv;
}

std::string& RobotInterface::Param::name()
{
    return mPriv->name;
}

std::string& RobotInterface::Param::value()
{
    return mPriv->value;
}

const std::string& RobotInterface::Param::name() const
{
    return mPriv->name;
}

const std::string& RobotInterface::Param::value() const
{
    return mPriv->value;
}

bool RobotInterface::Param::isGroup() const
{
    return mPriv->isGroup;
}

yarp::os::Property RobotInterface::Param::toProperty() const
{
    yarp::os::Property p;
    p.put(mPriv->name.c_str(), mPriv->value.c_str());
    return p;
}

//END Param


//BEGIN Action

class RobotInterface::Action::Private
{
public:
    Private(Action * /*parent*/) {}

    ActionPhase phase;
    ActionType type;
    unsigned int level;
    ParamList params;
};

void operator>>(const std::stringstream &sstream, RobotInterface::ActionPhase &actionphase)
{
    actionphase = StringToActionPhase(sstream.str());
}

void operator>>(const std::stringstream &sstream, RobotInterface::ActionType &actiontype)
{
    actiontype = StringToActionType(sstream.str());
}

DebugStream::Debug operator<<(DebugStream::Debug dbg, const RobotInterface::Action &t)
{
    std::ostringstream oss;
    oss << t;
    dbg << oss.str();
    return dbg;
}

RobotInterface::Action::Action() :
    mPriv(new Private(this))
{
}

RobotInterface::Action::Action(const std::string& phase, const std::string& type, unsigned int level) :
    mPriv(new Private(this))
{
    mPriv->phase = StringToActionPhase(phase);
    mPriv->type = StringToActionType(type);
    mPriv->level = level;
}

RobotInterface::Action::Action(RobotInterface::ActionPhase phase, RobotInterface::ActionType type, unsigned int level) :
    mPriv(new Private(this))
{
    mPriv->phase = phase;
    mPriv->type = type;
    mPriv->level = level;
}

RobotInterface::Action::Action(const RobotInterface::Action& other) :
    mPriv(new Private(this))
{
    mPriv->phase = other.mPriv->phase;
    mPriv->type = other.mPriv->type;
    mPriv->level = other.mPriv->level;
    mPriv->params = other.mPriv->params;
}

RobotInterface::Action& RobotInterface::Action::operator=(const RobotInterface::Action& other)
{
    if (&other != this) {
        mPriv->phase = other.mPriv->phase;
        mPriv->type = other.mPriv->type;
        mPriv->level = other.mPriv->level;

        mPriv->params.clear();
        mPriv->params = other.mPriv->params;
    }

    return *this;
}

RobotInterface::Action::~Action()
{
    delete mPriv;
}

RobotInterface::ActionPhase& RobotInterface::Action::phase()
{
    return mPriv->phase;
}

RobotInterface::ActionType& RobotInterface::Action::type()
{
    return mPriv->type;
}

unsigned int& RobotInterface::Action::level()
{
    return mPriv->level;
}

RobotInterface::ParamList& RobotInterface::Action::params()
{
    return mPriv->params;
}

RobotInterface::ActionPhase RobotInterface::Action::phase() const
{
    return mPriv->phase;
}

RobotInterface::ActionType RobotInterface::Action::type() const
{
    return mPriv->type;
}

unsigned int RobotInterface::Action::level() const
{
    return mPriv->level;
}

const RobotInterface::ParamList& RobotInterface::Action::params() const
{
    return mPriv->params;
}

bool RobotInterface::Action::hasParam(const std::string& name) const
{
    return ::hasParam(mPriv->params, name);
}

std::string RobotInterface::Action::findParam(const std::string& name) const
{
    return ::findParam(mPriv->params, name);
}

//END Action


//BEGIN Device

class RobotInterface::Device::Private
{
public:
    struct Driver {
        Driver(yarp::dev::PolyDriver *d) : driver(d), ref(1) {}
        yarp::dev::PolyDriver *driver;
        int ref;
    };

    Private(Device * /*parent*/) :
        driver(new Driver(new yarp::dev::PolyDriver()))
    {
    }

    Private(const Private &other) : driver(other.driver)
    {
        ++driver->ref;
    }

    Private& operator=(const Private& other)
    {
        (void)other; // UNUSED
        YFIXME_NOTIMPLEMENTED
        return *this;
    }

    ~Private()
    {
        if (!--driver->ref) {
            if (driver->driver->isValid()) {
                if (!driver->driver->close()) {
                    yWarning() << "Cannot close device" << name;
                }
            }
            delete driver;
            driver = NULL;
        }
    }

    inline yarp::dev::PolyDriver* drv() const { return driver->driver; }

    inline bool isValid() const { return drv()->isValid(); }
    inline bool open() { return drv()->open(paramsAsProperty().toString()); }
    inline bool close() { return drv()->close(); }

    yarp::os::Property paramsAsProperty() const
    {
        ParamList p = ::mergeDuplicateGroups(params);
        std::string s;
        s += "(device " + type + ")";
        for (RobotInterface::ParamList::const_iterator it = p.begin(); it != p.end(); it++) {
            const RobotInterface::Param &param = *it;
            s += " (" + param.name() + " " + param.value() + ")";
        }
        return yarp::os::Property(s.c_str());
    }

    std::string name;
    std::string type;
    ParamList params;
    ActionList actions;
    Driver *driver;
};

DebugStream::Debug operator<<(DebugStream::Debug dbg, const RobotInterface::Device &t)
{
    std::ostringstream oss;
    oss << t;
    dbg << oss.str();
    return dbg;
}

RobotInterface::Device::Device() :
    mPriv(new Private(this))
{
    mPriv->close();
}

RobotInterface::Device::Device(const std::string& name,
                               const std::string& type,
                               const RobotInterface::ParamList& params,
                               const RobotInterface::ActionList& actions) :
    mPriv(new Private(this))
{
    mPriv->name = name;
    mPriv->type = type;
    mPriv->params = params;
    mPriv->actions = actions;
}

RobotInterface::Device::Device(const RobotInterface::Device& other) :
    mPriv(new Private(*other.mPriv))
{
    mPriv->name = other.mPriv->name;
    mPriv->type = other.mPriv->type;
    mPriv->params = other.mPriv->params;
    mPriv->actions = other.mPriv->actions;
    *mPriv->driver = *other.mPriv->driver;
}

RobotInterface::Device& RobotInterface::Device::operator=(const RobotInterface::Device& other)
{
    if (&other != this) {
        mPriv->name = other.mPriv->name;
        mPriv->type = other.mPriv->type;

        mPriv->params.clear();
        mPriv->params = other.mPriv->params;

        mPriv->actions.clear();
        mPriv->actions = other.mPriv->actions;

        *mPriv->driver = *other.mPriv->driver;
    }
    return *this;
}

RobotInterface::Device::~Device()
{
    delete mPriv;
}

std::string& RobotInterface::Device::name()
{
    return mPriv->name;
}

std::string& RobotInterface::Device::type()
{
    return mPriv->type;
}

RobotInterface::ParamList& RobotInterface::Device::params()
{
    return mPriv->params;
}

RobotInterface::ActionList& RobotInterface::Device::actions()
{
    return mPriv->actions;
}

const std::string& RobotInterface::Device::name() const
{
    return mPriv->name;
}

const std::string& RobotInterface::Device::type() const
{
    return mPriv->type;
}

const RobotInterface::ParamList& RobotInterface::Device::params() const
{
    return mPriv->params;
}

const RobotInterface::ActionList& RobotInterface::Device::actions() const
{
    return mPriv->actions;
}

bool RobotInterface::Device::open()
{
    if (mPriv->isValid()) {
        yError() << "Trying to open an already opened device.";
        return false;
    }

    if (!mPriv->open()) {
        yWarning() << "Cannot open device" << mPriv->name;
        return false;
    }

    return true;
}

bool RobotInterface::Device::close()
{
    if (!mPriv->isValid()) {
        yError() << "Trying to close an already closed device.";
        return false;
    }

    if (!mPriv->close()) {
        yWarning() << "Cannot close device" << mPriv->name;
        return false;
    }

    return true;
}

bool RobotInterface::Device::hasParam(const std::string& name) const
{
    return ::hasParam(mPriv->params, name);
}

std::string RobotInterface::Device::findParam(const std::string& name) const
{
    return ::findParam(mPriv->params, name);
}

yarp::dev::PolyDriver* RobotInterface::Device::driver() const
{
    return mPriv->drv();
}


//END Device


class ParallelCalibrator: public yarp::os::Thread
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
    yarp::os::Thread* calibrate(const Device &device, const ParamList &params);

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

yarp::os::Thread* RobotInterface::Robot::Private::calibrate(const RobotInterface::Device &device, const RobotInterface::ParamList &params)
{
    yarp::dev::ICalibrator *calibrator;
    if (!device.driver()->view(calibrator)) {
        yError() << device.name() << "is not a calibrator, therefore it cannot have" << ActionTypeToString(ActionTypeCalibrate) << "actions";
        return NULL;
    }

    if (!::hasParam(params, "target")) {
        yError() << "Action \"" << ActionTypeToString(ActionTypeCalibrate) << "\" requires \"target\" parameter";
        return NULL;
    }
    std::string targetDeviceName = ::findParam(params, "target");

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

    if (!(::hasParam(params, "network") || ::hasParam(params, "networks"))) {
        yError() << "Action \"" << ActionTypeToString(ActionTypeAttach) << "\" requires either \"network\" or \"networks\" parameter";
        return false;
    }

    if (::hasParam(params, "network") && ::hasParam(params, "networks")) {
        yError() << "Action \"" << ActionTypeToString(ActionTypeAttach) << "\" cannot have both \"network\" and \"networks\" parameters";
        return false;
    }

    yarp::dev::PolyDriverList drivers;

    if (::hasParam(params, "network")) {
        std::string targetNetwork = ::findParam(params, "network");

        if (!::hasParam(params, "device")) {
            yError() << "Action \"" << ActionTypeToString(ActionTypeAttach) << "\" requires \"device\" parameter";
            return false;
        }
        std::string targetDeviceName = ::findParam(params, "device");

        if (!hasDevice(targetDeviceName)) {
            yError() << "Target device" << targetDeviceName << "(network =" << targetNetwork << ") does not exist.";
            return false;
        }
        Device &targetDevice = *findDevice(targetDeviceName);

        yDebug() << "Attach device" << device.name() << "to" << targetDevice.name() << "as" << targetNetwork;
        drivers.push(targetDevice.driver(), targetNetwork.c_str());

    } else {
        yarp::os::Value v;
        v.fromString(::findParam(params, "networks").c_str());
        yarp::os::Bottle &targetNetworks = *(v.asList());

        for (int i = 0; i < targetNetworks.size(); i++) {
            std::string targetNetwork = targetNetworks.get(i).toString().c_str();

            if (!::hasParam(params, targetNetwork)) {
                yError() << "Action \"" << ActionTypeToString(ActionTypeAttach) << "\" requires one parameter per network. \"" << targetNetwork << "\" parameter is missing.";
                return false;
            }
            std::string targetDeviceName = ::findParam(params, targetNetwork);
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
        std::vector<yarp::os::Thread*> threads;

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
                yarp::os::Thread *calibratorThread = mPriv->calibrate(device, action.params());
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
        for (std::vector<yarp::os::Thread*>::iterator tit = threads.begin(); tit != threads.end(); tit++) {
            yarp::os::Thread* thread = *tit;
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
    return ::hasParam(mPriv->params, name);
}

std::string RobotInterface::Robot::findParam(const std::string& name) const
{
    return ::findParam(mPriv->params, name);
}



//END Robot
