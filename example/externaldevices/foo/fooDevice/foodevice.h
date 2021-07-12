/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

// This is just an example for an empty device.


#include <yarp/dev/DeviceDriver.h>

class foodevice : public yarp::dev::DeviceDriver
{
public:
    foodevice() = default;
    foodevice(const foodevice&) = delete;
    foodevice(foodevice&&) = delete;
    foodevice& operator=(const foodevice&) = delete;
    foodevice& operator=(foodevice&&) = delete;
    ~foodevice() override = default;

    virtual bool open(yarp::os::Searchable& config);
};
