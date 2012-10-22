/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "Param.h"

#include <debugStream/Debug.h>

#include <yarp/os/Property.h>

#include <string>


std::ostringstream& operator<<(std::ostringstream &oss, const RobotInterface::Param &t)
{
    oss << "(\"" << t.name() << "\"" << (t.isGroup() ? " [group]" : "") << " = \"" << t.value() << "\")";
    return oss;
}


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
