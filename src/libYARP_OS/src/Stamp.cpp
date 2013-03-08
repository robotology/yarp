// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/String.h>

#include <yarp/os/Stamp.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>

#include <yarp/os/impl/IOException.h>


yarp::os::Stamp::Stamp(int count, double time) {
    sequenceNumber = count;
    timeStamp = time;
}

yarp::os::Stamp::Stamp() {
    sequenceNumber = -1;
    timeStamp = 0;
}

int yarp::os::Stamp::getCount() {
    return sequenceNumber;
}

double yarp::os::Stamp::getTime() {
    return timeStamp;
}

bool yarp::os::Stamp::isValid() {
    return sequenceNumber>=0;
}

bool yarp::os::Stamp::read(ConnectionReader& connection) {
    connection.convertTextMode();
    int header = connection.expectInt();
    if (header!=BOTTLE_TAG_LIST) {
        return false;
    }
    int len = connection.expectInt();
    if (len!=2) {
        return false;
    }
    int code;
    code = connection.expectInt();
    if (code!=BOTTLE_TAG_INT) {
        return false;
    }
    sequenceNumber = connection.expectInt();
    code = connection.expectInt();
    if (code!=BOTTLE_TAG_DOUBLE) {
        return false;
    }
    timeStamp = connection.expectDouble();
    if (connection.isError()) {
        sequenceNumber = -1;
        timeStamp = 0;
        return false;
    }
    return true;
}

bool yarp::os::Stamp::write(ConnectionWriter& connection) {
    connection.appendInt(BOTTLE_TAG_LIST); // nested structure
    connection.appendInt(2);               // with two elements
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt(sequenceNumber);
    connection.appendInt(BOTTLE_TAG_DOUBLE);
    connection.appendDouble(timeStamp);
    connection.convertTextMode();
    return !connection.isError();
}

int yarp::os::Stamp::getMaxCount() {
    // a very conservative maximum
    return 32767;
}

void yarp::os::Stamp::update() {
    double now = Time::now();

    sequenceNumber++;
    if (sequenceNumber>getMaxCount()||sequenceNumber<0) {
        sequenceNumber = 0;
    }
    timeStamp = now;
}

void yarp::os::Stamp::update(double time) {
    sequenceNumber++;
    if (sequenceNumber>getMaxCount()||sequenceNumber<0) {
        sequenceNumber = 0;
    }
    timeStamp = time;
}


yarp::os::Stamped::~Stamped() {
}

