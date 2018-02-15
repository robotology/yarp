/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/NullConnectionReader.h>


bool yarp::os::NullConnectionReader::expectBlock(const char *data, size_t len)
{
    YARP_UNUSED(data);
    YARP_UNUSED(len);
    return false;
}

yarp::os::ConstString yarp::os::NullConnectionReader::expectText(int terminatingChar)
{
    YARP_UNUSED(terminatingChar);
    return "";
}

int yarp::os::NullConnectionReader::expectInt()
{
    return 0;
}

bool yarp::os::NullConnectionReader::pushInt(int x)
{
    YARP_UNUSED(x);
    return false;
}

double yarp::os::NullConnectionReader::expectDouble()
{
    return 0.0;
}

bool yarp::os::NullConnectionReader::isTextMode()
{
    return false;
}

bool yarp::os::NullConnectionReader::convertTextMode()
{
    return false;
}

size_t yarp::os::NullConnectionReader::getSize()
{
    return 0;
}

yarp::os::ConnectionWriter *yarp::os::NullConnectionReader::getWriter()
{
    return nullptr;
}

yarp::os::Bytes yarp::os::NullConnectionReader::readEnvelope()
{
    return Bytes(nullptr, 0);
}

yarp::os::Portable *yarp::os::NullConnectionReader::getReference()
{
    return nullptr;
}

yarp::os::Contact yarp::os::NullConnectionReader::getRemoteContact()
{
    return Contact();
}

yarp::os::Contact yarp::os::NullConnectionReader::getLocalContact()
{
    return Contact();
}

bool yarp::os::NullConnectionReader::isValid()
{
    return false;
}

bool yarp::os::NullConnectionReader::isActive()
{
    return false;
}

bool yarp::os::NullConnectionReader::isError()
{
    return true;
}

void yarp::os::NullConnectionReader::requestDrop()
{
}

yarp::os::Searchable& yarp::os::NullConnectionReader::getConnectionModifiers()
{
    return blank;
}
