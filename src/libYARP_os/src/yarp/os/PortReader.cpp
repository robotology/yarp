/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/PortReader.h>
#include <yarp/os/Type.h>

yarp::os::PortReader::~PortReader() = default;

yarp::os::Type yarp::os::PortReader::getReadType() const
{
    return Type::anon();
}
