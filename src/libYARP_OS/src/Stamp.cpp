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

using namespace yarp::os;
using namespace yarp::os::impl;

bool Stamp::read(ConnectionReader& connection) {
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

bool Stamp::write(ConnectionWriter& connection) {
    connection.appendInt(BOTTLE_TAG_LIST); // nested structure
    connection.appendInt(2);               // with two elements
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt(sequenceNumber);
    connection.appendInt(BOTTLE_TAG_DOUBLE);
    connection.appendDouble(timeStamp);
    connection.convertTextMode();
    return !connection.isError();
}



int Stamp::getMaxCount() {
    // a very conservative maximum
    return 32767;
}


void Stamp::update() {
    double now = Time::now();

    sequenceNumber++;
    if (sequenceNumber>getMaxCount()||sequenceNumber<0) {
        sequenceNumber = 0;
    }
    timeStamp = now;
}

void Stamp::update(double time) {

    sequenceNumber++;
    if (sequenceNumber>getMaxCount()||sequenceNumber<0) {
        sequenceNumber = 0;
    }
    timeStamp = time;
}

Stamp::Stamp(int count, double time)
{
    sequenceNumber = count;
    timeStamp = time;
}

Stamp::Stamp()
{
    sequenceNumber = -1;
    timeStamp = 0;
}

