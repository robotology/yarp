/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Autogenerated by Thrift Compiler (0.14.1-yarped)
//
// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <return_getDeviceInfo.h>

// Constructor with field values
return_getDeviceInfo::return_getDeviceInfo(const yarp::dev::ReturnValue& retval,
                                           const std::string& device_info) :
        WirePortable(),
        retval(retval),
        device_info(device_info)
{
}

// Read structure on a Wire
bool return_getDeviceInfo::read(yarp::os::idl::WireReader& reader)
{
    if (!nested_read_retval(reader)) {
        return false;
    }
    if (!read_device_info(reader)) {
        return false;
    }
    if (reader.isError()) {
        return false;
    }
    return true;
}

// Read structure on a Connection
bool return_getDeviceInfo::read(yarp::os::ConnectionReader& connection)
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
bool return_getDeviceInfo::write(const yarp::os::idl::WireWriter& writer) const
{
    if (!nested_write_retval(writer)) {
        return false;
    }
    if (!write_device_info(writer)) {
        return false;
    }
    if (writer.isError()) {
        return false;
    }
    return true;
}

// Write structure on a Connection
bool return_getDeviceInfo::write(yarp::os::ConnectionWriter& connection) const
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
std::string return_getDeviceInfo::toString() const
{
    yarp::os::Bottle b;
    if (!yarp::os::Portable::copyPortable(*this, b)) {
        return {};
    }
    return b.toString();
}

// read retval field
bool return_getDeviceInfo::read_retval(yarp::os::idl::WireReader& reader)
{
    if (reader.noMore()) {
        reader.fail();
        return false;
    }
    if (!reader.read(retval)) {
        reader.fail();
        return false;
    }
    return true;
}

// write retval field
bool return_getDeviceInfo::write_retval(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.write(retval)) {
        return false;
    }
    return true;
}

// read (nested) retval field
bool return_getDeviceInfo::nested_read_retval(yarp::os::idl::WireReader& reader)
{
    if (reader.noMore()) {
        reader.fail();
        return false;
    }
    if (!reader.readNested(retval)) {
        reader.fail();
        return false;
    }
    return true;
}

// write (nested) retval field
bool return_getDeviceInfo::nested_write_retval(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeNested(retval)) {
        return false;
    }
    return true;
}

// read device_info field
bool return_getDeviceInfo::read_device_info(yarp::os::idl::WireReader& reader)
{
    if (reader.noMore()) {
        reader.fail();
        return false;
    }
    if (!reader.readString(device_info)) {
        reader.fail();
        return false;
    }
    return true;
}

// write device_info field
bool return_getDeviceInfo::write_device_info(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeString(device_info)) {
        return false;
    }
    return true;
}

// read (nested) device_info field
bool return_getDeviceInfo::nested_read_device_info(yarp::os::idl::WireReader& reader)
{
    if (reader.noMore()) {
        reader.fail();
        return false;
    }
    if (!reader.readString(device_info)) {
        reader.fail();
        return false;
    }
    return true;
}

// write (nested) device_info field
bool return_getDeviceInfo::nested_write_device_info(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeString(device_info)) {
        return false;
    }
    return true;
}
