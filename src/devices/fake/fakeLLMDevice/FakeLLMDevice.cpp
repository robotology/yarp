/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FakeLLMDevice.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

bool FakeLLMDevice::setPrompt(const std::string &prompt)
{
    if(!m_conversation.empty())
    {
        yError() << "The conversation is already ongoing. You must first delete the whole conversation and start from scratch.";
        return false;
    }

    m_conversation.push_back(yarp::dev::LLM_Message("system", prompt,{},{}));
    return true;
}

bool FakeLLMDevice::readPrompt(std::string &oPrompt)
{
    for (const auto &message : m_conversation)
    {
        if (message.type == "system")
        {
            oPrompt = message.content;
            return true;
        }
    }

    return false;
}

bool FakeLLMDevice::ask(const std::string &question, yarp::dev::LLM_Message &oAnswer)
{
    // In the fake device we ignore the question
    yarp::dev::LLM_Message answer;
    if(question == "function")
    {
        std::string function_name = "FakeFunction";
        std::vector<std::string> function_params = {"arg1","arg2"};
        std::vector<std::string> function_args = {"yes","no"};
        answer = yarp::dev::LLM_Message("function",function_name,function_params,function_args);
    }
    else
    {
        std::string answer_content = "Fatti non foste per viver come bruti ma per seguir virtute e canoscenza";
        answer = yarp::dev::LLM_Message("assistant", answer_content,{},{});
    }
    m_conversation.push_back(yarp::dev::LLM_Message("user", question,{},{}));
    m_conversation.push_back(answer);
    oAnswer = answer;
    return true;
}

bool FakeLLMDevice::getConversation(std::vector<yarp::dev::LLM_Message>& oConversation)
{
    oConversation = m_conversation;
    return true;
}

bool FakeLLMDevice::deleteConversation() noexcept
{
    m_conversation.clear();
    return true;
}

bool FakeLLMDevice::refreshConversation() noexcept
{
    std::string current_prompt;

    if(!this->readPrompt(current_prompt))
    {
        yError() << "No prompt found in the conversation. Cannot refresh.";
        return false;
    }

    this->deleteConversation();

    if(!this->setPrompt(current_prompt))
    {
        yError() << "Failed to refresh the conversation.";
        return false;
    }

    return true;
}

bool FakeLLMDevice::open(yarp::os::Searchable& config)
{
    if (!this->parseParams(config)) {return false;}
    return true;
}

bool FakeLLMDevice::close()
{
    return true;
}
