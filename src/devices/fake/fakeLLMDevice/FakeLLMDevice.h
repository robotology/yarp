/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef FAKE_LLMDEVICE_H
#define FAKE_LLMDEVICE_H

#include <yarp/dev/ILLM.h>
#include <vector>
#include <yarp/dev/DeviceDriver.h>
#include "FakeLLMDevice_ParamsParser.h"

/**
* @ingroup dev_impl_fake dev_impl_other
*
* @brief `fakeLLMDevice` : a fake device which implements the ILLM interface for testing purposes.
*
* Parameters required by this device are shown in class: FakeLLMDevice_ParamsParser
*
*/
class FakeLLMDevice : public yarp::dev::ILLM,
                      public yarp::dev::DeviceDriver,
                      public FakeLLMDevice_ParamsParser
{

public:
    FakeLLMDevice() : m_conversation{} {}
    yarp::dev::ReturnValue setPrompt(const std::string &prompt) override;
    yarp::dev::ReturnValue readPrompt(std::string &oPromp) override;
    yarp::dev::ReturnValue ask(const std::string &question, yarp::dev::LLM_Message &oAnswer) override;
    yarp::dev::ReturnValue getConversation(std::vector<yarp::dev::LLM_Message> &oConversation) override;
    yarp::dev::ReturnValue deleteConversation() noexcept override;
    yarp::dev::ReturnValue refreshConversation() noexcept override;

    bool open(yarp::os::Searchable& config) override;
    bool close() override;

private:
    std::vector<yarp::dev::LLM_Message> m_conversation;
};

#endif
