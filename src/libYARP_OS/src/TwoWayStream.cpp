// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/impl/TwoWayStream.h>


yarp::os::impl::TwoWayStream::~TwoWayStream() {
}

yarp::os::impl::NullStream::~NullStream() {
}

yarp::os::impl::InputStream& yarp::os::impl::NullStream::getInputStream() {
    return *this;
}

yarp::os::impl::OutputStream& yarp::os::impl::NullStream::getOutputStream() {
    return * this;
}

const yarp::os::impl::Address& yarp::os::impl::NullStream::getLocalAddress() {
    return address;
}

const yarp::os::impl::Address& yarp::os::impl::NullStream::getRemoteAddress() {
    return address;
}

bool yarp::os::impl::NullStream::isOk() {
    return false;
}

void yarp::os::impl::NullStream::reset() {
}

void yarp::os::impl::NullStream::close() {
}

void yarp::os::impl::NullStream::beginPacket() {
}

void yarp::os::impl::NullStream::endPacket()   {
}

ssize_t yarp::os::impl::NullStream::read(const Bytes& b) {
    return -1;
}

void yarp::os::impl::NullStream::write(const Bytes& b) {
}
