/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/LLM_Message.h>

using namespace yarp::dev;

LLM_Message::LLM_Message()
{
}

LLM_Message::LLM_Message(const std::string& type,
                const std::string& content,
                const std::vector<std::string>& parameters,
                const std::vector<std::string>& arguments)
    : LLM_MessageData(type, content, parameters, arguments)
{
}

LLM_Message::~LLM_Message()
{
}
