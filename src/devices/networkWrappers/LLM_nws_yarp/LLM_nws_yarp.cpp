/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <LLM_nws_yarp.h>

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

namespace
{
YARP_LOG_COMPONENT(LLM_NWS_YARP, "yarp.device.LLM_nws_yarp")
}

bool LLM_nws_yarp::attach(yarp::dev::PolyDriver* driver)
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

    std::string streaming_name = m_name + "/conv:o";
    m_RPC.setInterface(m_iLlm, streaming_name);

    yCDebug(LLM_NWS_YARP) << "Attachment successful";

    return true;
}

bool LLM_nws_yarp::open(yarp::os::Searchable& config)
{
    if (!parseParams(config)) { return false; }

    std::string rpc_name = m_name + "/rpc:i";
    if (!m_RpcPort.open(rpc_name))
    {
        yCError(LLM_NWS_YARP) << "Unable to open port:" << m_name;
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
    m_RPC.unsetInterface();
    return true;
}

bool LLM_nws_yarp::read(yarp::os::ConnectionReader& connection)
{
    bool b = m_RPC.read(connection);
    if (b)
    {
        return true;
    }

    yCDebug(LLM_NWS_YARP) << "read() Command failed";

    return false;
}
