/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/MpiStream.h>
#include <yarp/os/Log.h>

using namespace yarp::os;


/* --------------------------------------- */
/* MpiStream */

MpiStream::MpiStream(std::string n, MpiComm* c)
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
const yarp::os::Contact& MpiStream::getLocalAddress() {
    // left undefined
    return local;
}
const yarp::os::Contact& MpiStream::getRemoteAddress() {
    // left undefined
    return remote;
}
void MpiStream::beginPacket() {
     // nothing to do
}
void MpiStream::endPacket() {
     // nothing to do
}
