/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
