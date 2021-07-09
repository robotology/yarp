/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/PortWriter.h>
#include <yarp/os/Type.h>


yarp::os::PortWriter::~PortWriter() = default;

void yarp::os::PortWriter::onCompletion() const
{
}

void yarp::os::PortWriter::onCommencement() const
{
}

yarp::os::Type yarp::os::PortWriter::getWriteType() const
{
    return yarp::os::Type::anon();
}
