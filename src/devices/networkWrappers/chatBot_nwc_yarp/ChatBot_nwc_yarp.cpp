/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ChatBot_nwc_yarp.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

namespace
{
    YARP_LOG_COMPONENT(CHATBOT_NWC_YARP, "yarp.device.chatBot_nwc_yarp")
}

using namespace yarp::dev;

bool ChatBot_nwc_yarp::open(yarp::os::Searchable &config)
{
    if (!parseParams(config)) { return false; }

    if (!m_thriftClientPort.open(m_local))
    {
        yCError(CHATBOT_NWC_YARP) << "Cannot open rpc port, check network";

        return false;
    }

    bool ok = false;

    ok = yarp::os::Network::connect(m_local, m_remote);

    if (!ok)
    {
        yCError(CHATBOT_NWC_YARP) << "open() error could not connect to" << m_remote;
        return false;
    }

    if (!m_thriftClient.yarp().attachAsClient(m_thriftClientPort))
    {
        yCError(CHATBOT_NWC_YARP) << "Cannot attach the m_thriftClientPort port as client";
        return false;
    }

    yCDebug(CHATBOT_NWC_YARP) << "Opening of nwc successful";
    return true;
}

bool ChatBot_nwc_yarp::close()
{
    m_thriftClientPort.close();
    return true;
}

ReturnValue ChatBot_nwc_yarp::interact(const std::string& messageIn, std::string& messageOut)
{
    return_interact output = m_thriftClient.interactRPC(messageIn);
    if(!output.result)
    {
        yCError(CHATBOT_NWC_YARP) << "Could not interact with the chatbot";
        return output.result;
    }

    messageOut = output.messageOut;
    return ReturnValue_ok;
}

ReturnValue ChatBot_nwc_yarp::setLanguage(const std::string& language)
{
    auto ret = m_thriftClient.setLanguageRPC(language);
    if(!ret)
    {
        yCError(CHATBOT_NWC_YARP) << "Could not set the chatbot language to" << language;
        return ret;
    }
    return ReturnValue_ok;
}

ReturnValue ChatBot_nwc_yarp::getLanguage(std::string& language)
{
    auto output = m_thriftClient.getLanguageRPC();
    if(!output.result)
    {
        yCError(CHATBOT_NWC_YARP) << "Could not retrieve the currently set language";
        return output.result;
    }
    language = output.language;
    return ReturnValue_ok;
}

ReturnValue ChatBot_nwc_yarp::getStatus(std::string& status)
{
    auto output = m_thriftClient.getStatusRPC();
    if(!output.result)
    {
        yCError(CHATBOT_NWC_YARP) << "Could not retrieve the current bot status";
        return output.result;
    }
    status = output.status;
    return ReturnValue_ok;
}

ReturnValue ChatBot_nwc_yarp::resetBot()
{
    auto output = m_thriftClient.resetBotRPC();
    if(!output)
    {
        yCError(CHATBOT_NWC_YARP) << "Could not reset the chatbot";
        return output;
    }
    return ReturnValue_ok;
}
