/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
