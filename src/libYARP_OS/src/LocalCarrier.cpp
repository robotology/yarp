// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/os/impl/LocalCarrier.h>

using namespace yarp::os::impl;

LocalCarrierManager LocalCarrier::manager;


void LocalCarrierStream::close() {
    SocketTwoWayStream::close();
    if (owner!=NULL) {
        LocalCarrier *owned = owner;
        owner = NULL;
        owned->shutdown();
    }
    done = true;
}

