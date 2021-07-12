/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/TcpCarrier.h>

#include <yarp/os/ConnectionState.h>
#include <yarp/os/TwoWayStream.h>

#include <string>

using namespace yarp::os;


yarp::os::impl::TcpCarrier::TcpCarrier(bool requireAckFlag)
{
    this->requireAckFlag = requireAckFlag;
}

yarp::os::Carrier* yarp::os::impl::TcpCarrier::create() const
{
    return new TcpCarrier(requireAckFlag);
}

std::string yarp::os::impl::TcpCarrier::getName() const
{
    return requireAckFlag ? "tcp" : "fast_tcp";
}

int yarp::os::impl::TcpCarrier::getSpecifierCode() const
{
    return 3;
}

bool yarp::os::impl::TcpCarrier::checkHeader(const yarp::os::Bytes& header)
{
    int spec = getSpecifier(header);
    if (spec % 16 == getSpecifierCode()) {
        if (((spec & 128) != 0) == requireAckFlag) {
            return true;
        }
    }
    return false;
}

void yarp::os::impl::TcpCarrier::getHeader(yarp::os::Bytes& header) const
{
    createStandardHeader(getSpecifierCode() + (requireAckFlag ? 128 : 0), header);
}

void yarp::os::impl::TcpCarrier::setParameters(const yarp::os::Bytes& header)
{
    YARP_UNUSED(header);
    //int specifier = getSpecifier(header);
    //requireAckFlag = (specifier&128)!=0;
    // Now prefilter by ack flag
}

bool yarp::os::impl::TcpCarrier::requireAck() const
{
    return requireAckFlag;
}

bool yarp::os::impl::TcpCarrier::isConnectionless() const
{
    return false;
}

bool yarp::os::impl::TcpCarrier::respondToHeader(ConnectionState& proto)
{
    int cport = proto.getStreams().getLocalAddress().getPort();
    writeYarpInt(cport, proto);
    return proto.checkStreams();
}

bool yarp::os::impl::TcpCarrier::expectReplyToHeader(ConnectionState& proto)
{
    readYarpInt(proto); // ignore result
    return proto.checkStreams();
}
