/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/NullConnectionReader.h>


bool yarp::os::NullConnectionReader::expectBlock(char* data, size_t len)
{
    YARP_UNUSED(data);
    YARP_UNUSED(len);
    return false;
}

std::string yarp::os::NullConnectionReader::expectText(const char terminatingChar)
{
    YARP_UNUSED(terminatingChar);
    return {};
}

std::int8_t yarp::os::NullConnectionReader::expectInt8()
{
    return 0;
}

std::int16_t yarp::os::NullConnectionReader::expectInt16()
{
    return 0;
}

std::int32_t yarp::os::NullConnectionReader::expectInt32()
{
    return 0;
}

std::int64_t yarp::os::NullConnectionReader::expectInt64()
{
    return 0;
}

yarp::conf::float32_t yarp::os::NullConnectionReader::expectFloat32()
{
    return 0.0f;
}

yarp::conf::float64_t yarp::os::NullConnectionReader::expectFloat64()
{
    return 0.0;
}

bool yarp::os::NullConnectionReader::pushInt(int x)
{
    YARP_UNUSED(x);
    return false;
}
bool yarp::os::NullConnectionReader::isTextMode() const
{
    return false;
}

bool yarp::os::NullConnectionReader::convertTextMode()
{
    return false;
}

size_t yarp::os::NullConnectionReader::getSize() const
{
    return 0;
}

yarp::os::ConnectionWriter* yarp::os::NullConnectionReader::getWriter()
{
    return nullptr;
}

yarp::os::Bytes yarp::os::NullConnectionReader::readEnvelope()
{
    return {nullptr, 0};
}

yarp::os::Portable* yarp::os::NullConnectionReader::getReference() const
{
    return nullptr;
}

yarp::os::Contact yarp::os::NullConnectionReader::getRemoteContact() const
{
    return Contact();
}

yarp::os::Contact yarp::os::NullConnectionReader::getLocalContact() const
{
    return Contact();
}

bool yarp::os::NullConnectionReader::isValid() const
{
    return false;
}

bool yarp::os::NullConnectionReader::isActive() const
{
    return false;
}

bool yarp::os::NullConnectionReader::isError() const
{
    return true;
}

void yarp::os::NullConnectionReader::requestDrop()
{
}

const yarp::os::Searchable& yarp::os::NullConnectionReader::getConnectionModifiers() const
{
    return blank;
}
