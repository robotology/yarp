/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_LASERMEASURMENTDATA_H
#define YARP_SIG_LASERMEASURMENTDATA_H

#include <yarp/sig/api.h>

/*!
 * \file LaserMeasurementData.h
 */

namespace yarp::sig {
class LaserMeasurementData;
}

/*!
 * \LaserMeasurementData
 */
class YARP_sig_API yarp::sig::LaserMeasurementData
{
    double stored_x;
    double stored_y;
    double stored_angle;
    double stored_distance;
public:
    LaserMeasurementData();
    void set_cartesian(const double x, const double y);
    void set_polar(const double rho, const double theta);
    void get_cartesian(double& x, double& y);
    void get_polar(double& rho, double& theta);
};

#endif // YARP_SIG_LASERMEASURMENTDATA_H
