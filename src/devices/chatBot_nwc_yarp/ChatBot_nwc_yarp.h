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

 /**
 * @ingroup dev_impl_network_clients
 *
 * \brief `chatBot_nwc_yarp`: The client side of any IChatBot capable device.
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value | Required     | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:-----------: |:-----------------------------------------------------------------:|:-----:|
 * | local          |      -         | string  | -              |   -           | Yes          | Full port name opened by the nwc device.                |       |
 * | remote         |      -         | string  | -              |   -           | Yes          | Full port name of the port opened on the server side, to which the nwc connects to.    |     |
 */
class ChatBot_nwc_yarp : public yarp::dev::DeviceDriver,
                     public yarp::dev::IChatBot
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
    bool interact(const std::string& messageIn, std::string& messageOut) override;
    bool setLanguage(const std::string& language) override;
    bool getLanguage(std::string& language) override;
    bool resetBot() override;
};

#endif // YARP_DEV_CHATBOT_NWC_YARP_H
