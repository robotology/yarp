/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/UdpCarrier.h>

#include <yarp/os/ConnectionState.h>
#include <yarp/os/Log.h>

#include <string>

using namespace yarp::os;
using namespace yarp::os::impl;

yarp::os::impl::UdpCarrier::UdpCarrier() = default;

yarp::os::Carrier* yarp::os::impl::UdpCarrier::create() const
{
    return new UdpCarrier();
}

std::string yarp::os::impl::UdpCarrier::getName() const
{
    return "udp";
}

int yarp::os::impl::UdpCarrier::getSpecifierCode() const
{
    return 0;
}

bool yarp::os::impl::UdpCarrier::checkHeader(const Bytes& header)
{
    return getSpecifier(header) % 16 == getSpecifierCode();
}

void yarp::os::impl::UdpCarrier::getHeader(Bytes& header) const
{
    createStandardHeader(getSpecifierCode(), header);
}

void yarp::os::impl::UdpCarrier::setParameters(const Bytes& header)
{
    YARP_UNUSED(header);
}

bool yarp::os::impl::UdpCarrier::requireAck() const
{
    return false;
}

bool yarp::os::impl::UdpCarrier::isConnectionless() const
{
    return true;
}


bool yarp::os::impl::UdpCarrier::respondToHeader(ConnectionState& proto)
{
    // I am the receiver

    // issue: need a fresh port number...
    auto* stream = new DgramTwoWayStream();
    yAssert(stream != nullptr);

    Contact remote = proto.getStreams().getRemoteAddress();
    bool ok = stream->open(remote);
    if (!ok) {
        delete stream;
        return false;
    }

    int myPort = stream->getLocalAddress().getPort();
    writeYarpInt(myPort, proto);
    proto.takeStreams(stream);

    return true;
}

bool yarp::os::impl::UdpCarrier::expectReplyToHeader(ConnectionState& proto)
{
    // I am the sender
    int myPort = proto.getStreams().getLocalAddress().getPort();
    std::string myName = proto.getStreams().getLocalAddress().getHost();
    std::string altName = proto.getStreams().getRemoteAddress().getHost();

    int altPort = readYarpInt(proto);

    if (altPort == -1) {
        return false;
    }

    auto* stream = new DgramTwoWayStream();
    yAssert(stream != nullptr);

    proto.takeStreams(nullptr); // free up port from tcp
    bool ok = stream->open(Contact(myName, myPort), Contact(altName, altPort));
    if (!ok) {
        delete stream;
        return false;
    }
    proto.takeStreams(stream);
    return true;
}
