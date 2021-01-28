/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Bytes.h>


yarp::os::Bytes::Bytes() :
        data(nullptr),
        len(0)
{
}

yarp::os::Bytes::Bytes(char* data, size_t len) :
        data(data),
        len(len)
{
}

size_t yarp::os::Bytes::length() const
{
    return len;
}

const char* yarp::os::Bytes::get() const
{
    return data;
}

char* yarp::os::Bytes::get()
{
    return data;
}
