/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Autogenerated by Thrift Compiler (0.14.1-yarped)
//
// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_STRUCT_RETURN_GET_CURRENT_NAV_MAP_H
#define YARP_THRIFT_GENERATOR_STRUCT_RETURN_GET_CURRENT_NAV_MAP_H

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <yarp/dev/MapGrid2D.h>

class return_get_current_nav_map :
        public yarp::os::idl::WirePortable
{
public:
    // Fields
    bool ret{false};
    yarp::dev::Nav2D::MapGrid2D mapgrid{};

    // Default constructor
    return_get_current_nav_map() = default;

    // Constructor with field values
    return_get_current_nav_map(const bool ret,
                               const yarp::dev::Nav2D::MapGrid2D& mapgrid);

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
    typedef yarp::os::idl::Unwrapped<return_get_current_nav_map> unwrapped;

private:
    // read/write ret field
    bool read_ret(yarp::os::idl::WireReader& reader);
    bool write_ret(const yarp::os::idl::WireWriter& writer) const;
    bool nested_read_ret(yarp::os::idl::WireReader& reader);
    bool nested_write_ret(const yarp::os::idl::WireWriter& writer) const;

    // read/write mapgrid field
    bool read_mapgrid(yarp::os::idl::WireReader& reader);
    bool write_mapgrid(const yarp::os::idl::WireWriter& writer) const;
    bool nested_read_mapgrid(yarp::os::idl::WireReader& reader);
    bool nested_write_mapgrid(const yarp::os::idl::WireWriter& writer) const;
};

#endif // YARP_THRIFT_GENERATOR_STRUCT_RETURN_GET_CURRENT_NAV_MAP_H