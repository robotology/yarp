/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_BINPORTABLE_INL_H
#define YARP_OS_BINPORTABLE_INL_H

#include <yarp/os/ConnectionReader.h>

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
bool yarp::os::BinPortable<T>::write(ConnectionWriter& connection)
{
    connection.appendBlock((char*)(&t), sizeof(T));
    connection.convertTextMode(); // if connection is text-mode, convert!
    return true;
}

#endif // YARP_OS_BINPORTABLE_INL_H
