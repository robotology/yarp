/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Autogenerated by Thrift Compiler (0.14.1-yarped)
//
// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <return_get_estimated_odometry.h>

// Constructor with field values
return_get_estimated_odometry::return_get_estimated_odometry(const yarp::dev::ReturnValue& ret,
                                                             const yarp::dev::OdometryData& odom) :
        WirePortable(),
        ret(ret),
        odom(odom)
{
}

// Read structure on a Wire
bool return_get_estimated_odometry::read(yarp::os::idl::WireReader& reader)
{
    if (!nested_read_ret(reader)) {
        return false;
    }
    if (!nested_read_odom(reader)) {
        return false;
    }
    if (reader.isError()) {
        return false;
    }
    return true;
}

// Read structure on a Connection
bool return_get_estimated_odometry::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(2)) {
        return false;
    }
    if (!read(reader)) {
        return false;
    }
    return true;
}

// Write structure on a Wire
bool return_get_estimated_odometry::write(const yarp::os::idl::WireWriter& writer) const
{
    if (!nested_write_ret(writer)) {
        return false;
    }
    if (!nested_write_odom(writer)) {
        return false;
    }
    if (writer.isError()) {
        return false;
    }
    return true;
}

// Write structure on a Connection
bool return_get_estimated_odometry::write(yarp::os::ConnectionWriter& connection) const
{
    yarp::os::idl::WireWriter writer(connection);
    if (!writer.writeListHeader(2)) {
        return false;
    }
    if (!write(writer)) {
        return false;
    }
    return true;
}

// Convert to a printable string
std::string return_get_estimated_odometry::toString() const
{
    yarp::os::Bottle b;
    if (!yarp::os::Portable::copyPortable(*this, b)) {
        return {};
    }
    return b.toString();
}

// read ret field
bool return_get_estimated_odometry::read_ret(yarp::os::idl::WireReader& reader)
{
    if (reader.noMore()) {
        reader.fail();
        return false;
    }
    if (!reader.read(ret)) {
        reader.fail();
        return false;
    }
    return true;
}

// write ret field
bool return_get_estimated_odometry::write_ret(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.write(ret)) {
        return false;
    }
    return true;
}

// read (nested) ret field
bool return_get_estimated_odometry::nested_read_ret(yarp::os::idl::WireReader& reader)
{
    if (reader.noMore()) {
        reader.fail();
        return false;
    }
    if (!reader.readNested(ret)) {
        reader.fail();
        return false;
    }
    return true;
}

// write (nested) ret field
bool return_get_estimated_odometry::nested_write_ret(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeNested(ret)) {
        return false;
    }
    return true;
}

// read odom field
bool return_get_estimated_odometry::read_odom(yarp::os::idl::WireReader& reader)
{
    if (reader.noMore()) {
        reader.fail();
        return false;
    }
    if (!reader.read(odom)) {
        reader.fail();
        return false;
    }
    return true;
}

// write odom field
bool return_get_estimated_odometry::write_odom(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.write(odom)) {
        return false;
    }
    return true;
}

// read (nested) odom field
bool return_get_estimated_odometry::nested_read_odom(yarp::os::idl::WireReader& reader)
{
    if (reader.noMore()) {
        reader.fail();
        return false;
    }
    if (!reader.readNested(odom)) {
        reader.fail();
        return false;
    }
    return true;
}

// write (nested) odom field
bool return_get_estimated_odometry::nested_write_odom(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeNested(odom)) {
        return false;
    }
    return true;
}
