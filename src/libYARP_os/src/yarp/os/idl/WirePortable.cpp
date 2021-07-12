/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
