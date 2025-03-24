/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CHATBOT_NWC_YARP_H
#define YARP_DEV_CHATBOT_NWC_YARP_H

#include <yarp/os/Network.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IChatBot.h>
#include "IChatBotMsgs.h"
#include "ChatBot_nwc_yarp_ParamsParser.h"

 /**
 * @ingroup dev_impl_network_clients
 *
 * \brief `chatBot_nwc_yarp`: The client side of any IChatBot capable device.
 *
 * Parameters required by this device are shown in class: ChatBot_nwc_yarp_ParamsParser
 *
 */
class ChatBot_nwc_yarp : public yarp::dev::DeviceDriver,
                         public yarp::dev::IChatBot,
                         public ChatBot_nwc_yarp_ParamsParser
{
protected:
    // thrift interface
    IChatBotMsgs m_thriftClient;

    // rpc port
    yarp::os::Port m_thriftClientPort;

public:

    //From DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //From IChatBot
    yarp::dev::ReturnValue interact(const std::string& messageIn, std::string& messageOut) override;
    yarp::dev::ReturnValue setLanguage(const std::string& language) override;
    yarp::dev::ReturnValue getLanguage(std::string& language) override;
    yarp::dev::ReturnValue getStatus(std::string& status) override;
    yarp::dev::ReturnValue resetBot() override;
};

#endif // YARP_DEV_CHATBOT_NWC_YARP_H
