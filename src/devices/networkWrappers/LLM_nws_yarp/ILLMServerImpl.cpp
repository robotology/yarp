/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <ILLMServerImpl.h>

namespace {
YARP_LOG_COMPONENT(LLMSERVER, "yarp.device.LLMServer")
}

bool ILLMRPCd::setPrompt(const std::string& prompt)
{
    if (m_iLlm == nullptr) {
        yCError(LLMSERVER, "Invalid interface");
        return false;
    }

    if (!m_iLlm->setPrompt(prompt)) {
        yCError(LLMSERVER, "setPrompt failed");
        return false;
    }

    m_stream_conversation();

    return true;
}

yarp::dev::llm::return_readPrompt ILLMRPCd::readPrompt()
{

    yarp::dev::llm::return_readPrompt ret;

    if (m_iLlm == nullptr) {
        yCError(LLMSERVER, "Invalid interface");
        ret.ret = false;
        return ret;
    }

    ret.ret = m_iLlm->readPrompt(ret.prompt);

    return ret;
}

yarp::dev::llm::return_ask ILLMRPCd::ask(const std::string& question)
{
    yarp::dev::llm::return_ask ret;

    if (m_iLlm == nullptr) {
        yCError(LLMSERVER, "Invalid interface");
        ret.ret = false;
        return ret;
    }

    ret.ret = m_iLlm->ask(question,ret.answer);

    if (ret.ret) {
        m_stream_conversation();
    }

    return ret;
}

void ILLMRPCd::m_stream_conversation()
{
    std::vector<yarp::dev::LLM_Message> conversation;
    if (!m_iLlm->getConversation(conversation)) {
        yCError(LLMSERVER, "Unable to retrieve the conversation");
        return;
    }

    auto& bot = m_streaming_port.prepare();
    bot.clear();
    auto& list = bot.addList();
    for (const auto& message : conversation) {
        auto& message_bot = list.addList();
        message_bot.addString(message.type);
        message_bot.addString(message.content);
        auto& params_bot = message_bot.addList();
        for(const auto& params: message.parameters)
        {
            params_bot.addString(params);
        }

        auto& args_bot = message_bot.addList();
        for(const auto& args: message.arguments)
        {
            args_bot.addString(args);
        }
    }

    m_streaming_port.write();
}


yarp::dev::llm::return_getConversation ILLMRPCd::getConversation()
{
    yarp::dev::llm::return_getConversation ret;

    if (m_iLlm == nullptr) {
        yCError(LLMSERVER, "Invalid interface");
        ret.ret = false;
        return ret;
    }

    std::vector<yarp::dev::LLM_Message> conversation;
    ret.ret = m_iLlm->getConversation(conversation);
    ret.conversation = conversation;

    return ret;
}

bool ILLMRPCd::deleteConversation()
{
    bool ret = false;
    if (m_iLlm == nullptr) {
        yCError(LLMSERVER, "Invalid interface");
        return false;
    }

    ret = m_iLlm->deleteConversation();

    if (ret) {
        m_stream_conversation();
    }

    return ret;
}

bool ILLMRPCd::refreshConversation()
{
    bool ret = false;
    if (m_iLlm == nullptr) {
        yCError(LLMSERVER, "Invalid interface");
        return false;
    }

    ret = m_iLlm->refreshConversation();

    if (ret) {
        m_stream_conversation();
    }

    return ret;
}
