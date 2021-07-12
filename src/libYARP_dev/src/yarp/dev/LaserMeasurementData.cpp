/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/LaserMeasurementData.h>
#include <cmath>

using namespace yarp::dev;

LaserMeasurementData::LaserMeasurementData()
{
    stored_x = stored_y = stored_angle = stored_distance = 0;
}

void LaserMeasurementData::set_cartesian(const double x, const double y)
{
    stored_x = x; stored_y = y; stored_distance = sqrt(x*x + y*y); stored_angle = atan2(y, x);
}

void LaserMeasurementData::set_polar(const double rho, const double theta)
{
    stored_angle = theta; stored_distance = rho; stored_y = rho*sin(theta); stored_x = rho*cos(theta);
}

void LaserMeasurementData::get_cartesian(double& x, double& y)
{
    x = stored_x; y = stored_y;
}

void LaserMeasurementData::get_polar(double& rho, double& theta)
{
    rho = stored_distance; theta = stored_angle;
}
