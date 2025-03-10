/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Autogenerated by Thrift Compiler (0.14.1-yarped)
//
// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_STRUCT_RETURN_GET_AREA_H
#define YARP_THRIFT_GENERATOR_STRUCT_RETURN_GET_AREA_H

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <yarp/dev/Map2DArea.h>
#include <yarp/dev/ReturnValue.h>

class return_get_area :
        public yarp::os::idl::WirePortable
{
public:
    // Fields
    yarp::dev::ReturnValue retval{};
    yarp::dev::Nav2D::Map2DArea area{};

    // Default constructor
    return_get_area() = default;

    // Constructor with field values
    return_get_area(const yarp::dev::ReturnValue& retval,
                    const yarp::dev::Nav2D::Map2DArea& area);

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
    typedef yarp::os::idl::Unwrapped<return_get_area> unwrapped;

private:
    // read/write retval field
    bool read_retval(yarp::os::idl::WireReader& reader);
    bool write_retval(const yarp::os::idl::WireWriter& writer) const;
    bool nested_read_retval(yarp::os::idl::WireReader& reader);
    bool nested_write_retval(const yarp::os::idl::WireWriter& writer) const;

    // read/write area field
    bool read_area(yarp::os::idl::WireReader& reader);
    bool write_area(const yarp::os::idl::WireWriter& writer) const;
    bool nested_read_area(yarp::os::idl::WireReader& reader);
    bool nested_write_area(const yarp::os::idl::WireWriter& writer) const;
};

#endif // YARP_THRIFT_GENERATOR_STRUCT_RETURN_GET_AREA_H
