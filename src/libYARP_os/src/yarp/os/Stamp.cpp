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

int yarp::os::Stamp::getMaxCount() const
{
    return std::numeric_limits<int>::max();
}

void yarp::os::Stamp::update()
{
    double now = Time::now();

    if (sequenceNumber >= getMaxCount() || sequenceNumber < 0) {
        sequenceNumber = 0;
    } else {
        sequenceNumber++;
    }
    timeStamp = now;
}

void yarp::os::Stamp::update(double time)
{
    if (sequenceNumber >= getMaxCount() || sequenceNumber < 0) {
        sequenceNumber = 0;
    } else {
        sequenceNumber++;
    }
    timeStamp = time;
}
