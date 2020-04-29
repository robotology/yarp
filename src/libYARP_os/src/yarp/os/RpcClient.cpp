/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/RpcClient.h>

#include <yarp/os/LogComponent.h>

using namespace yarp::os;
using namespace yarp::os::impl;

namespace {
YARP_LOG_COMPONENT(RPCCLIENT,
                   "yarp.os.RpcServer",
                   yarp::os::Log::InfoType,
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::defaultPrintCallback(),
                   nullptr)
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
    yAssert(!expectInput);
}

void RpcClient::setOutputMode(bool expectOutput)
{
    yAssert(expectOutput);
}

void RpcClient::setRpcMode(bool expectRpc)
{
    yAssert(expectRpc);
}

Port& RpcClient::asPort()
{
    return mPriv->port;
}

const Port& RpcClient::asPort() const
{
    return mPriv->port;
}
