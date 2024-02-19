/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef FAKE_CHATBOTDEVICE_H
#define FAKE_CHATBOTDEVICE_H

#include <yarp/dev/IChatBot.h>
#include <vector>
#include <yarp/dev/DeviceDriver.h>
#include <map>
#include "FakeChatBotDevice_ParamsParser.h"

/**
* @ingroup dev_impl_fake dev_impl_other
*
* @brief `fakeChatBotDevice` : a fake device which implements the IChatBot interface for testing purposes.
*
* Parameters required by this device are shown in class: FakeChatBotDevice_ParamsParser
*
*/
class FakeChatBotDevice : public yarp::dev::IChatBot,
                          public yarp::dev::DeviceDriver,
                          public FakeChatBotDevice_ParamsParser
{

public:
    FakeChatBotDevice();
    bool interact(const std::string& messageIn, std::string& messageOut) override;
    bool setLanguage(const std::string& language) override;
    bool getLanguage(std::string& language) override;
    bool getStatus(std::string& status) override;
    bool resetBot() override;

    bool open(yarp::os::Searchable& config) override;
    bool close() override;

private:
    std::string m_currBot;
    std::string m_fallback;
    std::string m_noInput;
    std::string m_status;
    std::map<std::string, std::map<std::string,std::string>> m_qAndA;
};

#endif // FAKE_CHATBOTDEVICE_H
