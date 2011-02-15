// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Author: Daniel Krieg krieg@fias.uni-frankfurt.de
 * Copyright (C) 2010 Daniel Krieg
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifdef CREATE_MPI_CARRIER

#include <yarp/os/impl/MpiBcastCarrier.h>
#include <yarp/os/Network.h>

using namespace yarp::os::impl;
using namespace yarp::os;

MpiBcastCarrier::~MpiBcastCarrier() {
    #ifdef MPI_DEBUG
    printf("[MpiBcastCarrier @ %s] Destructor\n", name.c_str());
    #endif
}

void MpiBcastCarrier::close() {
    #ifdef MPI_DEBUG
    printf("[MpiBcastCarrier @ %s] Closing carrier \n", name.c_str() );
    #endif
    if (electionMember) {
        getCaster().remove(name, this);
        MpiBcastCarrier* elect = getCaster().getElect(name);
        if (elect == NULL)
            delete comm;
    }
}

void MpiBcastCarrier::createStream(String name) {

    MpiBcastCarrier* elect = getCaster().getElect(name);
    if (elect != NULL) {
        comm = elect->comm;
    }
    else {
        comm = new MpiComm(name);
    }
    stream = new MpiBcastStream(name, comm);
    dynamic_cast<MpiBcastStream*> (stream)->startJoin();

    getCaster().add(name, this);
    electionMember = true;

}




/*
 * Adopted from MCastCarrier
 * ----------------------------
 */
ElectionOf<MpiBcastCarrier> *MpiBcastCarrier::caster = NULL;

ElectionOf<MpiBcastCarrier>& MpiBcastCarrier::getCaster() {
    yarp::os::NetworkBase::lock();
    if (caster==NULL) {
        caster = new ElectionOf<MpiBcastCarrier>;
        yarp::os::NetworkBase::unlock();
        if (caster==NULL) {
            YARP_ERROR(Logger::get(), "No memory for MpiBcastCarrier::caster");
            exit(1);
        }
    } else {
        yarp::os::NetworkBase::unlock();
    }
    return *caster;
}
bool MpiBcastCarrier::isElect() {
    MpiBcastCarrier *elect = getCaster().getElect(name);
    return elect==this || elect==NULL;
}

bool MpiBcastCarrier::isActive() {
    return isElect();
}

/*
 * ----------------------------
 */

#endif
