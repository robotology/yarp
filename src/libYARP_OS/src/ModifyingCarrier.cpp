// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/ModifyingCarrier.h>

using namespace yarp::os;

bool yarp::os::ModifyingCarrier::checkHeader(const yarp::os::Bytes &header) {
    return false;
}


void yarp::os::ModifyingCarrier::getHeader(const yarp::os::Bytes &header) {
    if (header.length()==8) {
        ConstString target = "ohbehave";
        for (int i=0; i<8; i++) {
            header.get()[i] = target[i];
        }
    }
}

bool yarp::os::ModifyingCarrier::respondToHeader(yarp::os::ConnectionState &proto) {
    return false;
}

bool yarp::os::ModifyingCarrier::modifiesIncomingData() {
    return true;
}

bool yarp::os::ModifyingCarrier::modifiesOutgoingData() {
    return true;
}

bool yarp::os::ModifyingCarrier::modifiesReply() {
    return true;
}


void yarp::os::ModifyingCarrier::setCarrierParams(const yarp::os::Property &params) {
}

void yarp::os::ModifyingCarrier::getCarrierParams(yarp::os::Property &params) {
}
