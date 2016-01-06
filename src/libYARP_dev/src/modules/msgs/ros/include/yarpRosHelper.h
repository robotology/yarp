// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef YARP__ROS__HELPER
#define YARP__ROS__HELPER

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

/* return false if errors occourr, like norm of the resulting vector is not 1*/
inline bool convertEulerAngleYXZrads_to_quaternion(double *eulerXYZ, double *quaternion)
{
    bool ret = true;

    quaternion[0] = -sin(eulerXYZ[0]/2) * sin(eulerXYZ[1]/2) * sin(eulerXYZ[2]/2) + cos(eulerXYZ[0]/2) * cos(eulerXYZ[1]/2) * cos(eulerXYZ[2]/2);
    quaternion[1] =  sin(eulerXYZ[0]/2) * cos(eulerXYZ[1]/2) * cos(eulerXYZ[2]/2) + cos(eulerXYZ[0]/2) * sin(eulerXYZ[1]/2) * sin(eulerXYZ[2]/2);
    quaternion[2] = -sin(eulerXYZ[0]/2) * cos(eulerXYZ[1]/2) * sin(eulerXYZ[2]/2) + cos(eulerXYZ[0]/2) * sin(eulerXYZ[1]/2) * cos(eulerXYZ[2]/2);
    quaternion[3] =  sin(eulerXYZ[0]/2) * sin(eulerXYZ[1]/2) * cos(eulerXYZ[2]/2) + cos(eulerXYZ[0]/2) * cos(eulerXYZ[1]/2) * sin(eulerXYZ[2]/2);

    // verifica norma vettore

    double norma = 0;
    for(int i=0; i<4; i++)
    {
        norma += quaternion[i] * quaternion[i];
    }
    norma = sqrt(norma);

    if((norma -1)  >= 0.05)
    {
        yError() << "Error on quaternion conversion!!!!!";
        ret = false;
    }

    return ret;
}

inline bool convertEulerAngleYXZdegrees_to_quaternion(double *eulerXYZ, double *quaternion)
{
    eulerXYZ[0] = convertDegreesToRadians(eulerXYZ[0]);
    eulerXYZ[1] = convertDegreesToRadians(eulerXYZ[1]);
    eulerXYZ[2] = convertDegreesToRadians(eulerXYZ[2]);
    return convertEulerAngleYXZrads_to_quaternion(eulerXYZ, quaternion);
}

#endif  // __YARP__ROS__HELPER__
