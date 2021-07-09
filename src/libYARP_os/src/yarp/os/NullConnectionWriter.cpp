/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/NullConnectionWriter.h>
#include <yarp/os/PortReader.h>

void yarp::os::NullConnectionWriter::appendBlock(const char* data, size_t len)
{
    YARP_UNUSED(data);
    YARP_UNUSED(len);
}

void yarp::os::NullConnectionWriter::appendInt8(std::int8_t data)
{
    YARP_UNUSED(data);
}

void yarp::os::NullConnectionWriter::appendInt16(std::int16_t data)
{
    YARP_UNUSED(data);
}

void yarp::os::NullConnectionWriter::appendInt32(std::int32_t data)
{
    YARP_UNUSED(data);
}

void yarp::os::NullConnectionWriter::appendInt64(std::int64_t data)
{
    YARP_UNUSED(data);
}

void yarp::os::NullConnectionWriter::appendFloat32(yarp::conf::float32_t data)
{
    YARP_UNUSED(data);
}

void yarp::os::NullConnectionWriter::appendFloat64(yarp::conf::float64_t data)
{
    YARP_UNUSED(data);
}

void yarp::os::NullConnectionWriter::appendText(const std::string& str, const char terminate)
{
    YARP_UNUSED(str);
    YARP_UNUSED(terminate);
}

void yarp::os::NullConnectionWriter::appendExternalBlock(const char* data, size_t len)
{
    YARP_UNUSED(data);
    YARP_UNUSED(len);
}

bool yarp::os::NullConnectionWriter::isTextMode() const
{
    return false;
}

bool yarp::os::NullConnectionWriter::isBareMode() const
{
    return false;
}

void yarp::os::NullConnectionWriter::declareSizes(int argc, int* argv)
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);
}

void yarp::os::NullConnectionWriter::setReplyHandler(PortReader& reader)
{
    YARP_UNUSED(reader);
}

void yarp::os::NullConnectionWriter::setReference(Portable* obj)
{
    YARP_UNUSED(obj);
}

bool yarp::os::NullConnectionWriter::convertTextMode()
{
    return false;
}

bool yarp::os::NullConnectionWriter::isValid() const
{
    return false;
}

bool yarp::os::NullConnectionWriter::isActive() const
{
    return true;
}

bool yarp::os::NullConnectionWriter::isError() const
{
    return true;
}

void yarp::os::NullConnectionWriter::requestDrop()
{
}

bool yarp::os::NullConnectionWriter::isNull() const
{
    return true;
}

yarp::os::SizedWriter* yarp::os::NullConnectionWriter::getBuffer() const
{
    return nullptr;
}
