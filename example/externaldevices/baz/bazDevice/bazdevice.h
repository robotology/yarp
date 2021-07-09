/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is just an example for an empty device.

#include <yarp/dev/DeviceDriver.h>

class bazdevice : public yarp::dev::DeviceDriver
{
public:
    bazdevice() = default;
    bazdevice(const bazdevice&) = delete;
    bazdevice(bazdevice&&) = delete;
    bazdevice& operator=(const bazdevice&) = delete;
    bazdevice& operator=(bazdevice&&) = delete;
    ~bazdevice() override = default;

    bool open(yarp::os::Searchable& config) override;
};
