// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
