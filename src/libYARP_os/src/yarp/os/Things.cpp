/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Things.h>

#include <cstdio>

using namespace yarp::os;

Things::Things() :
        beenRead(false),
        conReader(nullptr),
        writer(nullptr),
        reader(nullptr),
        portable(nullptr)
{
}

Things::~Things()
{
    delete portable;
}

void Things::setPortWriter(yarp::os::PortWriter* writer)
{
    yarp::os::Things::writer = writer;
}

yarp::os::PortWriter* Things::getPortWriter()
{
    return writer;
}

void Things::setPortReader(yarp::os::PortReader* reader)
{
    yarp::os::Things::reader = reader;
}

yarp::os::PortReader* Things::getPortReader()
{
    return reader;
}

bool Things::setConnectionReader(yarp::os::ConnectionReader& reader)
{
    conReader = &reader;
    delete portable;
    portable = nullptr;
    return true;
}

bool Things::write(yarp::os::ConnectionWriter& connection)
{
    if (writer != nullptr) {
        return writer->write(connection);
    }
    if (portable != nullptr) {
        return portable->write(connection);
    }
    return false;
}

void Things::reset()
{
    delete portable;
    conReader = nullptr;
    writer = nullptr;
    reader = nullptr;
    portable = nullptr;
    beenRead = false;
}

bool Things::hasBeenRead()
{
    return beenRead;
}
