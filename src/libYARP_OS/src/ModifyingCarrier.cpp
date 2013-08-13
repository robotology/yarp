// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/impl/ModifyingCarrier.h>

using namespace yarp::os;

bool yarp::os::impl::ModifyingCarrier::checkHeader(const yarp::os::Bytes &header) {
    return false;
}


void yarp::os::impl::ModifyingCarrier::getHeader(const yarp::os::Bytes &header) {
    if (header.length()==8) {
        String target = "ohbehave";
        for (int i=0; i<8; i++) {
            header.get()[i] = target[i];
        }
    }
}

bool yarp::os::impl::ModifyingCarrier::respondToHeader(yarp::os::ConnectionState &proto) {
    return false;
}

bool yarp::os::impl::ModifyingCarrier::modifiesIncomingData() {
    return true;
}

void yarp::os::impl::ModifyingCarrier::setCarrierParams(const yarp::os::Property &params) {
}

void yarp::os::impl::ModifyingCarrier::getCarrierParams(yarp::os::Property &params) {
}
