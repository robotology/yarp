/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef FAKE_LLMDEVICE_H
#define FAKE_LLMDEVICE_H

#include <yarp/dev/ILLM.h>
#include <vector>
#include <yarp/dev/DeviceDriver.h>

/**
* @ingroup dev_impl_fake dev_impl_other
*
* @brief `fakeLLMDevice` : a fake device which implements the ILLM interface for testing purposes.
*
*/
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

#endif
