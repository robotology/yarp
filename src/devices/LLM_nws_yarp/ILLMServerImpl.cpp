/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "ILLMServerImpl.h"
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

namespace
{
    YARP_LOG_COMPONENT(LLMSERVER, "yarp.device.LLMServer")
}

bool ILLMRPCd::setPrompt(const std::string &prompt)
{
    if (m_iLlm == nullptr)
    {
        yCError(LLMSERVER, "Invalid interface");
        return false;
    }
    // TODO: maybe we can catch errors here, that would be better
    m_iLlm->setPrompt(prompt);
    return true;
}

yarp::dev::return_readPrompt ILLMRPCd::readPrompt()
{

    yarp::dev::return_readPrompt ret;

    if (m_iLlm == nullptr)
    {
        yCError(LLMSERVER, "Invalid interface");
        ret.ret = false;
    }

    ret.ret = m_iLlm->readPrompt(ret.prompt);

    return ret;
}

yarp::dev::return_ask ILLMRPCd::ask(const std::string &question)
{
    yarp::dev::return_ask ret;

    if (m_iLlm == nullptr)
    {
        yCError(LLMSERVER, "Invalid interface");
        ret.ret = false;
    }

    ret.ret = m_iLlm->ask(question,ret.answer);

    return ret;
}

yarp::dev::return_getConversation ILLMRPCd::getConversation()
{
    yarp::dev::return_getConversation ret;

    if (m_iLlm == nullptr)
    {
        yCError(LLMSERVER, "Invalid interface");
        ret.ret = false;
    }

    std::vector<std::pair<Author,Content>> conversation;
    ret.ret = m_iLlm->getConversation(conversation);

    for (const auto &[author, message] : conversation)
    {
        ret.conversation.push_back(yarp::dev::Message(author, message));
    }

    return ret;
}

bool ILLMRPCd::deleteConversation()
{
    if (m_iLlm == nullptr)
    {
        yCError(LLMSERVER, "Invalid interface");
    }

    return m_iLlm->deleteConversation();
}
