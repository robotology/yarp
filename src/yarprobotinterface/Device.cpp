/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "Device.h"
#include "Action.h"
#include "Param.h"

#include <debugStream/Debug.h>

#include <yarp/os/Property.h>
#include <yarp/dev/PolyDriver.h>

#include <string>


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
        ParamList p = RobotInterface::mergeDuplicateGroups(params);
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
    return RobotInterface::hasParam(mPriv->params, name);
}

std::string RobotInterface::Device::findParam(const std::string& name) const
{
    return RobotInterface::findParam(mPriv->params, name);
}

yarp::dev::PolyDriver* RobotInterface::Device::driver() const
{
    return mPriv->drv();
}


//END Device