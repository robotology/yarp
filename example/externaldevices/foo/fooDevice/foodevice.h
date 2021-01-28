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
