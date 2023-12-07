/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-FileCopyrightText: 2006, 2008 Arjan Gijsberts
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Searchable.h>

#include <yarp/os/Value.h>

using namespace yarp::os;

yarp::os::Searchable::Searchable()
{
}

yarp::os::Searchable::~Searchable() = default;

bool yarp::os::Searchable::check(const std::string& key,
                                 yarp::os::Value*& result,
                                 const std::string& comment) const
{
    yarp::os::Value& bit = find(key);
    bool ok = !(bit.isNull());
    if (ok) {
        result = &bit;
    }
    return ok;
}

yarp::os::Value yarp::os::Searchable::check(const std::string& key,
                                            const yarp::os::Value& fallback,
                                            const std::string& comment) const
{
    yarp::os::Value& bit = find(key);
    bool ok = !(bit.isNull());
    if (ok) {
        return bit;
    }
    return fallback;
}

bool yarp::os::Searchable::check(const std::string& key,
                                 const std::string& comment) const
{
    return check(key);
}

yarp::os::Bottle& yarp::os::Searchable::findGroup(const std::string& key,
                                                  const std::string& comment) const
{
    return findGroup(key);
}

bool yarp::os::Searchable::isNull() const
{
    return false;
}
