// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <stdio.h>

#include "yarp.h"
#include "yarpimpl.h"

YARP_DEFINE(yarpNetworkPtr) yarpNetworkCreate() {
    yarpNetworkPtr network = new yarpNetwork;
    if (network!=NULL) {
        network->implementation = new Network();
        if (network->implementation==NULL) {
            delete network;
            network = NULL;
        }
    }
    return network;
}


YARP_DEFINE(void) yarpNetworkFree(yarpNetworkPtr network) {
    if (network!=NULL) {
        if (network->implementation!=NULL) {
            delete (Network*)(network->implementation);
            network->implementation = NULL;
        }
        delete network;
    }
}
