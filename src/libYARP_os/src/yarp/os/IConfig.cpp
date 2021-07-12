/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/IConfig.h>


yarp::os::IConfig::~IConfig() = default;

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
