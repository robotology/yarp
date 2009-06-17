// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <stdlib.h>
#include <yarp/os/impl/McastCarrier.h>
#include <yarp/os/impl/Logger.h>

using namespace yarp::os::impl;

ElectionOf<McastCarrier> *McastCarrier::caster = NULL;

ElectionOf<McastCarrier>& McastCarrier::getCaster() {
    if (caster==NULL) {
        caster = new ElectionOf<McastCarrier>;
        if (caster==NULL) {
            YARP_ERROR(Logger::get(), "No memory for McastCarrier::caster");
            exit(1);
        }
    }
    return *caster;
}






