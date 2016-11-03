/*
* Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
* Authors: Marco Randazzo <marco.randazzo@iit.it>
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#include <yarp/dev/api.h>

#ifndef YARPLASERMEASURMENTDATA
#define YARPLASERMEASURMENTDATA

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

#endif
