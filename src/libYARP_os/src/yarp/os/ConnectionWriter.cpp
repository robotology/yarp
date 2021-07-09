/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/OutputStream.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>

using namespace yarp::os;
using namespace yarp::os::impl;

ConnectionWriter::~ConnectionWriter() = default;

bool ConnectionWriter::isNull() const
{
    return false;
}


ConnectionWriter* ConnectionWriter::createBufferedConnectionWriter()
{
    return new BufferedConnectionWriter;
}

bool ConnectionWriter::writeToStream(PortWriter& portable, OutputStream& os)
{
    BufferedConnectionWriter writer;
    if (!portable.write(writer)) {
        return false;
    }
    writer.write(os);
    return os.isOk();
}
