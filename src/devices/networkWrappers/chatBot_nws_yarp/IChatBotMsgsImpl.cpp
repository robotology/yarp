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

    if(!m_iChatBot->interact(messageIn,messageOut))
    {
        yCError(ICHATBOTMSGSIMPL) << "An error occurred while interacting with the chatBot";
        response.result = false;
        return response;
    }

    response.messageOut = messageOut;
    response.result = true;
    return response;
}

bool IChatBotMsgsImpl::setLanguageRPC(const std::string& language)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    if(!m_iChatBot->setLanguage(language))
    {
        yCError(ICHATBOTMSGSIMPL) << "Could not set bot language to" << language;
        return false;
    }

    return true;
}

return_getLanguage IChatBotMsgsImpl::getLanguageRPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return_getLanguage response;
    std::string language;
    if(!m_iChatBot->getLanguage(language))
    {
        yCError(ICHATBOTMSGSIMPL) << "Could not retrieve the chatbot language";
        response.result = false;
        return response;
    }

    response.result = true;
    response.language = language;

    return response;
}

return_getStatus IChatBotMsgsImpl::getStatusRPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return_getStatus response;
    std::string status;
    if(!m_iChatBot->getStatus(status))
    {
        yCError(ICHATBOTMSGSIMPL) << "Could not retrieve the chatbot status";
        response.result = false;
        return response;
    }

    response.result = true;
    response.status = status;

    return response;
}

bool IChatBotMsgsImpl::resetBotRPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    if(!m_iChatBot->resetBot())
    {
        yCError(ICHATBOTMSGSIMPL) << "Could not reset the bot";

        return false;
    }

    return true;
}
