/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is just an example for an empty device.

#include <yarp/dev/DeviceDriver.h>

class bazbot : public yarp::dev::DeviceDriver
{
public:
    bazbot() = default;
    bazbot(const bazbot&) = delete;
    bazbot(bazbot&&) = delete;
    bazbot& operator=(const bazbot&) = delete;
    bazbot& operator=(bazbot&&) = delete;
    ~bazbot() override = default;

    bool open(yarp::os::Searchable& config) override;
};
