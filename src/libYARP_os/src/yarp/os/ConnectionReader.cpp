/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/ConnectionReader.h>
#include <yarp/os/impl/StreamConnectionReader.h>

using namespace yarp::os;
using namespace yarp::os::impl;

ConnectionReader::~ConnectionReader() = default;

Bytes ConnectionReader::readEnvelope()
{
    return {nullptr, 0};
}

void ConnectionReader::setParentConnectionReader(ConnectionReader* parentConnectionReader)
{
    YARP_UNUSED(parentConnectionReader);
}

ConnectionReader* ConnectionReader::createConnectionReader(InputStream& is)
{
    auto* reader = new StreamConnectionReader();
    Route r;
    reader->reset(is, nullptr, r, 0, false);
    return reader;
}

bool ConnectionReader::readFromStream(PortReader& portable, InputStream& is)
{
    StreamConnectionReader reader;
    Route r;
    reader.reset(is, nullptr, r, 0, false);
    return portable.read(reader);
}
