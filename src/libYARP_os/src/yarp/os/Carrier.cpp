/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Carrier.h>
#include <yarp/os/impl/TcpFace.h>
#include <yarp/os/ConnectionState.h>

using namespace yarp::os;

bool Carrier::isBroadcast() const
{
    return false;
}

void Carrier::handleEnvelope(const std::string& envelope)
{
    YARP_UNUSED(envelope);
}

bool Carrier::isPush() const
{
    return true;
}

bool Carrier::reply(ConnectionState& proto, SizedWriter& writer)
{
    writer.write(proto.os());
    return proto.os().isOk();
}

std::string Carrier::getBootstrapCarrierName() const
{
    return "tcp";
}

int Carrier::connect(const Contact& src,
                     const Contact& dest,
                     const ContactStyle& style,
                     int mode,
                     bool reversed)
{
    YARP_UNUSED(src);
    YARP_UNUSED(dest);
    YARP_UNUSED(style);
    YARP_UNUSED(mode);
    YARP_UNUSED(reversed);
    return -1;
}

bool Carrier::modifiesIncomingData() const
{
    return false;
}


ConnectionReader& Carrier::modifyIncomingData(ConnectionReader& reader)
{
    return reader;
}

bool Carrier::acceptIncomingData(ConnectionReader& reader)
{
    YARP_UNUSED(reader);
    return true;
}


bool Carrier::modifiesOutgoingData() const
{
    return false;
}


const PortWriter& Carrier::modifyOutgoingData(const PortWriter& writer)
{
    return writer;
}

bool Carrier::modifiesReply() const
{
    return false;
}

void Carrier::prepareDisconnect()
{
}

void Carrier::close()
{
}

Carrier::~Carrier() = default;

PortReader& Carrier::modifyReply(PortReader& reader)
{
    return reader;
}

bool Carrier::acceptOutgoingData(const PortWriter& writer)
{
    YARP_UNUSED(writer);
    return true;
}


bool Carrier::configure(ConnectionState& proto)
{
    YARP_UNUSED(proto);
    return true;
}

bool Carrier::configureFromProperty(yarp::os::Property& options)
{
    YARP_UNUSED(options);
    return true;
}


void Carrier::setCarrierParams(const Property& params)
{
    YARP_UNUSED(params);
}

void Carrier::getCarrierParams(Property& params) const
{
    YARP_UNUSED(params);
}

yarp::os::Face* Carrier::createFace() const
{
    return new yarp::os::impl::TcpFace();
}
