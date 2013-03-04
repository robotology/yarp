// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/NullConnectionReader.h>


bool yarp::os::NullConnectionReader::expectBlock(const char *data, size_t len) {
    return false;
}

yarp::os::ConstString yarp::os::NullConnectionReader::expectText(int terminatingChar) {
    return "";
}

int yarp::os::NullConnectionReader::expectInt() {
    return 0;
}

bool yarp::os::NullConnectionReader::pushInt(int x) {
    return false;
}

double yarp::os::NullConnectionReader::expectDouble() {
    return 0.0;
}

bool yarp::os::NullConnectionReader::isTextMode() {
    return false;
}

bool yarp::os::NullConnectionReader::convertTextMode() {
    return false;
}

size_t yarp::os::NullConnectionReader::getSize() {
    return 0;
}

yarp::os::ConnectionWriter *yarp::os::NullConnectionReader::getWriter() {
    return NULL;
}

yarp::os::Bytes yarp::os::NullConnectionReader::readEnvelope() {
    return Bytes(0,0);
}

yarp::os::Portable *yarp::os::NullConnectionReader::getReference() {
    return NULL;
}

yarp::os::Contact yarp::os::NullConnectionReader::getRemoteContact() {
    return Contact();
}

yarp::os::Contact yarp::os::NullConnectionReader::getLocalContact() {
    return Contact();
}

bool yarp::os::NullConnectionReader::isValid() {
    return false;
}

bool yarp::os::NullConnectionReader::isActive() {
    return false;
}

bool yarp::os::NullConnectionReader::isError() {
    return true;
}

void yarp::os::NullConnectionReader::requestDrop() {
}

yarp::os::Searchable& yarp::os::NullConnectionReader::getConnectionModifiers() {
    return blank;
}
