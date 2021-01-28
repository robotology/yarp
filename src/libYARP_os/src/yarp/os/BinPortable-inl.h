/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_BINPORTABLE_INL_H
#define YARP_OS_BINPORTABLE_INL_H

#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>

template <class T>
T& yarp::os::BinPortable<T>::content()
{
    return t;
}

template <class T>

bool yarp::os::BinPortable<T>::read(ConnectionReader& connection)
{
    // An exception will get thrown upon error.
    // Pending: translate this in expectBlock to a return value.
    connection.convertTextMode(); // if connection is text-mode, convert!
    connection.expectBlock((char*)(&t), sizeof(T));
    return true;
}

template <class T>
bool yarp::os::BinPortable<T>::write(ConnectionWriter& connection) const
{
    connection.appendBlock((char*)(&t), sizeof(T));
    connection.convertTextMode(); // if connection is text-mode, convert!
    return true;
}

#endif // YARP_OS_BINPORTABLE_INL_H
