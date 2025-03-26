/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ChatBot_nws_yarp.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Os.h>

namespace {
YARP_LOG_COMPONENT(ICHATBOTMSGSIMPL, "yarp.devices.chatBot_nws_yarp.ChatBotRPC_CallbackHelper")
}

using namespace yarp::dev;

bool IChatBotMsgsImpl::setInterfaces(yarp::dev::IChatBot* iChatBot)
{
    if(!iChatBot)
    {
        yCError(ICHATBOTMSGSIMPL) << "Invalid pointer to IChatBot device";
        return false;
    }

    m_iChatBot = iChatBot;

    return true;
}

return_interact IChatBotMsgsImpl::interactRPC(const std::string& messageIn)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return_interact response;
    std::string messageOut;

    auto ret = m_iChatBot->interact(messageIn, messageOut);
    if(!ret)
    {
        yCError(ICHATBOTMSGSIMPL) << "An error occurred while interacting with the chatBot";
        response.result = ret;
        return response;
    }

    response.result = ret;
    response.messageOut = messageOut;
    return response;
}

ReturnValue IChatBotMsgsImpl::setLanguageRPC(const std::string& language)
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iChatBot->setLanguage(language);
    if(!ret)
    {
        yCError(ICHATBOTMSGSIMPL) << "Could not set bot language to" << language;
        return ret;
    }

    return ReturnValue_ok;
}

return_getLanguage IChatBotMsgsImpl::getLanguageRPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return_getLanguage response;
    std::string language;

    auto ret = m_iChatBot->getLanguage(language);
    if(!ret)
    {
        yCError(ICHATBOTMSGSIMPL) << "Could not retrieve the chatbot language";
        response.result = ret;
        return response;
    }

    response.result = ret;
    response.language = language;

    return response;
}

return_getStatus IChatBotMsgsImpl::getStatusRPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return_getStatus response;
    std::string status;

    auto ret = m_iChatBot->getStatus(status);
    if(!ret)
    {
        yCError(ICHATBOTMSGSIMPL) << "Could not retrieve the chatbot status";
        response.result = ret;
        return response;
    }

    response.result = ret;
    response.status = status;

    return response;
}

ReturnValue IChatBotMsgsImpl::resetBotRPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);

    auto ret = m_iChatBot->resetBot();
    if(!ret)
    {
        yCError(ICHATBOTMSGSIMPL) << "Could not reset the bot";
        return ret;
    }

    return ReturnValue_ok;
}
