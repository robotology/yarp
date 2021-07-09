/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/OutputStream.h>

#include <yarp/os/Bytes.h>


yarp::os::OutputStream::~OutputStream() = default;

void yarp::os::OutputStream::write(char ch)
{
    write(yarp::os::Bytes(&ch, 1));
}

void yarp::os::OutputStream::write(const Bytes& b, int offset, int len)
{
    const yarp::os::Bytes bytes(const_cast<char*>(b.get()) + offset, len);
    write(bytes);
}

void yarp::os::OutputStream::flush()
{
}

void yarp::os::OutputStream::writeLine(const char* data, int len)
{
    yarp::os::Bytes b((char*)data, len);
    write(b);
    write('\n');
}

bool yarp::os::OutputStream::setWriteTimeout(double timeout)
{
    YARP_UNUSED(timeout);
    return false;
}

bool yarp::os::OutputStream::setTypeOfService(int tos)
{
    YARP_UNUSED(tos);
    return false;
}

int yarp::os::OutputStream::getTypeOfService()
{
    return -1;
}
