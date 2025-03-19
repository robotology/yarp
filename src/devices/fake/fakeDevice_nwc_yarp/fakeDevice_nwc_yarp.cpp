/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fakeDevice_nwc_yarp.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

namespace
{
    YARP_LOG_COMPONENT(FAKEDEVICE_NWC_YARP, "yarp.device.FakeDevice_nwc_yarp")
}

bool FakeDevice_nwc_yarp::open(yarp::os::Searchable &config)
{
    if (!m_rpc_port_to_server.open(m_local))
    {
        yCError(FAKEDEVICE_NWC_YARP, "Cannot open rpc port, check network");
    }

    bool ok = false;

    ok = yarp::os::Network::connect(m_local, m_remote);

    if (!ok)
    {
        yCError(FAKEDEVICE_NWC_YARP, "open() error could not connect to %s", m_remote.c_str());
        return false;
    }

    if (!m_thrift_RPC.yarp().attachAsClient(m_rpc_port_to_server))
    {
        yCError(FAKEDEVICE_NWC_YARP, "Cannot attach the RPC port as client");
    }

    yCDebug(FAKEDEVICE_NWC_YARP) << "Opening of NWC successful";
    return true;
}

bool FakeDevice_nwc_yarp::close()
{
    m_rpc_port_to_server.close();
    return true;
}

yarp::dev::ReturnValue FakeDevice_nwc_yarp::doSomething()
{
    yarp::dev::ReturnValue ret = m_thrift_RPC.doSomethingRPC();
    return ret;
}
