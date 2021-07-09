/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/TwoWayStream.h>

using namespace yarp::os;

TwoWayStream::~TwoWayStream() = default;

NullStream::~NullStream() = default;

InputStream& NullStream::getInputStream()
{
    return *this;
}

OutputStream& NullStream::getOutputStream()
{
    return *this;
}

const Contact& NullStream::getLocalAddress() const
{
    return address;
}

const Contact& NullStream::getRemoteAddress() const
{
    return address;
}

bool NullStream::isOk() const
{
    return false;
}

void NullStream::reset()
{
}

void NullStream::close()
{
}

void NullStream::beginPacket()
{
}

void NullStream::endPacket()
{
}

yarp::conf::ssize_t NullStream::read(Bytes& b)
{
    YARP_UNUSED(b);
    return -1;
}

void NullStream::write(const Bytes& b)
{
    YARP_UNUSED(b);
}
