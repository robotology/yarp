/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ChatBot_nws_yarp.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Os.h>

namespace {
YARP_LOG_COMPONENT(CHATBOT_NWS_YARP, "yarp.devices.chatBot_nws_yarp")
}

ChatBot_nws_yarp::~ChatBot_nws_yarp()
{
    closeMain();
}

bool ChatBot_nws_yarp::close()
{
    return closeMain();
}

bool ChatBot_nws_yarp::open(Searchable& prop)
{
    std::string rootName =
        prop.check("name",Value("/chatBot_nws"),
                    "prefix for port names").asString();

    m_inputBuffer.attach(m_inputPort);
    m_inputPort.open(rootName+"/text:i");
    m_outputPort.open(rootName + "/text:o");
    if (!m_thriftServerPort.open(rootName+"/rpc"))
    {
        yCError(CHATBOT_NWS_YARP, "Failed to open rpc port");
        return false;
    }
    m_thriftServerPort.setReader(*this);

    yCInfo(CHATBOT_NWS_YARP, "Device waiting for attach...");
    return true;
}

bool  ChatBot_nws_yarp::attach(yarp::dev::PolyDriver* deviceToAttach)
{
    if (deviceToAttach->isValid())
    {
        deviceToAttach->view(m_iChatBot);
    }

    if (nullptr == m_iChatBot)
    {
        yCError(CHATBOT_NWS_YARP, "Subdevice passed to attach method is invalid");
        return false;
    }

    yCInfo(CHATBOT_NWS_YARP, "Attach done");

    m_cbkHelper = new ChatBotRPC_CallbackHelper(m_iChatBot,&m_outputPort);
    m_inputBuffer.useCallback(*m_cbkHelper);
    m_msgsImpl.setInterfaces(m_iChatBot);
    m_msgsImpl.setOutputPort(&m_outputPort);

    return true;
}

bool  ChatBot_nws_yarp::detach()
{
    m_iChatBot = nullptr;
    return true;
}

bool ChatBot_nws_yarp::closeMain()
{
    //close the port connection here
    m_inputBuffer.disableCallback();
    m_inputPort.close();
    m_outputPort.close();
    m_thriftServerPort.close();
    if (m_cbkHelper) {delete m_cbkHelper; m_cbkHelper=nullptr;}
    return true;
}

bool ChatBot_nws_yarp::read(yarp::os::ConnectionReader& connection)
{
    bool b = m_msgsImpl.read(connection);
    if (b)
    {
        return true;
    }
    else
    {
        yCError(CHATBOT_NWS_YARP, "read() Command failed");
        return false;
    }
}

