/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fakeLLMDevice.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

bool fakeLLMDevice::setPrompt(const std::string &prompt)
{
    if(!m_conversation.empty())
    {
        yError() << "The conversation is already ongoing. You must first delete the whole conversation and start from scratch.";
        return false;
    }

    m_conversation.push_back(std::make_pair("system", prompt));
    return true;
}

bool fakeLLMDevice::readPrompt(std::string &oPrompt)
{
    for (const auto &[author, content] : m_conversation)
    {
        if (author == "system")
        {
            oPrompt = content;
            return true;
        }
    }

    return false;
}

bool fakeLLMDevice::ask(const std::string &question, std::string &oAnswer)
{
    // In the fake device we ignore the question
    std::string answer = "Fatti non foste per viver come bruti ma per seguir virtute e canoscenza";
    m_conversation.push_back(std::make_pair("user", question));
    m_conversation.push_back(std::make_pair("assistant", answer));
    oAnswer = answer;
    return true;
}

bool fakeLLMDevice::getConversation(std::vector<std::pair<Author, Content>>& oConversation)
{
    oConversation = m_conversation;
    return true;
}

bool fakeLLMDevice::deleteConversation() noexcept
{
    m_conversation.clear();
    return true;
}
