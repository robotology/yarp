/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FakeDeviceUnwrapped.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace yarp::os;
using namespace yarp::dev;

namespace {
    YARP_LOG_COMPONENT(FAKEDEVICEUNWRAPPED, "yarp.device.fakeDeviceUnwrapped")
}

yarp::dev::ReturnValue FakeDeviceUnwrapped::doSomething()
{
    yCInfo(FAKEDEVICEUNWRAPPED) << "doSomething executed";
    return ReturnValue_ok;
}

yarp::dev::ReturnValue FakeDeviceUnwrapped::testSetValue(int value)
{
    yCInfo(FAKEDEVICEUNWRAPPED) << "testSetValue" << value;
    m_value = value;
    return ReturnValue_ok;
}

yarp::dev::ReturnValue FakeDeviceUnwrapped::testGetValue(int& value)
{
    yCInfo(FAKEDEVICEUNWRAPPED) << "testGetValue" << m_value;
    value=m_value;
    return ReturnValue_ok;
}

bool FakeDeviceUnwrapped::open(yarp::os::Searchable& config)
{
    yCInfo(FAKEDEVICEUNWRAPPED) << "FakeDeviceUnwrapped opened";
    return true;
}

bool FakeDeviceUnwrapped::close()
{
    yCInfo(FAKEDEVICEUNWRAPPED) << "FakeDeviceUnwrapped closed";
    return true;
}
