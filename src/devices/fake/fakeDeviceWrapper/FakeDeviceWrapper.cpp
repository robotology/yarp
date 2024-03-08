/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FakeDeviceWrapper.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace yarp::os;
using namespace yarp::dev;

namespace {
YARP_LOG_COMPONENT(FAKEDEVICEWRAPPER, "yarp.device.fakeDeviceWrapper")
}

void FakeDeviceWrapper::testSetValue(int value)
{
    if (!iTest) { yCError(FAKEDEVICEWRAPPER) << "Invalid/not yet attached interface"; }

    yCInfo(FAKEDEVICEWRAPPER) << "testSetValue() called";
    iTest->testSetValue(value);
}

void FakeDeviceWrapper::testGetValue(int& value)
{
    if (!iTest) { yCError(FAKEDEVICEWRAPPER) << "Invalid/not yet attached interface"; }

    yCInfo(FAKEDEVICEWRAPPER) << "testGetValue() called";
    iTest->testGetValue(value);
}

bool FakeDeviceWrapper::open(yarp::os::Searchable& config)
{
    yCInfo(FAKEDEVICEWRAPPER) << "FakeDeviceWrapper opened";
    return true;
}

bool FakeDeviceWrapper::close()
{
    yCInfo(FAKEDEVICEWRAPPER) << "FakeDeviceWrapper closed";
    return true;
}

bool FakeDeviceWrapper::detach()
{
    iTest=nullptr;
    return true;
}

bool FakeDeviceWrapper::attach(yarp::dev::PolyDriver* drv)
{
    if (drv->isValid())
    {
        drv->view(iTest);
        return true;
    }
    return false;
}
