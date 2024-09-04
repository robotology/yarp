/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.sig

struct YarpVector {
  1: list<double> content;
} (
  yarp.name = "yarp::sig::Vector"
  yarp.includefile="yarp/sig/Vector.h"
)

struct LaserScan2D
{
    /** first angle of the scan [deg] */
    1: double       angle_min;

    /** last angle of the scan [deg] */
    2: double       angle_max;

    /** the minimum distance of the scan [m] */
    3: double       range_min;

    /** the maximum distance of the scan [m] */
    4: double       range_max;

    /** the scan data, measured in [m]. The angular increment of each ray is obtained by (angle_max-angle_min)/num_of_elements. Invalid data are represented as std::inf.  */
    5: YarpVector   scans;

    /** the status of the device. See yarp::dev::IRangefinder2D::Device_status. The default value is DEVICE_TIMEOUT. */
    6: i32          status=3;
}
(
    yarp.api.include = "yarp/sig/api.h"
    yarp.api.keyword = "YARP_sig_API"
)
