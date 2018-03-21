/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/IConfig.h>


yarp::os::IConfig::~IConfig()
{
}

bool yarp::os::IConfig::open(yarp::os::Searchable& config)
{
    YARP_UNUSED(config);
    return true;
}

bool yarp::os::IConfig::close()
{
    return true;
}

bool yarp::os::IConfig::configure(Searchable& config)
{
    YARP_UNUSED(config);
    return false;
}
