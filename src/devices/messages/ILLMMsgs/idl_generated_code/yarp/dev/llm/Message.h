/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Autogenerated by Thrift Compiler (0.14.1-yarped)
//
// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_STRUCT_MESSAGE_H
#define YARP_THRIFT_GENERATOR_STRUCT_MESSAGE_H

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>

namespace yarp::dev::llm {

class Message :
        public yarp::os::idl::WirePortable
{
public:
    // Fields
    std::string sender{};
    std::string content{};

    // Default constructor
    Message() = default;

    // Constructor with field values
    Message(const std::string& sender,
            const std::string& content);

    // Read structure on a Wire
    bool read(yarp::os::idl::WireReader& reader) override;

    // Read structure on a Connection
    bool read(yarp::os::ConnectionReader& connection) override;

    // Write structure on a Wire
    bool write(const yarp::os::idl::WireWriter& writer) const override;

    // Write structure on a Connection
    bool write(yarp::os::ConnectionWriter& connection) const override;

    // Convert to a printable string
    std::string toString() const;

    // If you want to serialize this class without nesting, use this helper
    typedef yarp::os::idl::Unwrapped<Message> unwrapped;

private:
    // read/write sender field
    bool read_sender(yarp::os::idl::WireReader& reader);
    bool write_sender(const yarp::os::idl::WireWriter& writer) const;
    bool nested_read_sender(yarp::os::idl::WireReader& reader);
    bool nested_write_sender(const yarp::os::idl::WireWriter& writer) const;

    // read/write content field
    bool read_content(yarp::os::idl::WireReader& reader);
    bool write_content(const yarp::os::idl::WireWriter& writer) const;
    bool nested_read_content(yarp::os::idl::WireReader& reader);
    bool nested_write_content(const yarp::os::idl::WireWriter& writer) const;
};

} // namespace yarp::dev::llm

#endif // YARP_THRIFT_GENERATOR_STRUCT_MESSAGE_H