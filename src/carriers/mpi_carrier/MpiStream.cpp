/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "MpiStream.h"

#include <yarp/os/Log.h>

#include <utility>

using namespace yarp::os;


/* --------------------------------------- */
/* MpiStream */

MpiStream::MpiStream(std::string n, MpiComm* c)
    : terminate(false), name(std::move(n)), comm(c) {
    readBuffer = nullptr;
    resetBuffer();

}
#ifdef MPI_DEBUG
MpiStream::~MpiStream() {
    printf("[MpiStream @ %s] Destructor\n", name.c_str());
}
#else
MpiStream::~MpiStream() = default;
#endif

void MpiStream::resetBuffer() {
    // reset buffer
    readAt = 0;
    readAvail = 0;
    delete [] readBuffer;
    readBuffer = nullptr;
}

bool MpiStream::isOk() const {
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
const yarp::os::Contact& MpiStream::getLocalAddress() const {
    // left undefined
    return local;
}
const yarp::os::Contact& MpiStream::getRemoteAddress() const {
    // left undefined
    return remote;
}
void MpiStream::beginPacket() {
     // nothing to do
}
void MpiStream::endPacket() {
     // nothing to do
}
