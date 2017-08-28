/*
 * Copyright (C) 2007 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
