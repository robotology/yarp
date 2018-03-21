/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "ShmemCarrier.h"
#include "ShmemHybridStream.h"

#include <yarp/os/ConstString.h>
#include <cstdlib>


ShmemCarrier::ShmemCarrier() = default;

ShmemCarrier::~ShmemCarrier() = default;

yarp::os::Carrier* ShmemCarrier::create()
{
    return new ShmemCarrier();
}

yarp::os::ConstString ShmemCarrier::getName()
{
    return "shmem";
}

int ShmemCarrier::getSpecifierCode()
{
    // specifier codes are a very old yarp feature,
    // not necessary any more really, should be replaced.
    return 14;
}

bool ShmemCarrier::requireAck()
{
    return true;
}

bool ShmemCarrier::isConnectionless()
{
    return false;
}

bool ShmemCarrier::checkHeader(const yarp::os::Bytes& header)
{
    return getSpecifier(header) % 16 == getSpecifierCode();
}

void ShmemCarrier::getHeader(const yarp::os::Bytes& header)
{
    createStandardHeader(getSpecifierCode(), header);
}

void ShmemCarrier::setParameters(const yarp::os::Bytes& header)
{
    YARP_UNUSED(header);
}

bool ShmemCarrier::becomeShmemVersionHybridStream(yarp::os::ConnectionState& proto, bool sender)
{
    ShmemHybridStream* stream = new ShmemHybridStream();
    yAssert(stream != nullptr);
    yarp::os::Contact base;

    bool ok = true;

    if (!sender) {
        ACE_INET_Addr anywhere((u_short)0, (ACE_UINT32)INADDR_ANY);
        base = yarp::os::Contact(anywhere.get_host_addr(), anywhere.get_port_number());
        bool ok = stream->open(base, sender) == 0;
        if (ok) {
            int myPort = stream->getLocalAddress().getPort();
            writeYarpInt(myPort, proto);
            stream->accept();
            proto.takeStreams(nullptr);
            proto.takeStreams(stream);
        }
    } else {
        int altPort = readYarpInt(proto);
        yarp::os::ConstString myName = proto.getStreams().getLocalAddress().getHost();
        proto.takeStreams(nullptr);
        base = yarp::os::Contact(myName, altPort);
        ok = stream->open(base, sender) == 0;
        if (ok) {
            proto.takeStreams(stream);
        }
    }

    if (!ok) {
        delete stream;
        stream = nullptr;
        return false;
    }

    return true;
}

bool ShmemCarrier::becomeShmem(yarp::os::ConnectionState& proto, bool sender)
{
    return becomeShmemVersionHybridStream(proto, sender);
}

bool ShmemCarrier::respondToHeader(yarp::os::ConnectionState& proto)
{
    // i am the receiver
    return becomeShmem(proto, false);
}

bool ShmemCarrier::expectReplyToHeader(yarp::os::ConnectionState& proto)
{
    // i am the sender
    return becomeShmem(proto, true);
}
