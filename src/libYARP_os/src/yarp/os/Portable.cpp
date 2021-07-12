/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Portable.h>

#include <yarp/os/DummyConnector.h>
#include <yarp/os/Type.h>

yarp::os::Type yarp::os::Portable::getType() const
{
    return getReadType();
}

bool yarp::os::Portable::copyPortable(const yarp::os::PortWriter& writer, yarp::os::PortReader& reader)
{
    yarp::os::DummyConnector con;
    if (!writer.write(con.getWriter())) {
        return false;
    }
    return reader.read(con.getReader());
}
