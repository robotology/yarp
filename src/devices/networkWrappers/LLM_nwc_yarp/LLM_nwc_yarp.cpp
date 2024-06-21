/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "LLM_nwc_yarp.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

namespace
{
    YARP_LOG_COMPONENT(LLM_NWC_YARP, "yarp.device.LLM_nwc_yarp")
}

bool LLM_nwc_yarp::open(yarp::os::Searchable &config)
{
    if (!parseParams(config)) { return false; }

    if (!m_rpc_port_to_LLM_server.open(m_local))
    {
        yCError(LLM_NWC_YARP, "Cannot open rpc port, check network");
    }

    bool ok = false;

    ok = yarp::os::Network::connect(m_local, m_remote);

    if (!ok)
    {
        yCError(LLM_NWC_YARP, "open() error could not connect to %s", m_remote.c_str());
        return false;
    }

    if (!m_LLM_RPC.yarp().attachAsClient(m_rpc_port_to_LLM_server))
    {
        yCError(LLM_NWC_YARP, "Cannot attach the m_rpc_port_to_LLM_server port as client");
    }

    yCDebug(LLM_NWC_YARP) << "Opening of nwc successful";
    return true;
}

bool LLM_nwc_yarp::close()
{
    m_rpc_port_to_LLM_server.close();
    return true;
}

bool LLM_nwc_yarp::setPrompt(const std::string &prompt)
{
    return m_LLM_RPC.setPrompt(prompt);
}

bool LLM_nwc_yarp::readPrompt(std::string &oPrompt)
{
    auto ret = m_LLM_RPC.readPrompt();

    oPrompt = ret.prompt;
    return ret.ret;
}

bool LLM_nwc_yarp::ask(const std::string &question, yarp::dev::LLM_Message &oAnswer)
{
    auto ret = m_LLM_RPC.ask(question);

    oAnswer = ret.answer;
    return ret.ret;
}

bool LLM_nwc_yarp::getConversation(std::vector<yarp::dev::LLM_Message> &oConversation)
{
    auto ret = m_LLM_RPC.getConversation();
    for (const auto &message : ret.conversation)
    {
        oConversation.push_back(message);
    }

    return true;
}

bool LLM_nwc_yarp::deleteConversation()
{
    return m_LLM_RPC.deleteConversation();
}

bool LLM_nwc_yarp::refreshConversation()
{
    return m_LLM_RPC.refreshConversation();
}