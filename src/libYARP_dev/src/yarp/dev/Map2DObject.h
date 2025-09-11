/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_MAP2DOBJECT_H
#define YARP_DEV_MAP2DOBJECT_H

#include <sstream>
#include <string>

//#include <yarp/math/Vec2D.h>
#include <yarp/dev/api.h>
#include <yarp/dev/NavTypes.h>
#include <yarp/dev/Map2DObjectData.h>
#include <yarp/dev/Map2DLocation.h>

/**
* \file Map2DObject.h contains the definition of a Map2DObject type
*/
namespace yarp::dev::Nav2D {
struct YARP_dev_API Map2DObject : public Map2DObjectData
{
    /**
     * Constructor
     * @param map_name: the name of the map the Object refers to.
     * @param inX: Object coordinates w.r.t. map reference frame (expressed in meters)
     * @param inY: object coordinates w.r.t. map reference frame (expressed in meters)
     * @param inT: object orientation w.r.t. map reference frame (expressed in degrees)
     */
    Map2DObject(const std::string& map_name, const double& inX, const double& inY, const double& inZ, const std::string& desc = "")
    {
        map_id = map_name;
        x = inX;
        y = inY;
        z = inZ;
        description = desc;
    }

    /**
     * A constructor which accepts a yarp::math::Vec2D, defining the object by its x,y coordinates.
     * The orientation is set to 0 by default.
     * @param map_name: the name of the map the object refers to.
     * @param inX: object coordinates w.r.t. map reference frame (expressed in meters)
     * @param inY: object coordinates w.r.t. map reference frame (expressed in meters)
     * @param inT: object orientation w.r.t. map reference frame (expressed in degrees)
     */
    Map2DObject(const std::string& map_name, const yarp::dev::Nav2D::XYWorld& object, const std::string& desc = "")
    {
        map_id = map_name;
        x = object.x;
        y = object.y;
        z = 0;
        roll = 0;
        pitch = 0;
        yaw = 0;
        description = desc;
    }

    /**
     * Default constructor: the map name is empty, coordinates are set to zero.
     */
    Map2DObject()
    {
        map_id = "";
        description = "";
        x = 0;
        y = 0;
        z = 0;
        roll = 0;
        pitch = 0;
        yaw = 0;
    }

    /**
     * Returns text representation of the object.
     * @return a human readable string containing the object infos.
     */
    std::string toString() const
    {
        std::ostringstream stringStream;
        stringStream.precision(-1);
        stringStream.width(-1);
        stringStream << std::string("map_id:") << map_id << std::string(" x:") << x << std::string(" y:") << y << std::string(" z:") << z;
        stringStream << std::string(" roll:") << roll << std::string(" pitch:") << pitch << std::string(" yaw:") << yaw << " ";
        stringStream << std::string("desc:") << description;
        return stringStream.str();
    }

    /**
     * Compares two Map2DObjects
     * @return true if the two locations are different.
     */
    inline bool operator!=(const Map2DObject& r) const
    {
        if (
            map_id != r.map_id || x != r.x || y != r.y || z != r.z ||
            roll != r.roll || pitch != r.pitch || yaw != r.yaw) {
            return true;
        }
        return false;
    }

    /**
     * Compares two Map2DObjects
     * @return true if the two locations are identical.
     */
    inline bool operator==(const Map2DObject& r) const
    {
        if (
            map_id == r.map_id && x == r.x && y == r.y && z == r.z &&
            roll == r.roll && pitch == r.pitch && yaw == r.yaw) {
            return true;
        }
        return false;
    }

    /**
     * Compares two Map2DObjects
     * @return true if the two locations are near.
     */
    bool is_near_to(const Map2DObject& other_loc, double linear_tolerance) const;
    bool is_near_to(const Map2DLocation& other_loc, double linear_tolerance) const;

    /**
     * Serialization methods
     */
    bool read(yarp::os::idl::WireReader& reader) override
    {
        return Map2DObjectData::read(reader);
    }
    bool write(const yarp::os::idl::WireWriter& writer) const override
    {
        return Map2DObjectData::write(writer);
    }
    bool read(yarp::os::ConnectionReader& reader) override
    {
        return Map2DObjectData::read(reader);
    }
    bool write(yarp::os::ConnectionWriter& writer) const override
    {
        return Map2DObjectData::write(writer);
    }
};
} // namespace yarp::dev::Nav2D

#endif // YARP_DEV_MAP2DLOCATION_H
