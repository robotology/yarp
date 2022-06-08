/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "JoypadControlNetUtils.h"

#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>

bool JoyData::read(yarp::os::ConnectionReader& connection)
{
    Buttons.resize(connection.expectInt32());
    Sticks.resize(connection.expectInt32());
    Axes.resize(connection.expectInt32());
    Balls.resize(connection.expectInt32());
    Touch.resize(connection.expectInt32());
    Hats.resize(connection.expectInt32());
    connection.expectBlock((char*)Buttons.data(), Buttons.length() * sizeof(double));
    connection.expectBlock((char*)Sticks.data(),  Sticks.length()  * sizeof(double));
    connection.expectBlock((char*)Axes.data(),    Axes.length()    * sizeof(double));
    connection.expectBlock((char*)Balls.data(),   Balls.length()   * sizeof(double));
    connection.expectBlock((char*)Touch.data(),   Touch.length()   * sizeof(double));
    connection.expectBlock((char*)&Hats[0],       Hats.size()      * sizeof(char));
    return !connection.isError();
}

bool JoyData::write(yarp::os::ConnectionWriter& connection) const
{
    connection.appendInt32(Buttons.length());
    connection.appendInt32(Sticks.length());
    connection.appendInt32(Axes.length()  );
    connection.appendInt32(Balls.length() );
    connection.appendInt32(Touch.length() );
    connection.appendInt32(Hats.size()  );
    connection.appendBlock((char*)Buttons.data(), Buttons.length() * sizeof(double));
    connection.appendBlock((char*)Sticks.data(),  Sticks.length()  * sizeof(double));
    connection.appendBlock((char*)Axes.data(),    Axes.length()    * sizeof(double));
    connection.appendBlock((char*)Balls.data(),   Balls.length()   * sizeof(double));
    connection.appendBlock((char*)Touch.data(),   Touch.length()   * sizeof(double));
    connection.appendBlock((char*)&Hats[0],       Hats.size()      * sizeof(char));
    connection.convertTextMode();
    return !connection.isError();
}
