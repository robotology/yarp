// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Author: Daniel Krieg krieg@fias.uni-frankfurt.de
 * Copyright (C) 2010 Daniel Krieg
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifdef CREATE_MPI_CARRIER

#include <yarp/os/impl/MpiStream.h>
#include <yarp/os/impl/Logger.h>

using namespace yarp::os::impl;


/* --------------------------------------- */
/* MpiStream */

MpiStream::MpiStream(String n, MpiComm* c)
    : terminate(false), name(n), comm(c) {
    readBuffer = NULL;
    resetBuffer();

}
MpiStream::~MpiStream() {
    #ifdef MPI_DEBUG
    printf("[MpiStream @ %s] Destructor\n", name.c_str());
    #endif
}

void MpiStream::resetBuffer() {
    // reset buffer
    readAt = 0;
    readAvail = 0;
    delete [] readBuffer;
    readBuffer = NULL;
}

bool MpiStream::isOk() {
    return !terminate;
}

 void MpiStream::interrupt() {
    #ifdef MPI_DEBUG
    printf("[MpiStream @ %s] Trying to interrupt\n", name.c_str());
    #endif
    terminate = true;
}

/* --------------------------------------- */
/* TwoWayStream */

InputStream& MpiStream::getInputStream() {
    return *this;
}
yarp::os::OutputStream& MpiStream::getOutputStream() {
    return *this;
}
const Address& MpiStream::getLocalAddress() {
    // left undefined
    return local;
}
const Address& MpiStream::getRemoteAddress() {
    // left undefined
    return remote;
}
void MpiStream::beginPacket() {
     // nothing to do
}
void MpiStream::endPacket() {
     // nothing to do
}



#else

extern "C" int MpiStreamStatus() {
    return 0;
}

#endif
