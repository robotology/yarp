/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Stamp.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/Time.h>

#include <cfloat>
#include <limits>

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

int yarp::os::Stamp::getCount() const
{
    return sequenceNumber;
}

double yarp::os::Stamp::getTime() const
{
    return timeStamp;
}

bool yarp::os::Stamp::isValid() const
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
        std::int32_t header = connection.expectInt32();
        if (header != BOTTLE_TAG_LIST) {
            return false;
        }
        std::int32_t len = connection.expectInt32();
        // len should be 2 for Stamp, 3 for Header
        if (len != 2 && len != 3) {
            return false;
        }
        std::int32_t code = connection.expectInt32();
        if (code != BOTTLE_TAG_INT32) {
            return false;
        }
        sequenceNumber = connection.expectInt32();
        code = connection.expectInt32();
        if (code != BOTTLE_TAG_FLOAT64) {
            return false;
        }
        timeStamp = connection.expectFloat64();
        if (connection.isError()) {
            sequenceNumber = -1;
            timeStamp = 0;
            return false;
        }

        // Read frameId (unless receiving a Stamp), but just discard its value
        if (len == 3) {
            code = connection.expectInt32();
            if (code != BOTTLE_TAG_STRING) {
                sequenceNumber = -1;
                timeStamp = 0;
                return false;
            }
            std::string unused = connection.expectString();
            YARP_UNUSED(unused);
        }

    }
    return !connection.isError();
}

bool yarp::os::Stamp::write(ConnectionWriter& connection) const
{
    if (connection.isTextMode()) {
        char buf[512];
        std::snprintf(buf, 512, "%d %.*g", sequenceNumber, DBL_DIG, timeStamp);
        connection.appendText(buf);
    } else {
        connection.appendInt32(BOTTLE_TAG_LIST); // nested structure
        connection.appendInt32(2);               // with two elements
        connection.appendInt32(BOTTLE_TAG_INT32);
        connection.appendInt32(sequenceNumber);
        connection.appendInt32(BOTTLE_TAG_FLOAT64);
        connection.appendFloat64(timeStamp);
        connection.convertTextMode();
    }
    return !connection.isError();
}

int yarp::os::Stamp::getMaxCount() const
{
    return std::numeric_limits<int>::max();
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

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
yarp::os::Stamped::~Stamped() = default;
YARP_WARNING_POP
#endif // YARP_NO_DEPRECATED
