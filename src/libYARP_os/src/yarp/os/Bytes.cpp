/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
