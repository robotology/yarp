/*
 * Copyright (C) 2012 Istituto Italiano di Tecnologia (IIT)
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/ModifyingCarrier.h>

using namespace yarp::os;

bool yarp::os::ModifyingCarrier::checkHeader(const yarp::os::Bytes &header)
{
    YARP_UNUSED(header);
    return false;
}


void yarp::os::ModifyingCarrier::getHeader(const yarp::os::Bytes &header)
{
    if (header.length()==8) {
        ConstString target = "ohbehave";
        for (int i=0; i<8; i++) {
            header.get()[i] = target[i];
        }
    }
}

bool yarp::os::ModifyingCarrier::respondToHeader(yarp::os::ConnectionState &proto)
{
    YARP_UNUSED(proto);
    return false;
}

bool yarp::os::ModifyingCarrier::modifiesIncomingData()
{
    return true;
}

bool yarp::os::ModifyingCarrier::modifiesOutgoingData()
{
    return true;
}

bool yarp::os::ModifyingCarrier::modifiesReply()
{
    return true;
}


void yarp::os::ModifyingCarrier::setCarrierParams(const yarp::os::Property &params)
{
    YARP_UNUSED(params);
}

void yarp::os::ModifyingCarrier::getCarrierParams(yarp::os::Property &params)
{
    YARP_UNUSED(params);
}

bool yarp::os::ModifyingCarrier::configureFromProperty(yarp::os::Property& prop)
{
    YARP_UNUSED(prop);
    return false;
}
