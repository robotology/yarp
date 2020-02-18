/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "UnixSocketCarrier.h"

#include <yarp/os/ConnectionState.h>
#include <yarp/os/Log.h>

using namespace yarp::os;

yarp::os::Carrier *UnixSocketCarrier::create() const
{
    return new UnixSocketCarrier();
}

std::string UnixSocketCarrier::getName() const
{
    return name;
}

bool UnixSocketCarrier::requireAck() const
{
    return false;
}

bool UnixSocketCarrier::isConnectionless() const
{
    return true;
}

bool UnixSocketCarrier::canEscape() const
{
    return true;
}

bool UnixSocketCarrier::isUnixSockSupported(ConnectionState& proto)
{
    yarp::os::Contact remote = proto.getStreams().getRemoteAddress();
    yarp::os::Contact local  = proto.getStreams().getLocalAddress();

    if(remote.getHost() != local.getHost())
    {
        yError("UnixSocketCarrier: The ports are on different machines, unix socket not supported...");
        return false;
    }
    return true;

}

bool UnixSocketCarrier::checkHeader(const Bytes& header)
{
    if (header.length() != headerSize) {
        return false;
    }
    const char *target = headerCode;
    for (size_t i = 0; i < headerSize; i++) {
        if (header.get()[i] != target[i]) {
            return false;
        }
    }
    return true;
}

void UnixSocketCarrier::getHeader(Bytes& header) const
{
    const char *target = headerCode;
    for (size_t i = 0; i < headerSize && i < header.length(); i++)
    {
        header.get()[i] = target[i];
    }
}

bool UnixSocketCarrier::sendIndex(ConnectionState& proto, SizedWriter& writer)
{
    return true;
}

bool UnixSocketCarrier::expectIndex(ConnectionState& proto)
{
    return true;
}



bool UnixSocketCarrier::respondToHeader(ConnectionState& proto)
{
    // I am the receiver
    return becomeUnixSocket(proto, false);
}

bool UnixSocketCarrier::expectReplyToHeader(ConnectionState& proto)
{
    // I am the sender
    return becomeUnixSocket(proto, true);
}

bool UnixSocketCarrier::becomeUnixSocket(ConnectionState &proto, bool sender)
{
    Contact remote = proto.getStreams().getRemoteAddress();
    Contact local  = proto.getStreams().getLocalAddress();

    proto.takeStreams(YARP_NULLPTR); // free up port from tcp

    if(sender)
    {
        socketPath = "/tmp/yarp-" + std::to_string(remote.getPort()) + "_" + std::to_string(local.getPort())+".sock";
    }
    else
    {
        socketPath = "/tmp/yarp-" + std::to_string(local.getPort()) + "_" + std::to_string(remote.getPort())+".sock";

    }

    if (!socketPath.empty())
    {
        stream = new UnixSockTwoWayStream(socketPath);
    }
    else
    {
        return false;
    }

    stream->setLocalAddress(local);
    stream->setRemoteAddress(remote);


    if (!stream->open(sender))
    {
        delete stream;
        stream = YARP_NULLPTR;
        return false;
    }
    yAssert(stream!=YARP_NULLPTR);

    proto.takeStreams(stream);
    return true;

}

