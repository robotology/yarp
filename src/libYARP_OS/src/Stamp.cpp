// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/os/Stamp.h>

#include <yarp/IOException.h>

using namespace yarp::os;
using namespace yarp;

bool Stamp::read(ConnectionReader& connection) {
    try {
        connection.convertTextMode();
        sequenceNumber = connection.expectInt();
        timeStamp = connection.expectDouble();
        return true;
    } catch (IOException e) {
        sequenceNumber = -1;
        timeStamp = 0;
        return false;
    }
}

bool Stamp::write(ConnectionWriter& connection) {
    try {
        connection.appendInt(sequenceNumber);
        connection.appendDouble(timeStamp);
        connection.convertTextMode();
        return true;
    } catch (IOException e) {
        return false;
    }
}

