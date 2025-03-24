/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.dev


struct BatteryData
{
    /** Battery charge percentage [%] */
    1: double       charge;

    /** Battery Voltage [V] */
    2: double       voltage;

    /** Battery Current [A] */
    3: double       current;

    /** Battery Temperature [degC] */
    4: double       temperature;

    /** the status of the device. See yarp::dev::IRangefinder2D::Device_status. The default value is DEVICE_TIMEOUT. */
    5: i32          status=3;
}
(
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)
