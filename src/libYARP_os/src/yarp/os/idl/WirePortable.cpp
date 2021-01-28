/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/idl/WirePortable.h>

#include <yarp/os/idl/WireReader.h>
#include <yarp/os/idl/WireWriter.h>

bool yarp::os::idl::WirePortable::read(yarp::os::idl::WireReader& reader)
{
    YARP_UNUSED(reader);
    return false;
}

bool yarp::os::idl::WirePortable::write(const yarp::os::idl::WireWriter& writer) const
{
    YARP_UNUSED(writer);
    return false;
}
