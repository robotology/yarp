/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FakeChatBotDevice.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

YARP_LOG_COMPONENT(FAKECHATBOTDEVICE, "yarp.devices.FakeChatBotDevice")

using namespace yarp::dev;

FakeChatBotDevice::FakeChatBotDevice() :
    m_fallback{"Sorry, I did not get that. Can you repeat?"},
    m_noInput{"I heard nothing. Can you please speak up?"},
    m_status{"greetings"},
    m_qAndA{
            {"greetings",
             {{"Hello!","Hello there."},{"Who are you?","I am just a fake chatbot"},
              {"Goodbye!", "Already? Ok... bye."}}
            },
            {"chatting",
             {{"Hello!","Hello again."},{"Who are you?","I am just a fake chatbot"},
              {"Goodbye!", "Bye bye!"}}
            }
        }
{}

ReturnValue FakeChatBotDevice::interact(const std::string& messageIn, std::string& messageOut)
{
    if(messageIn.empty())
    {
        messageOut = m_noInput;
        return ReturnValue_ok;
    }
    if(m_qAndA[m_status].count(messageIn) < 1)
    {
        messageOut = m_fallback;
        return ReturnValue_ok;
    }

    messageOut = m_qAndA[m_status][messageIn];

    if(m_status == std::string("greetings"))
    {
        m_status = "chatting";
    }

    return ReturnValue_ok;
}

ReturnValue FakeChatBotDevice::setLanguage(const std::string& language)
{
    if (language != "eng")
    {
        yCError(FAKECHATBOTDEVICE) << "Unsupported language. Only English is supported for the moment being";
        return ReturnValue::return_code::return_value_error_method_failed;
    }
    m_language = language;

    return ReturnValue_ok;
}

ReturnValue FakeChatBotDevice::getLanguage(std::string& language)
{
    language = m_language;
    return ReturnValue_ok;
}

ReturnValue FakeChatBotDevice::getStatus(std::string& status)
{
    status = m_status;
    return ReturnValue_ok;
}

ReturnValue FakeChatBotDevice::resetBot()
{
    m_status = "greetings";
    return ReturnValue_ok;
}

bool FakeChatBotDevice::open(yarp::os::Searchable& config)
{
    if (!this->parseParams(config)) {return false;}
    return true;
}

bool FakeChatBotDevice::close()
{
    return true;
}
