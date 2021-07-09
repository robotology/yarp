/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/NullConnection.h>

#include <yarp/os/Bytes.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/PortWriter.h>

using yarp::os::NullConnection;
using yarp::os::PortReader;
using yarp::os::PortWriter;


bool NullConnection::isValid() const
{
    return false;
}
bool NullConnection::isTextMode() const
{
    return true;
}
bool NullConnection::canEscape() const
{
    return true;
}
void NullConnection::handleEnvelope(const std::string& envelope)
{
    YARP_UNUSED(envelope);
}
bool NullConnection::requireAck() const
{
    return false;
}
bool NullConnection::supportReply() const
{
    return false;
}
bool NullConnection::isLocal() const
{
    return false;
}
bool NullConnection::isPush() const
{
    return true;
}
bool NullConnection::isConnectionless() const
{
    return false;
}
bool NullConnection::isBroadcast() const
{
    return false;
}
bool NullConnection::isActive() const
{
    return false;
}

bool NullConnection::modifiesIncomingData() const
{
    return false;
}

yarp::os::ConnectionReader& NullConnection::modifyIncomingData(yarp::os::ConnectionReader& reader)
{
    return reader;
}

bool NullConnection::acceptIncomingData(yarp::os::ConnectionReader& reader)
{
    YARP_UNUSED(reader);
    return true;
}

bool NullConnection::modifiesOutgoingData() const
{
    return false;
}

const PortWriter& NullConnection::modifyOutgoingData(const PortWriter& writer)
{
    return writer;
}

bool NullConnection::acceptOutgoingData(const PortWriter& writer)
{
    YARP_UNUSED(writer);
    return true;
}

bool NullConnection::modifiesReply() const
{
    return false;
}

PortReader& NullConnection::modifyReply(PortReader& reader)
{
    return reader;
}

void NullConnection::setCarrierParams(const yarp::os::Property& params)
{
    YARP_UNUSED(params);
}

void NullConnection::getCarrierParams(yarp::os::Property& params) const
{
    YARP_UNUSED(params);
}

void NullConnection::getHeader(yarp::os::Bytes& header) const
{
    for (size_t i = 0; i < header.length(); i++) {
        header.get()[i] = '\0';
    }
}

void NullConnection::prepareDisconnect()
{
}

std::string NullConnection::getName() const
{
    return "null";
}
