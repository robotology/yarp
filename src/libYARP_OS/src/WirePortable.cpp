/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/idl/WirePortable.h>
#include <yarp/os/idl/WireReader.h>
#include <yarp/os/idl/WireWriter.h>

bool yarp::os::idl::WirePortable::read(yarp::os::idl::WireReader& reader) {
    YARP_UNUSED(reader);
    return false;
}

bool yarp::os::idl::WirePortable::write(yarp::os::idl::WireWriter& writer) {
    YARP_UNUSED(writer);
    return false;
}
