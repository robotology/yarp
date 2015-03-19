// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef __YARP__ROS__HELPER__
#define __YARP__ROS__HELPER__

/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Alberto Cardellino
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

// This file contains helper functions for the ControlBoardWrapper


#include <iostream>
#include <limits.h>
#include <cmath>
#include <yarp/os/LogStream.h>

#include "sensor_msgs_JointState.h"


typedef enum
{
    ROS_config_error,
    ROS_disabled,
    ROS_enabled,
    ROS_only
}
ROSTopicUsageType;

double const PI = 3.1415926535897932384626433;

/**
 * This function has been took from ROS source file
 * http://docs.ros.org/diamondback/api/rostime/html/time_8h_source.html#l00095
 * and modified a bit to cope with yarp time handling in double
 * yarpTimeStamp: input yarp time in double
 * sec: sec part
 * nsec: nano second part
 */
inline TickTime normalizeSecNSec(double yarpTimeStamp)
{
    uint64_t time = (uint64_t) (yarpTimeStamp * 1000000000UL);
    uint64_t nsec_part = (time % 1000000000UL);
    uint64_t sec_part = (time / 1000000000UL);
    TickTime ret;

    if (sec_part > UINT_MAX)
    {
        yWarning() << "Timestamp exceeded the 64 bit representation, resetting it to 0";
        sec_part = 0;
    }

    ret.sec  = sec_part;
    ret.nsec = nsec_part;
    return ret;
}

/** convert degrees to radiants for ROS messages */
inline double convertDegreesToRadians(double degrees)
{
    return degrees / 180.0 * PI;
}

inline void convertDegreesToRadians(std::vector<yarp::os::NetFloat64>  &degrees)
{
    for(size_t i=0; i<degrees.size(); i++)
        degrees[i] = convertDegreesToRadians(degrees[i]);
}

#endif  // __YARP__ROS__HELPER__
