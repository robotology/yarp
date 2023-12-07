/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ChatBot_nws_yarp.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Os.h>

namespace {
YARP_LOG_COMPONENT(CHATBOTRPC_HELPER, "yarp.devices.chatBot_nws_yarp.ChatBotRPC_CallbackHelper")
}

bool ChatBotRPC_CallbackHelper::setCommunications(yarp::dev::IChatBot* iChat, yarp::os::Port* p)
{
    if (iChat ==nullptr || p==nullptr)
    {
        yCError(CHATBOTRPC_HELPER) << "Could not get IChatBot interface/output port";
        return false;
    }
    m_iChatBot = iChat;
    m_outPort = p;

    return true;
}

void ChatBotRPC_CallbackHelper::onRead(yarp::os::Bottle &b)
{
    if (m_iChatBot)
    {
        std::string response;
        bool ok = m_iChatBot->interact(b.get(0).asString(), response);

        yCWarning(CHATBOTRPC_HELPER) << "Sent:" << b.get(0).asString() << "Got" << response;

        yarp::os::Bottle toSend;
        toSend.addString(response);
        if (!ok)
        {
            yCError(CHATBOTRPC_HELPER) << "Problems during interaction with the chatBot";
        }
        else
        {
            if (m_outPort && m_outPort->getOutputCount() > 0)
            {
                m_outPort->write(toSend);
            }
        }
    }
    else
    {
        yCError(CHATBOTRPC_HELPER)<< "IChatBot interface was not set";
    }
}
