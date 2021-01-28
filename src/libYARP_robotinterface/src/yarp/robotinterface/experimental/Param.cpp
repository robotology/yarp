/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/robotinterface/experimental/Param.h>

#include <yarp/os/LogStream.h>
#include <yarp/os/Property.h>

#include <string>


std::ostream& std::operator<<(std::ostream& oss, const yarp::robotinterface::experimental::Param& t)
{
    oss << "(\"" << t.name() << "\"" << (t.isGroup() ? " [group]" : "") << " = \"" << t.value() << "\")";
    return oss;
}


class yarp::robotinterface::experimental::Param::Private
{
public:
    Private(Param* /*parent*/) :
            isGroup(false)
    {
    }

    std::string name;
    std::string value;
    bool isGroup;
};

yarp::os::LogStream operator<<(yarp::os::LogStream dbg, const yarp::robotinterface::experimental::Param& t)
{
    std::ostringstream oss;
    oss << t;
    dbg << oss.str();
    return dbg;
}

yarp::robotinterface::experimental::Param::Param(bool isGroup) :
        mPriv(new Private(this))
{
    mPriv->isGroup = isGroup;
}

yarp::robotinterface::experimental::Param::Param(const std::string& name, const std::string& value, bool isGroup) :
        mPriv(new Private(this))
{
    mPriv->name = name;
    mPriv->value = value;
    mPriv->isGroup = isGroup;
}

yarp::robotinterface::experimental::Param::Param(const ::yarp::robotinterface::experimental::Param& other) :
        mPriv(new Private(this))
{
    mPriv->name = other.mPriv->name;
    mPriv->value = other.mPriv->value;
    mPriv->isGroup = other.mPriv->isGroup;
}

yarp::robotinterface::experimental::Param& yarp::robotinterface::experimental::Param::operator=(const yarp::robotinterface::experimental::Param& other)
{
    if (&other != this) {
        mPriv->name = other.mPriv->name;
        mPriv->value = other.mPriv->value;
        mPriv->isGroup = other.mPriv->isGroup;
    }

    return *this;
}

yarp::robotinterface::experimental::Param::~Param()
{
    delete mPriv;
}

std::string& yarp::robotinterface::experimental::Param::name()
{
    return mPriv->name;
}

std::string& yarp::robotinterface::experimental::Param::value()
{
    return mPriv->value;
}

const std::string& yarp::robotinterface::experimental::Param::name() const
{
    return mPriv->name;
}

const std::string& yarp::robotinterface::experimental::Param::value() const
{
    return mPriv->value;
}

bool yarp::robotinterface::experimental::Param::isGroup() const
{
    return mPriv->isGroup;
}

yarp::os::Property yarp::robotinterface::experimental::Param::toProperty() const
{
    yarp::os::Property p;
    std::string s = "(" + mPriv->name + " " + mPriv->value + ")";
    p.fromString(s);
    return p;
}
