/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fakeDevice_nws_yarp.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

namespace
{
YARP_LOG_COMPONENT(FAKEDEVICE_NWS_YARP, "yarp.device.FakeDevice_nws_yarp")
}

bool FakeDevice_nws_yarp::attach(yarp::dev::PolyDriver* driver)
{
    if (driver->isValid())
    {
        driver->view(m_iFake);
    }

    if (m_iFake == nullptr)
    {
        yCError(FAKEDEVICE_NWS_YARP, "Subdevice passed to attach method is invalid (it does not implement all the required interfaces)");
        return false;
    }

    m_RPC.setInterface(m_iFake);

    yCDebug(FAKEDEVICE_NWS_YARP) << "Attach successful";

    return true;
}

bool FakeDevice_nws_yarp::open(yarp::os::Searchable& config)
{
    if (!m_RpcPort.open(m_rpc_port_name))
    {
        yCError(FAKEDEVICE_NWS_YARP) << "Unable to open port:" << m_rpc_port_name;
        return false;
    }

    m_RpcPort.setReader(*this);

    yCDebug(FAKEDEVICE_NWS_YARP) << "Waiting to be attached";

    return true;
}

bool FakeDevice_nws_yarp::detach()
{
    m_RPC.unsetInterface();
    m_iFake = nullptr;
    return true;
}

bool FakeDevice_nws_yarp::close()
{
    detach();
    m_RpcPort.close();
    return true;
}

bool FakeDevice_nws_yarp::read(yarp::os::ConnectionReader& connection)
{
    bool b = m_RPC.read(connection);
    if (b)
    {
        return true;
    }

    yCWarning(FAKEDEVICE_NWS_YARP) << "read() Command failed";

    return false;
}
