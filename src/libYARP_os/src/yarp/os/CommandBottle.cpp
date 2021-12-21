/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/CommandBottle.h>

using yarp::os::CommandBottle;

CommandBottle::CommandBottle(const std::string& text) :
        cmd(text)
{
}

CommandBottle::CommandBottle(std::initializer_list<yarp::os::Value> values) :
        cmd(values)
{
}

bool CommandBottle::write(yarp::os::ConnectionWriter& connection) const
{
    return cmd.write(connection);
}

bool CommandBottle::read(yarp::os::ConnectionReader& connection)
{
    return reply.read(connection);
}
