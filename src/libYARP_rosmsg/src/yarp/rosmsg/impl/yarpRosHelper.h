/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_ROSMSG_IMPL_HELPER_H
#define YARP_ROSMSG_IMPL_HELPER_H


#include <iostream>
#include <climits>
#include <cmath>
#include <yarp/os/Log.h>

typedef enum
{
    ROS_config_error,
    ROS_disabled,
    ROS_enabled,
    ROS_only
}
ROSTopicUsageType;

constexpr double PI = 3.1415926535897932384626433;

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
        yError("convertEulerAngleYXZrads_to_quaternion: Error on quaternion conversion.");
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

#endif  // YARP_ROSMSG_IMPL_HELPER_H
