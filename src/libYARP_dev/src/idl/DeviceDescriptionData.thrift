/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.dev

struct DeviceDescriptionData
{
    /** name of the device */
    1: string device_name;
    /** type of the device */
    2: string device_type;
    /** configuration parameters of the device */
    3: string device_configuration;
    /** user defined extra information */
    4: string device_extra_info;
}
(
    yarp.api.include = "yarp/dev/api.h"
    yarp.api.keyword = "YARP_dev_API"
)
