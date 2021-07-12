/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * SPDX-License-Identifier: BSD-3-Clause
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

MpiStream::~MpiStream() {
    yCTrace(MPI_CARRIER, "[MpiStream @ %s] Destructor", name.c_str());
}

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
    yCDebug(MPI_CARRIER, "[MpiStream @ %s] Trying to interrupt", name.c_str());
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
