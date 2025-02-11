/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Autogenerated by Thrift Compiler (0.14.1-yarped)
//
// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <return_get_rel_loc_of_curr_target.h>

// Constructor with field values
return_get_rel_loc_of_curr_target::return_get_rel_loc_of_curr_target(const yarp::dev::ReturnValue& ret,
                                                                     const double x,
                                                                     const double y,
                                                                     const double theta) :
        WirePortable(),
        ret(ret),
        x(x),
        y(y),
        theta(theta)
{
}

// Read structure on a Wire
bool return_get_rel_loc_of_curr_target::read(yarp::os::idl::WireReader& reader)
{
    if (!nested_read_ret(reader)) {
        return false;
    }
    if (!read_x(reader)) {
        return false;
    }
    if (!read_y(reader)) {
        return false;
    }
    if (!read_theta(reader)) {
        return false;
    }
    if (reader.isError()) {
        return false;
    }
    return true;
}

// Read structure on a Connection
bool return_get_rel_loc_of_curr_target::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(4)) {
        return false;
    }
    if (!read(reader)) {
        return false;
    }
    return true;
}

// Write structure on a Wire
bool return_get_rel_loc_of_curr_target::write(const yarp::os::idl::WireWriter& writer) const
{
    if (!nested_write_ret(writer)) {
        return false;
    }
    if (!write_x(writer)) {
        return false;
    }
    if (!write_y(writer)) {
        return false;
    }
    if (!write_theta(writer)) {
        return false;
    }
    if (writer.isError()) {
        return false;
    }
    return true;
}

// Write structure on a Connection
bool return_get_rel_loc_of_curr_target::write(yarp::os::ConnectionWriter& connection) const
{
    yarp::os::idl::WireWriter writer(connection);
    if (!writer.writeListHeader(4)) {
        return false;
    }
    if (!write(writer)) {
        return false;
    }
    return true;
}

// Convert to a printable string
std::string return_get_rel_loc_of_curr_target::toString() const
{
    yarp::os::Bottle b;
    if (!yarp::os::Portable::copyPortable(*this, b)) {
        return {};
    }
    return b.toString();
}

// read ret field
bool return_get_rel_loc_of_curr_target::read_ret(yarp::os::idl::WireReader& reader)
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
bool return_get_rel_loc_of_curr_target::write_ret(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.write(ret)) {
        return false;
    }
    return true;
}

// read (nested) ret field
bool return_get_rel_loc_of_curr_target::nested_read_ret(yarp::os::idl::WireReader& reader)
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
bool return_get_rel_loc_of_curr_target::nested_write_ret(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeNested(ret)) {
        return false;
    }
    return true;
}

// read x field
bool return_get_rel_loc_of_curr_target::read_x(yarp::os::idl::WireReader& reader)
{
    if (reader.noMore()) {
        reader.fail();
        return false;
    }
    if (!reader.readFloat64(x)) {
        reader.fail();
        return false;
    }
    return true;
}

// write x field
bool return_get_rel_loc_of_curr_target::write_x(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeFloat64(x)) {
        return false;
    }
    return true;
}

// read (nested) x field
bool return_get_rel_loc_of_curr_target::nested_read_x(yarp::os::idl::WireReader& reader)
{
    if (reader.noMore()) {
        reader.fail();
        return false;
    }
    if (!reader.readFloat64(x)) {
        reader.fail();
        return false;
    }
    return true;
}

// write (nested) x field
bool return_get_rel_loc_of_curr_target::nested_write_x(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeFloat64(x)) {
        return false;
    }
    return true;
}

// read y field
bool return_get_rel_loc_of_curr_target::read_y(yarp::os::idl::WireReader& reader)
{
    if (reader.noMore()) {
        reader.fail();
        return false;
    }
    if (!reader.readFloat64(y)) {
        reader.fail();
        return false;
    }
    return true;
}

// write y field
bool return_get_rel_loc_of_curr_target::write_y(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeFloat64(y)) {
        return false;
    }
    return true;
}

// read (nested) y field
bool return_get_rel_loc_of_curr_target::nested_read_y(yarp::os::idl::WireReader& reader)
{
    if (reader.noMore()) {
        reader.fail();
        return false;
    }
    if (!reader.readFloat64(y)) {
        reader.fail();
        return false;
    }
    return true;
}

// write (nested) y field
bool return_get_rel_loc_of_curr_target::nested_write_y(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeFloat64(y)) {
        return false;
    }
    return true;
}

// read theta field
bool return_get_rel_loc_of_curr_target::read_theta(yarp::os::idl::WireReader& reader)
{
    if (reader.noMore()) {
        reader.fail();
        return false;
    }
    if (!reader.readFloat64(theta)) {
        reader.fail();
        return false;
    }
    return true;
}

// write theta field
bool return_get_rel_loc_of_curr_target::write_theta(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeFloat64(theta)) {
        return false;
    }
    return true;
}

// read (nested) theta field
bool return_get_rel_loc_of_curr_target::nested_read_theta(yarp::os::idl::WireReader& reader)
{
    if (reader.noMore()) {
        reader.fail();
        return false;
    }
    if (!reader.readFloat64(theta)) {
        reader.fail();
        return false;
    }
    return true;
}

// write (nested) theta field
bool return_get_rel_loc_of_curr_target::nested_write_theta(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeFloat64(theta)) {
        return false;
    }
    return true;
}
