/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Autogenerated by Thrift Compiler (0.14.1-yarped)
//
// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <return_transcribe.h>

// Constructor with field values
return_transcribe::return_transcribe(const yarp::dev::ReturnValue& ret,
                                     const std::string& transcription,
                                     const double score) :
        WirePortable(),
        ret(ret),
        transcription(transcription),
        score(score)
{
}

// Read structure on a Wire
bool return_transcribe::read(yarp::os::idl::WireReader& reader)
{
    if (!nested_read_ret(reader)) {
        return false;
    }
    if (!read_transcription(reader)) {
        return false;
    }
    if (!read_score(reader)) {
        return false;
    }
    if (reader.isError()) {
        return false;
    }
    return true;
}

// Read structure on a Connection
bool return_transcribe::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::idl::WireReader reader(connection);
    if (!reader.readListHeader(3)) {
        return false;
    }
    if (!read(reader)) {
        return false;
    }
    return true;
}

// Write structure on a Wire
bool return_transcribe::write(const yarp::os::idl::WireWriter& writer) const
{
    if (!nested_write_ret(writer)) {
        return false;
    }
    if (!write_transcription(writer)) {
        return false;
    }
    if (!write_score(writer)) {
        return false;
    }
    if (writer.isError()) {
        return false;
    }
    return true;
}

// Write structure on a Connection
bool return_transcribe::write(yarp::os::ConnectionWriter& connection) const
{
    yarp::os::idl::WireWriter writer(connection);
    if (!writer.writeListHeader(3)) {
        return false;
    }
    if (!write(writer)) {
        return false;
    }
    return true;
}

// Convert to a printable string
std::string return_transcribe::toString() const
{
    yarp::os::Bottle b;
    if (!yarp::os::Portable::copyPortable(*this, b)) {
        return {};
    }
    return b.toString();
}

// read ret field
bool return_transcribe::read_ret(yarp::os::idl::WireReader& reader)
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
bool return_transcribe::write_ret(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.write(ret)) {
        return false;
    }
    return true;
}

// read (nested) ret field
bool return_transcribe::nested_read_ret(yarp::os::idl::WireReader& reader)
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
bool return_transcribe::nested_write_ret(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeNested(ret)) {
        return false;
    }
    return true;
}

// read transcription field
bool return_transcribe::read_transcription(yarp::os::idl::WireReader& reader)
{
    if (reader.noMore()) {
        reader.fail();
        return false;
    }
    if (!reader.readString(transcription)) {
        reader.fail();
        return false;
    }
    return true;
}

// write transcription field
bool return_transcribe::write_transcription(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeString(transcription)) {
        return false;
    }
    return true;
}

// read (nested) transcription field
bool return_transcribe::nested_read_transcription(yarp::os::idl::WireReader& reader)
{
    if (reader.noMore()) {
        reader.fail();
        return false;
    }
    if (!reader.readString(transcription)) {
        reader.fail();
        return false;
    }
    return true;
}

// write (nested) transcription field
bool return_transcribe::nested_write_transcription(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeString(transcription)) {
        return false;
    }
    return true;
}

// read score field
bool return_transcribe::read_score(yarp::os::idl::WireReader& reader)
{
    if (reader.noMore()) {
        reader.fail();
        return false;
    }
    if (!reader.readFloat64(score)) {
        reader.fail();
        return false;
    }
    return true;
}

// write score field
bool return_transcribe::write_score(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeFloat64(score)) {
        return false;
    }
    return true;
}

// read (nested) score field
bool return_transcribe::nested_read_score(yarp::os::idl::WireReader& reader)
{
    if (reader.noMore()) {
        reader.fail();
        return false;
    }
    if (!reader.readFloat64(score)) {
        reader.fail();
        return false;
    }
    return true;
}

// write (nested) score field
bool return_transcribe::nested_write_score(const yarp::os::idl::WireWriter& writer) const
{
    if (!writer.writeFloat64(score)) {
        return false;
    }
    return true;
}
