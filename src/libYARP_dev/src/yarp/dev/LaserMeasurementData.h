/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_LASERMEASURMENTDATA_H
#define YARP_DEV_LASERMEASURMENTDATA_H

#include <yarp/dev/api.h>

/*!
 * \file LaserMeasurementData.h
 */

namespace yarp{
    namespace dev{
        class LaserMeasurementData;
    }
}

/*!
 * \LaserMeasurementData
 */
class YARP_dev_API yarp::dev::LaserMeasurementData
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

#endif // YARP_DEV_LASERMEASURMENTDATA_H
