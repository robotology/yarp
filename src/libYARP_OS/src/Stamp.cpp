/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Stamp.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>

#include <cfloat>

yarp::os::Stamp::Stamp(int count, double time)
{
    sequenceNumber = count;
    timeStamp = time;
}

yarp::os::Stamp::Stamp()
{
    sequenceNumber = -1;
    timeStamp = 0;
}

int yarp::os::Stamp::getCount()
{
    return sequenceNumber;
}

double yarp::os::Stamp::getTime()
{
    return timeStamp;
}

bool yarp::os::Stamp::isValid()
{
    return sequenceNumber >= 0;
}

bool yarp::os::Stamp::read(ConnectionReader& connection)
{
    if (connection.isTextMode()) {
        std::string stampStr = connection.expectText();
        int seqNum;
        double ts;
        int ret = std::sscanf(stampStr.c_str(), "%d %lg\n", &seqNum, &ts);
        if (ret != 2) {
            sequenceNumber = -1;
            timeStamp = 0;
            return false;
        }
        sequenceNumber = seqNum;
        timeStamp = ts;
    } else {
        connection.convertTextMode();
        int header = connection.expectInt();
        if (header != BOTTLE_TAG_LIST) {
            return false;
        }
        int len = connection.expectInt();
        if (len != 2) {
            return false;
        }
        int code;
        code = connection.expectInt();
        if (code != BOTTLE_TAG_INT) {
            return false;
        }
        sequenceNumber = connection.expectInt();
        code = connection.expectInt();
        if (code != BOTTLE_TAG_DOUBLE) {
            return false;
        }
        timeStamp = connection.expectDouble();
        if (connection.isError()) {
            sequenceNumber = -1;
            timeStamp = 0;
            return false;
        }
    }
    return !connection.isError();
}

bool yarp::os::Stamp::write(ConnectionWriter& connection)
{
    if (connection.isTextMode()) {
        char buf[512];
        std::snprintf(buf, 512, "%d %.*g", sequenceNumber, DBL_DIG, timeStamp);
        connection.appendString(buf);
    } else {
        connection.appendInt(BOTTLE_TAG_LIST); // nested structure
        connection.appendInt(2);               // with two elements
        connection.appendInt(BOTTLE_TAG_INT);
        connection.appendInt(sequenceNumber);
        connection.appendInt(BOTTLE_TAG_DOUBLE);
        connection.appendDouble(timeStamp);
        connection.convertTextMode();
    }
    return !connection.isError();
}

int yarp::os::Stamp::getMaxCount()
{
    // a very conservative maximum
    return 32767;
}

void yarp::os::Stamp::update()
{
    double now = Time::now();

    sequenceNumber++;
    if (sequenceNumber > getMaxCount() || sequenceNumber < 0) {
        sequenceNumber = 0;
    }
    timeStamp = now;
}

void yarp::os::Stamp::update(double time)
{
    sequenceNumber++;
    if (sequenceNumber > getMaxCount() || sequenceNumber < 0) {
        sequenceNumber = 0;
    }
    timeStamp = time;
}

YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
yarp::os::Stamped::~Stamped() = default;
YARP_WARNING_POP
