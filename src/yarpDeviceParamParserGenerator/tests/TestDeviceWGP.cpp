/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "TestDeviceWGP.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace yarp::os;
using namespace yarp::dev;

TestDeviceWGP::TestDeviceWGP()
{
}

TestDeviceWGP::~TestDeviceWGP()
{
}

bool TestDeviceWGP::open(yarp::os::Searchable &config)
{
    bool ret = parseParams(config);
    return ret;
}

bool TestDeviceWGP::close()
{
    return true;
}
