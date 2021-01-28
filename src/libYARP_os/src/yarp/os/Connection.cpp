/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Connection.h>

using yarp::os::Connection;

Connection::~Connection() = default;

bool Connection::isValid() const
{
    return true;
}

bool Connection::isBareMode() const
{
    return false;
}
