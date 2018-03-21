/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/TwoWayStream.h>

using namespace yarp::os;

TwoWayStream::~TwoWayStream() {
}

NullStream::~NullStream() {
}

InputStream& NullStream::getInputStream() {
    return *this;
}

OutputStream& NullStream::getOutputStream() {
    return * this;
}

const Contact& NullStream::getLocalAddress() {
    return address;
}

const Contact& NullStream::getRemoteAddress() {
    return address;
}

bool NullStream::isOk() {
    return false;
}

void NullStream::reset() {
}

void NullStream::close() {
}

void NullStream::beginPacket() {
}

void NullStream::endPacket()   {
}

YARP_SSIZE_T NullStream::read(const Bytes& b) {
    YARP_UNUSED(b);
    return -1;
}

void NullStream::write(const Bytes& b) {
    YARP_UNUSED(b);
}
