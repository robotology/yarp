/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/RpcClient.h>

#include <yarp/os/impl/LogComponent.h>

using namespace yarp::os;
using namespace yarp::os::impl;

namespace {
YARP_OS_LOG_COMPONENT(RPCCLIENT, "yarp.os.RpcClient")
} // namespace

class RpcClient::Private
{
public:
    // an RpcClient may be implemented with a regular port
    Port port;
};


RpcClient::RpcClient() :
        mPriv(new Private)
{
    mPriv->port.setInputMode(false);
    mPriv->port.setOutputMode(true);
    mPriv->port.setRpcMode(true);
}

RpcClient::~RpcClient()
{
    mPriv->port.close();
    delete mPriv;
}

bool RpcClient::read(PortReader& reader, bool willReply)
{
    YARP_UNUSED(reader);
    YARP_UNUSED(willReply);
    yCError(RPCCLIENT,
            "cannot read from RpcClient %s, please use a regular Port for that",
            mPriv->port.getName().c_str());
    return false;
}

bool RpcClient::reply(PortWriter& writer)
{
    YARP_UNUSED(writer);
    return false;
}

bool RpcClient::replyAndDrop(PortWriter& writer)
{
    YARP_UNUSED(writer);
    return false;
}

void RpcClient::setInputMode(bool expectInput)
{
    yCAssert(RPCCLIENT, !expectInput);
}

void RpcClient::setOutputMode(bool expectOutput)
{
    yCAssert(RPCCLIENT, expectOutput);
}

void RpcClient::setRpcMode(bool expectRpc)
{
    yCAssert(RPCCLIENT, expectRpc);
}

Port& RpcClient::asPort()
{
    return mPriv->port;
}

const Port& RpcClient::asPort() const
{
    return mPriv->port;
}
