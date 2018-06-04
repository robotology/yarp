/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Carrier.h>
#include <yarp/os/impl/TcpFace.h>

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

std::string Carrier::getBootstrapCarrierName()
{
    return "tcp";
}

int  Carrier::connect(const Contact& src,
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

bool Carrier::modifiesIncomingData()
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


bool Carrier::modifiesOutgoingData()
{
    return false;
}


PortWriter& Carrier::modifyOutgoingData(PortWriter& writer)
{
    return writer;
}

bool Carrier::modifiesReply()
{
    return false;
}

void Carrier::prepareDisconnect()
{;}

void Carrier::close()
{;}

Carrier::~Carrier()
{;}

PortReader& Carrier::modifyReply(PortReader& reader)
{
    return reader;
}

bool Carrier::acceptOutgoingData(PortWriter& writer)
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

void Carrier::getCarrierParams(Property& params)
{
    YARP_UNUSED(params);
}

yarp::os::Face* Carrier::createFace(void)
{
    return new yarp::os::impl::TcpFace();
}
