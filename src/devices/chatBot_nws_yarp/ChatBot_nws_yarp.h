/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CHATBOT_NWS_YARP_H
#define YARP_DEV_CHATBOT_NWS_YARP_H

#include <cstdio>
#include <cstdlib>
#include <mutex>

#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IChatBot.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/RpcServer.h>
#include <yarp/dev/WrapperSingle.h>
#include "IChatBotMsgs.h"

using namespace yarp::os;
using namespace yarp::dev;

// Callback implementation after buffered input.
class ChatBotRPC_CallbackHelper :
        public yarp::os::TypedReaderCallback<yarp::os::Bottle>
{
protected:
    yarp::dev::IChatBot* m_iChatBot{nullptr};
    yarp::os::Port*      m_outPort{nullptr};

public:
    ChatBotRPC_CallbackHelper() = delete;
    ChatBotRPC_CallbackHelper(yarp::dev::IChatBot* x, yarp::os::Port* output_port);
    virtual ~ChatBotRPC_CallbackHelper() override {};

    using yarp::os::TypedReaderCallback<yarp::os::Bottle>::onRead;
    void onRead(yarp::os::Bottle& b) override;
};

// rpc commands
class IChatBotMsgsImpl : public IChatBotMsgs
{
private:
    std::mutex           m_mutex;
    yarp::dev::IChatBot* m_iChatBot{nullptr};
    yarp::os::Port*      m_output_port{nullptr};

public:
    return_interact    interactRPC(const std::string& messageIn) override;
    bool               setLanguageRPC(const std::string& language) override;
    return_getLanguage getLanguageRPC() override;
    bool               resetBotRPC() override;

public:
    void setInterfaces(yarp::dev::IChatBot* iser) { m_iChatBot = iser;}
    void setOutputPort(yarp::os::Port* port) { m_output_port = port; }
    std::mutex* getMutex() { return &m_mutex; }
};


/**
 * @ingroup dev_impl_nws_yarp
 *
 * \brief `ChatBot_nws_yarp`: A wrapper for a plugin able to interact with a chatbot or implementing one.
 *
 * The network interface is composed by two ports.
 * When a text bottle is received on the input port, the attached subdevice sends it to the chatbot
 * and sends the bot resonse on the output port
 * The same functionality is available also via rpc port, which also provides additional functionalities,
 * such as setting the language, resetting the bot and performing backup and restore operations.
 *
 * Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value                | Required    | Description                                                           | Notes                                                                               |
 * |:--------------:|:--------------:|:-------:|:--------------:|:----------------------------:|:----------: |:---------------------------------------------------------------------:|:-----------------------------------------------------------------------------------:|
 * | name           |      -         | string  | -              |   /chatBot_nws               | No          | full name of the port opened by the device                            | MUST start with a '/' character, xxx/text:i, xxx/rpc, xxx/audio:o ports are opened  |
 */
class ChatBot_nws_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::dev::WrapperSingle,
        public yarp::os::PortReader
{
private:
    yarp::dev::IChatBot* m_iChatBot{ nullptr };
    yarp::os::Port       m_inputPort;
    yarp::os::Port       m_outputPort;
    yarp::os::RpcServer  m_thriftServerPort;
    IChatBotMsgsImpl     m_msgsImpl;

    yarp::os::PortReaderBuffer <yarp::os::Bottle> m_inputBuffer;
    ChatBotRPC_CallbackHelper*                    m_cbkHelper{ nullptr };

    // yarp::dev::IWrapper
    bool  attach(yarp::dev::PolyDriver* deviceToAttach) override;
    bool  detach() override;

private:
    bool closeMain();

public:
    ChatBot_nws_yarp() = default;
    ChatBot_nws_yarp(const ChatBot_nws_yarp&) = delete;
    ChatBot_nws_yarp(ChatBot_nws_yarp&&) = delete;
    ChatBot_nws_yarp& operator=(const ChatBot_nws_yarp&) = delete;
    ChatBot_nws_yarp& operator=(ChatBot_nws_yarp&&) = delete;
    virtual ~ChatBot_nws_yarp() override;

    bool open(yarp::os::Searchable& config) override;
    bool close() override;
    bool read(yarp::os::ConnectionReader& connection) override;
};

#endif // YARP_DEV_CHATBOT_NWS_YARP_H
