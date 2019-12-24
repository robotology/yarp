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

#include "Param.h"

#include <yarp/os/LogStream.h>

#include <yarp/os/Property.h>

#include <string>


std::ostream& std::operator<<(std::ostream &oss, const yarp::robotinterface::Param &t)
{
    oss << "(\"" << t.name() << "\"" << (t.isGroup() ? " [group]" : "") << " = \"" << t.value() << "\")";
    return oss;
}


class yarp::robotinterface::Param::Private
{
public:
    Private(Param * /*parent*/) :
            isGroup(false)
    {
    }

    std::string name;
    std::string value;
    bool isGroup;
};

yarp::os::LogStream operator<<(yarp::os::LogStream dbg, const yarp::robotinterface::Param &t)
{
    std::ostringstream oss;
    oss << t;
    dbg << oss.str();
    return dbg;
}

yarp::robotinterface::Param::Param(bool isGroup) :
    mPriv(new Private(this))
{
    mPriv->isGroup = isGroup;
}

yarp::robotinterface::Param::Param(const std::string &name, const std::string &value, bool isGroup) :
    mPriv(new Private(this))
{
    mPriv->name = name;
    mPriv->value = value;
    mPriv->isGroup = isGroup;
}

yarp::robotinterface::Param::Param(const ::yarp::robotinterface::Param& other) :
    mPriv(new Private(this))
{
    mPriv->name = other.mPriv->name;
    mPriv->value = other.mPriv->value;
    mPriv->isGroup = other.mPriv->isGroup;
}

yarp::robotinterface::Param& yarp::robotinterface::Param::operator=(const yarp::robotinterface::Param& other)
{
    if (&other != this) {
        mPriv->name = other.mPriv->name;
        mPriv->value = other.mPriv->value;
        mPriv->isGroup = other.mPriv->isGroup;
    }

    return *this;
}

yarp::robotinterface::Param::~Param()
{
    delete mPriv;
}

std::string& yarp::robotinterface::Param::name()
{
    return mPriv->name;
}

std::string& yarp::robotinterface::Param::value()
{
    return mPriv->value;
}

const std::string& yarp::robotinterface::Param::name() const
{
    return mPriv->name;
}

const std::string& yarp::robotinterface::Param::value() const
{
    return mPriv->value;
}

bool yarp::robotinterface::Param::isGroup() const
{
    return mPriv->isGroup;
}

yarp::os::Property yarp::robotinterface::Param::toProperty() const
{
    yarp::os::Property p;
    std::string s = "(" + mPriv->name + " " + mPriv->value + ")";
    p.fromString(s);
    return p;
}
