/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "LLM_nws_yarp.h"
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

namespace
{
    YARP_LOG_COMPONENT(LLM_NWS_YARP, "yarp.device.LLM_nws_yarp")
}

bool LLM_nws_yarp::attach(yarp::dev::PolyDriver *driver)
{
    if (driver->isValid())
    {
        driver->view(m_iLlm);
    }

    if (m_iLlm == nullptr)
    {
        yCError(LLM_NWS_YARP, "Subdevice passed to attach method is invalid (it does not implement all the required interfaces)");
        return false;
    }

    m_RPC.setInterface(m_iLlm);

    yCDebug(LLM_NWS_YARP) << "Attachment successful";

    return true;
}

bool LLM_nws_yarp::open(yarp::os::Searchable &prop)
{
    std::string rpc_portname;

    if (!prop.check("name"))
    {
        yCError(LLM_NWS_YARP) << "Missing 'name' parameter. Using something like: /LLM_nws/rpc";
        return false;
    }
    else
    {
        rpc_portname = prop.find("name").asString();
        if (rpc_portname.c_str()[0] != '/') { yCError(LLM_NWS_YARP) << "Missing '/' in name parameter";  return false; }
        yCInfo(LLM_NWS_YARP) << "Using local name:" << rpc_portname;
    }

    if (!m_RpcPort.open(rpc_portname))
    {
        yCError(LLM_NWS_YARP) << "Unable to open port:" << rpc_portname;
        return false;
    }

    m_RpcPort.setReader(*this);

    yCDebug(LLM_NWS_YARP) << "Waiting to be attached";

    return true;
}

bool LLM_nws_yarp::detach()
{
    m_iLlm = nullptr;
    return true;
}

bool LLM_nws_yarp::close()
{
    m_RpcPort.interrupt();
    m_RpcPort.close();
    return true;
}

bool LLM_nws_yarp::read(yarp::os::ConnectionReader &connection)
{
    bool b = m_RPC.read(connection);
    if (b)
    {
        return true;
    }

    yCDebug(LLM_NWS_YARP) << "read() Command failed";

    return false;
}
