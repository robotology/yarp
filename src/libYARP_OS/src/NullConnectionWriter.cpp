// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/NullConnectionWriter.h>


void yarp::os::NullConnectionWriter::appendBlock(const char *data, size_t len) {
}

void yarp::os::NullConnectionWriter::appendInt(int data) {
}

void yarp::os::NullConnectionWriter::appendInt64(const YARP_INT64& data) {
}

void yarp::os::NullConnectionWriter::appendDouble(double data) {
}

void yarp::os::NullConnectionWriter::appendString(const char *str, int terminate) {
}

void yarp::os::NullConnectionWriter::appendExternalBlock(const char *data, size_t len) {
}

bool yarp::os::NullConnectionWriter::isTextMode() {
    return false;
}

bool yarp::os::NullConnectionWriter::isBareMode() {
    return false;
}

void yarp::os::NullConnectionWriter::declareSizes(int argc, int *argv) {
}

void yarp::os::NullConnectionWriter::setReplyHandler(PortReader& reader) {
}

void yarp::os::NullConnectionWriter::setReference(Portable *obj) {
}

bool yarp::os::NullConnectionWriter::convertTextMode() {
 return false;
}

bool yarp::os::NullConnectionWriter::isValid() {
 return false;
}

bool yarp::os::NullConnectionWriter::isActive() {
 return true;
}

bool yarp::os::NullConnectionWriter::isError() {
 return true;
}

void yarp::os::NullConnectionWriter::requestDrop() {
}

bool yarp::os::NullConnectionWriter::isNull() const { 
    return true;
}

yarp::os::SizedWriter *yarp::os::NullConnectionWriter::getBuffer() {
    return NULL;
}

