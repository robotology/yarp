/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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

