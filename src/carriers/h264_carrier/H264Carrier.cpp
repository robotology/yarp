/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "H264Carrier.h"
#include "H264Stream.h"
#include <yarp/os/Contact.h>
#include <yarp/os/impl/FakeFace.h>
#include <yarp/os/Name.h>
#include <yarp/os/ConnectionState.h>
#include <cstdio>


using namespace yarp::os;
using namespace yarp::sig;



std::string H264Carrier::getName() const
{
    return "h264";
}

bool H264Carrier::isConnectionless() const
{
    return true;
}

bool H264Carrier::canAccept() const
{
    return true;
}

bool H264Carrier::canOffer() const
{
    return true;
}

bool H264Carrier::isTextMode() const
{
    return false;
}

bool H264Carrier::canEscape() const
{
    return false;
}

void H264Carrier::handleEnvelope(const std::string& envelope)
{
    this->envelope = envelope;
}

bool H264Carrier::requireAck() const
{
    return false;
}

bool H264Carrier::supportReply() const
{
    return false;
}

bool H264Carrier::isLocal() const
{
    return false;
}

// this is important - flips expected flow of messages
bool H264Carrier::isPush() const
{
    return false;
}

std::string H264Carrier::toString() const
{
    return "h264_carrier";
}

void H264Carrier::getHeader(Bytes& header) const
{
}

bool H264Carrier::checkHeader(const Bytes& header)
{
    return true;
}

void H264Carrier::setParameters(const Bytes& header)
{
    // no parameters - no carrier variants
    printf("h264: setParameters \n");
}


static int getIntParam(Name &n, const char *param)
{
    bool hasField;
    std::string strValue = n.getCarrierModifier(param, &hasField);
    Value *v = Value::makeValue(strValue);
    int intvalue = 0;
    if((hasField) && v->isInt32())
    {
        intvalue = v->asInt32();
    }

    delete v;
    return intvalue;
}

// Now, the initial hand-shaking
bool H264Carrier::prepareSend(ConnectionState& proto)
{
    //get all parameters of this carrier
    Name n(proto.getRoute().getCarrierName() + "://test");

    cfgParams.crop.left = getIntParam(n, "cropLeft");
    cfgParams.crop.right = getIntParam(n, "cropRight");
    cfgParams.crop.top = getIntParam(n, "cropTop");
    cfgParams.crop.bottom = getIntParam(n, "cropBottom");
    cfgParams.fps_max = getIntParam(n, "max_fps");
    cfgParams.verbose = (getIntParam(n, "verbose") >0) ? true: false;
    cfgParams.removeJitter = (getIntParam(n, "removeJitter") > 0) ? true : false;
    return true;
}

bool H264Carrier::sendHeader(ConnectionState& proto)
{
    //printf("h264: sendHeader \n");
    return true;
}

bool H264Carrier::expectSenderSpecifier(ConnectionState& proto)
{
    printf("h264: expectSenderSpecifier \n");
    return true;
}

bool H264Carrier::expectExtraHeader(ConnectionState& proto)
{
    printf("h264: expectExtraHeader \n");
    return true;
}

bool H264Carrier::respondToHeader(ConnectionState& proto)
{
    printf("h264: respondToHeader \n");
    return true;
}

bool H264Carrier::expectReplyToHeader(ConnectionState& proto)
{
    // I'm the receiver...

    cfgParams.remotePort = proto.getRoute().getToContact().getPort();

    auto* stream = new H264Stream(cfgParams);
    if (stream==nullptr) { return false; }

    yarp::os::Contact remote = proto.getStreams().getRemoteAddress();
    bool ok = stream->open(remote);

    //std::cout << "Remote contact info: host=" << proto.getRoute().getToContact().getHost() << " port= " << proto.getRoute().getToContact().getPort() <<std::endl;
    if (!ok)
    {
        delete stream;
        return false;
    }
    stream->start();

    proto.takeStreams(stream);
    return true;
}

bool H264Carrier::isActive() const
{
    return true;
}

bool H264Carrier::write(ConnectionState& proto, SizedWriter& writer)
{
    //I should not be here: the carried doesn't perform writing
    return false;
}

bool H264Carrier::reply(ConnectionState& proto, SizedWriter& writer)
{
    return false;
}

bool H264Carrier::sendIndex(ConnectionState& proto, SizedWriter& writer)
{
    return true;
}

bool H264Carrier::expectIndex(ConnectionState& proto)
{
    return true;
}

bool H264Carrier::sendAck(ConnectionState& proto)
{
    return true;
}

bool H264Carrier::expectAck(ConnectionState& proto)
{
    return true;
}

std::string H264Carrier::getBootstrapCarrierName() const
{
    return {};
}

yarp::os::Face* H264Carrier::createFace() const
{
    return new yarp::os::impl::FakeFace();
}

