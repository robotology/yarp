/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/SizedWriter.h>

#include <yarp/os/Bytes.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/OutputStream.h>


yarp::os::SizedWriter::~SizedWriter() = default;

void yarp::os::SizedWriter::write(OutputStream& os)
{
    for (size_t i = 0; i < length(); i++) {
        Bytes b((char*)data(i), length(i));
        os.write(b);
    }
}

bool yarp::os::SizedWriter::write(ConnectionWriter& connection) const
{
    for (size_t i = 0; i < length(); i++) {
        connection.appendBlock((char*)data(i), length(i));
    }
    return true;
}

void yarp::os::SizedWriter::clear()
{
}
