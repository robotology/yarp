/*
 * Copyright (C) 2012 Istituto Italiano di Tecnologia (IIT)
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/NullConnectionWriter.h>
#include <yarp/os/PortReader.h>

void yarp::os::NullConnectionWriter::appendBlock(const char *data, size_t len)
{
    YARP_UNUSED(data);
    YARP_UNUSED(len);
}

void yarp::os::NullConnectionWriter::appendInt(int data)
{
    YARP_UNUSED(data);
}

void yarp::os::NullConnectionWriter::appendInt64(const YARP_INT64& data)
{
    YARP_UNUSED(data);
}

void yarp::os::NullConnectionWriter::appendDouble(double data)
{
    YARP_UNUSED(data);
}

void yarp::os::NullConnectionWriter::appendString(const char *str, int terminate)
{
    YARP_UNUSED(str);
    YARP_UNUSED(terminate);
}

void yarp::os::NullConnectionWriter::appendExternalBlock(const char *data, size_t len)
{
    YARP_UNUSED(data);
    YARP_UNUSED(len);
}

bool yarp::os::NullConnectionWriter::isTextMode()
{
    return false;
}

bool yarp::os::NullConnectionWriter::isBareMode()
{
    return false;
}

void yarp::os::NullConnectionWriter::declareSizes(int argc, int *argv)
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);
}

void yarp::os::NullConnectionWriter::setReplyHandler(PortReader& reader)
{
    YARP_UNUSED(reader);
}

void yarp::os::NullConnectionWriter::setReference(Portable *obj)
{
    YARP_UNUSED(obj);
}

bool yarp::os::NullConnectionWriter::convertTextMode()
{
    return false;
}

bool yarp::os::NullConnectionWriter::isValid()
{
    return false;
}

bool yarp::os::NullConnectionWriter::isActive()
{
    return true;
}

bool yarp::os::NullConnectionWriter::isError()
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

yarp::os::SizedWriter *yarp::os::NullConnectionWriter::getBuffer()
{
    return nullptr;
}
