/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
