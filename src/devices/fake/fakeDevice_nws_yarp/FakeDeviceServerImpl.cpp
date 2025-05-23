/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <FakeDeviceServerImpl.h>

namespace {
YARP_LOG_COMPONENT(FAKEDEVICESERVER, "yarp.device.FakeDeviceServer")
}

yarp::dev::ReturnValue FakeServerRPCd::doSomethingRPC()
{
    if (m_iFake == nullptr)
    {
        yCError(FAKEDEVICESERVER, "Invalid interface");
        return yarp::dev::ReturnValue::return_code::return_value_error_not_ready;
    }

    yarp::dev::ReturnValue ret = m_iFake->doSomething();
    if (!ret)
    {
        yCError(FAKEDEVICESERVER, "doSomething() failed");
        return ret;
    }

    return yarp::dev::ReturnValue_ok;
}

void FakeServerRPCd::setInterface(yarp::dev::test::IFakeDeviceInterfaceTest1* _ifake)
{
    m_iFake = _ifake;
}

void FakeServerRPCd::unsetInterface()
{
    m_iFake = nullptr;
}
