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
#include <yarp/dev/PolyDriver.h>

#include "RobotInterface.h"
#include "Debug.h"


namespace {

inline static RobotInterface::ActionPhase StringToActionPhase(const std::string &phase)
{
    if (!phase.compare("startup")) {
        return RobotInterface::ActionPhaseStartup;
    } else if (!phase.compare("interrupt")) {
        return RobotInterface::ActionPhaseInterrupt;
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
    case RobotInterface::ActionPhaseInterrupt:
        return std::string("interrupt");
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

inline static const std::string& findParam(const RobotInterface::ParamList &list, const std::string& name)
{
    for (RobotInterface::ParamList::const_iterator it = list.begin(); it != list.end(); it++) {
        const RobotInterface::Param &param = *it;
        if (!name.compare(param.name())) {
            return param.value();
        }
    }
    warning() << "Param" << name << "not found";
    return std::string();
}

} // namespace

std::ostringstream& operator<<(std::ostringstream &oss, const RobotInterface::Param &t)
{
    oss << "(\"" << t.name() << "\" = \"" << t.value() << "\")";
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
};

RobotInterface::Debug operator<<(RobotInterface::Debug dbg, const RobotInterface::Param &t)
{
    std::ostringstream oss;
    oss << t;
    dbg << oss.str();
    return dbg;
}

RobotInterface::Param::Param() :
    mPriv(new Private(this))
{
}

RobotInterface::Param::Param(const std::string &name, const std::string &value) :
    mPriv(new Private(this))
{
    mPriv->name = name;
    mPriv->value = value;
}

RobotInterface::Param::Param(const ::RobotInterface::Param& other) :
    mPriv(new Private(this))
{
    mPriv->name = other.mPriv->name;
    mPriv->value = other.mPriv->value;
}

RobotInterface::Param& RobotInterface::Param::operator=(const RobotInterface::Param& other)
{
    if (&other != this) {
        mPriv->name = other.mPriv->name;
        mPriv->value = other.mPriv->value;
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

RobotInterface::Debug operator<<(RobotInterface::Debug dbg, const RobotInterface::Action &t)
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
    mPriv->type = StringToActionType(phase);
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

const std::string& RobotInterface::Action::findParam(const std::string& name) const
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
        error() << "FIXME: not implemented:" << __PRETTY_FUNCTION__;
    }

    ~Private()
    {
        if (!--driver->ref) {
            if (driver->driver->isValid()) {
                if (!driver->driver->close()) {
                    warning() << "Cannot close device" << name;
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
        std::string s;
        s += "(device " + type + ")";
        for (RobotInterface::ParamList::const_iterator it = params.begin(); it != params.end(); it++) {
            const RobotInterface::Param &param = *it;
            s += " (" + param.name() + " " + param.value() + ")";
        }
        yarp::os::Property p(s.c_str());

        debug() << "   -->" << "\033[01;32m" << s << "\033[00m";
        debug() << "   --->" << "\033[01;35m" << p.toString() << "\033[00m";
        return p;
    }

    std::string name;
    std::string type;
    ParamList params;
    ActionList actions;
    Driver *driver;
};

RobotInterface::Debug operator<<(RobotInterface::Debug dbg, const RobotInterface::Device &t)
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

        error() << "FIXME: not implemented:" << __PRETTY_FUNCTION__;
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
        error() << "Trying to create a device already opened device.";
        return false;
    }

    if (!mPriv->open()) {
        warning() << "Cannot open device" << mPriv->name;
        return false;
    }

    return true;
}

bool RobotInterface::Device::close()
{
    // TODO implement me
}

bool RobotInterface::Device::hasParam(const std::string& name) const
{
    return ::hasParam(mPriv->params, name);
}

const std::string& RobotInterface::Device::findParam(const std::string& name) const
{
    return ::findParam(mPriv->params, name);
}

yarp::dev::PolyDriver* RobotInterface::Device::driver() const
{
    return mPriv->drv();
}


//END Device


//BEGIN Robot

class RobotInterface::Robot::Private
{
public:
    Private(Robot * /*parent*/) {}

    // return the device with the given name or an invalid device if not found
    Device& findDevice(const std::string &name);

    // open all the devices and return true if all the open calls were succesful
    bool openDevices();

    // return a vector of levels that have actions in the requested phase
    std::vector<unsigned int> getLevels(ActionPhase phase) const;

    // return a vector of actions for that phase and that level
    std::vector<std::pair<Device, Action> > getActions(ActionPhase phase, unsigned int level) const;

    std::string name;
    ParamList params;
    DeviceList devices;
};

RobotInterface::Device& RobotInterface::Robot::Private::findDevice(const std::string &name)
{
    for (DeviceList::iterator it = devices.begin(); it != devices.end(); it++) {
        if (!name.compare(it->name())) {
            return *it;
        }
    }
    fatal() << "Cannot find device" << name;
}

bool RobotInterface::Robot::Private::openDevices()
{
    bool ret = true;
    for (RobotInterface::DeviceList::iterator it = devices.begin(); it != devices.end(); it++) {
        RobotInterface::Device &device = *it;

        debug() << device;

        if (!device.open()) {
            warning() << "Cannot open device" << device.name();
            ret = false;
        }

    }
    if (ret) {
        debug() << "All devices opened.";
    } else {
        warning() << "There was some problem opening one or more devices. Please check the log and your configuration";
    }

    return ret;
}

std::vector<unsigned int> RobotInterface::Robot::Private::getLevels(RobotInterface::ActionPhase phase) const
{
    std::vector<unsigned int> levels;
    for (DeviceList::const_iterator it = devices.begin(); it != devices.end(); it++) {
        const Device &device = *it;
        if (device.actions().empty()) {
            continue;
        }

        for (ActionList::const_iterator it = device.actions().begin(); it != device.actions().end(); it++) {
            const Action &action = *it;
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
    for (DeviceList::const_iterator it = devices.begin(); it != devices.end(); it++) {
        const Device &device = *it;
        if (device.actions().empty()) {
            continue;
        }

        for (ActionList::const_iterator it = device.actions().begin(); it != device.actions().end(); it++) {
            const Action &action = *it;
            if (action.phase() == phase && action.level() == level) {
                actions.push_back(std::make_pair<RobotInterface::Device, RobotInterface::Action>(device, action));
            }
        }
    }
    return actions;
}


RobotInterface::Debug operator<<(RobotInterface::Debug dbg, const RobotInterface::Robot &t)
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
    return mPriv->findDevice(name);
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
    return mPriv->findDevice(name);
}

bool RobotInterface::Robot::hasParam(const std::string& name) const
{
    return ::hasParam(mPriv->params, name);
}

const std::string& RobotInterface::Robot::findParam(const std::string& name) const
{
    return ::findParam(mPriv->params, name);
}



//END Robot

#if 0
class RobotInterface::RobotInterface::Private
{
public:
    Private(RobotInterface *parent);
    ~Private();

    RobotInterface * const parent;
};


RobotInterface::RobotInterface::Private::Private(RobotInterface *parent) :
    parent(parent)
{

}


RobotInterface::RobotInterface::Private::~Private()
{

}

RobotInterface::RobotInterface::RobotInterface() :
    mPriv(new Private(this))
{

}

RobotInterface::RobotInterface::~RobotInterface()
{
    delete mPriv;
}
#endif
