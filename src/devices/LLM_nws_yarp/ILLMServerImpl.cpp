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
    }

    ret.ret = m_iLlm->ask(question, ret.answer);

    if (ret.ret) {
        m_stream_conversation();
    }

    return ret;
}

void ILLMRPCd::m_stream_conversation()
{
    std::vector<std::pair<Author, Content>> conversation;
    if (!m_iLlm->getConversation(conversation)) {
        yCError(LLMSERVER, "Unable to retrieve the conversation");
        return;
    }

    auto& bot = m_streaming_port.prepare();
    auto& list = bot.addList();
    for (const auto& [author, message] : conversation) {
        auto& message_bot = list.addList();
        message_bot.addString(author);
        message_bot.addString(message);
    }

    m_streaming_port.write();
}


yarp::dev::llm::return_getConversation ILLMRPCd::getConversation()
{
    yarp::dev::llm::return_getConversation ret;

    if (m_iLlm == nullptr) {
        yCError(LLMSERVER, "Invalid interface");
        ret.ret = false;
    }

    std::vector<std::pair<Author, Content>> conversation;
    ret.ret = m_iLlm->getConversation(conversation);

    for (const auto& [author, message] : conversation) {
        ret.conversation.push_back(yarp::dev::llm::Message(author, message));
    }

    return ret;
}

bool ILLMRPCd::deleteConversation()
{
    bool ret = false;
    if (m_iLlm == nullptr) {
        yCError(LLMSERVER, "Invalid interface");
    }

    ret = m_iLlm->deleteConversation();

    if (ret) {
        m_stream_conversation();
    }

    return ret;
}
