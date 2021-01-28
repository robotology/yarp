/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
