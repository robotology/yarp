/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/Value.h>

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::os::impl;

class DeviceDriver::Private
{
public:
    std::string device_id;
};

DeviceDriver::DeviceDriver() :
        mPriv(new Private)
{
}

DeviceDriver::~DeviceDriver()
{
    delete mPriv;
}

void DeviceDriver::setId(const std::string& id)
{
    mPriv->device_id = id;
}

std::string DeviceDriver::id() const
{
    return mPriv->device_id;
}
