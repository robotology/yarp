/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>

using namespace yarp::os;
using namespace yarp::os::impl;

ConnectionWriter::~ConnectionWriter() {
}

bool ConnectionWriter::isNull() const {
    return false;
}


ConnectionWriter *ConnectionWriter::createBufferedConnectionWriter() {
    return new BufferedConnectionWriter;
}

bool ConnectionWriter::writeToStream(PortWriter& portable, OutputStream& os) {
    BufferedConnectionWriter writer;
    if (!portable.write(writer)) return false;
    writer.write(os);
    return os.isOk();
}

