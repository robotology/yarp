/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/ILLM.h>
#include <vector>
#include <yarp/dev/DeviceDriver.h>

class fakeLLMDevice : public yarp::dev::ILLM,
                      public yarp::dev::DeviceDriver
{

public:
    fakeLLMDevice() : m_conversation{} {}
    bool setPrompt(const std::string &prompt) override;
    bool readPrompt(std::string &oPromp) override;
    bool ask(const std::string &question, std::string &oAnswer) override;
    bool getConversation(std::vector<std::pair<Author, Content>> &oConversation) override;
    bool deleteConversation() noexcept override;

private:
    std::vector<std::pair<Author, Content>> m_conversation;
};