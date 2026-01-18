/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ISIMULATEDWORLD_H
#define YARP_DEV_ISIMULATEDWORLD_H

#include <string>
#include <vector>

#include <yarp/dev/api.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/ColorRGB.h>
#include <yarp/sig/Pose6D.h>
#include <yarp/dev/ReturnValue.h>

namespace yarp::dev {
class ISimulatedWorld;
}

/**
 * @ingroup dev_iface_others
 *
 * ISimulatedWorld Interface.
 */
class YARP_dev_API yarp::dev::ISimulatedWorld
{
public:
    /**
     * Destructor.
     */
    virtual ~ISimulatedWorld();

    /**
    * Make a sphere.
    * @param id the name of the object
    * @param radius the radius of the sphere [m]
    * @param pose the pose of the sphere [m]
    * @param color the color of the sphere
    * @param frame_name (optional) is specified, the pose will be relative to the specified fully scoped frame (e.g. MODEL_ID::FRAME_ID). Otherwise, world it will be used.
    * @param gravity_enable (optional) enables gravity (default false)
    * @param collision_enable (optional) enables collision (default true)
    * @return returns true or false on success failure
    */
    virtual yarp::dev::ReturnValue makeSphere (std::string id, double radius, yarp::sig::Pose6D pose, yarp::sig::ColorRGB color, std::string frame_name="", bool gravity_enable=0, bool collision_enable=1) =0;

    /**
    * Make a box.
    * @param id the name of the object
    * @param width box width [m]
    * @param height box height[m]
    * @param thickness box thickness [m]
    * @param pose the pose of the box [m]
    * @param color the color of the box
    * @param frame_name (optional) is specified, the pose will be relative to the specified fully scoped frame (e.g. MODEL_ID::FRAME_ID). Otherwise, world it will be used.
    * @param gravity_enable (optional) enables gravity (default false)
    * @param collision_enable (optional) enables collision (default true)
    * @return returns true or false on success failure
    */
    virtual yarp::dev::ReturnValue makeBox (std::string id, double width, double height, double thickness, yarp::sig::Pose6D pose, yarp::sig::ColorRGB color, std::string frame_name="", bool gravity_enable=0, bool collision_enable=1) =0;

    /**
    * Make a cylinder.
    * @param id the name of the object
    * @param radius the radius of the cylinder [m]
    * @param length the length of the cylinder [m]
    * @param pose the pose of the cylinder [m]
    * @param color the color of the cylinder
    * @param frame_name (optional) is specified, the pose will be relative to the specified fully scoped frame (e.g. MODEL_ID::FRAME_ID). Otherwise, world it will be used.
    * @param object_name (optional) assigns a name to the object.
    * @param gravity_enable (optional) enables gravity (default false)
    * @param collision_enable (optional) enables collision (default true)
    * @return returns true or false on success failure
    */
    virtual yarp::dev::ReturnValue makeCylinder (std::string id, double radius, double length, yarp::sig::Pose6D pose, yarp::sig::ColorRGB color, std::string frame_name="", bool gravity_enable=0, bool collision_enable=1) =0;

    /**
    * Make a reference frame.
    * @param id the name of the object
    * @param size the size of the frame [m]
    * @param pose the pose of the frame [m]
    * @param color the color of the frame
    * @param frame_name (optional) is specified, the pose will be relative to the specified fully scoped frame (e.g. MODEL_ID::FRAME_ID). Otherwise, world it will be used.
    * @param object_name (optional) assigns a name to the object.
    * @param gravity_enable (optional) enables gravity (default false)
    * @param collision_enable (optional) enables collision (default true)
    * @return returns true or false on success failure
    */
    virtual yarp::dev::ReturnValue makeFrame (std::string id, double size, yarp::sig::Pose6D pose, yarp::sig::ColorRGB color, std::string frame_name="", bool gravity_enable=0, bool collision_enable=1) =0;

    /**
    * Change the color of an object
    * @param id object id
    * @param color the color of the frame
    * @return returns true or false on success failure
    */
    virtual yarp::dev::ReturnValue changeColor (std::string id, yarp::sig::ColorRGB color) =0;

     /**
    * Set new object pose.
    * @param id object id
    * @param pose new pose
    * @param frame_name (optional) is specified, the pose will be relative to the specified fully scoped frame (e.g. MODEL_ID::FRAME_ID). Otherwise, world it will be used.
    * @return returns true or false on success failure
    */
    virtual yarp::dev::ReturnValue setPose(std::string id, yarp::sig::Pose6D pose, std::string frame_name="") =0;

     /**
    * Enable/disables gravity for an object
    * @param id object id
    * @param enable 1 to enable gravity, 0 otherwise
    * @return returns true or false on success failure
    */
    virtual yarp::dev::ReturnValue enableGravity (std::string id, bool enable) =0;

     /**
    * Enable/disables collision detection for an object
    * @param id object id
    * @param enable 1 to enable collision detection, 0 otherwise
    * @return returns true or false on success failure
    */
    virtual yarp::dev::ReturnValue enableCollision (std::string id, bool enable) =0;

     /**
    * Get object pose.
    * @param id string that identifies object in gazebo (returned after creation)
    * @param frame_name (optional) is specified, the pose will be relative to the specified fully scoped frame (e.g. MODEL_ID::FRAME_ID). Otherwise, world it will be used.
    * @return returns true or false on success failure
    */
    virtual yarp::dev::ReturnValue getPose(std::string id, yarp::sig::Pose6D& pose,  std::string frame_name="") =0;

    /**
    * Create a model from a file.
    * @param id the name of the object
    * @param filename string that specifies the name of the model
    * @param pose the pose of the object [m]
    * @return returns true or false on success failure
    */
    virtual yarp::dev::ReturnValue makeModel(std::string id,  std::string filename, yarp::sig::Pose6D pose,  std::string frame_name, bool gravity_enable, bool collision_enable) =0;

    /**
    * Delete an object.
    * @param id string that identifies object in gazebo (returned after creation)
    * @return returns true or false on success failure
    */
    virtual yarp::dev::ReturnValue deleteObject(std::string id) =0;

    /**
    * Delete all objects in the world.
    * @return returns true or false on success failure
    */
    virtual yarp::dev::ReturnValue deleteAll() =0;

    /**
    * List id of all objects that have been added to the world.
    * @return returns true or false on success failure
    */
    virtual yarp::dev::ReturnValue getList(std::vector<std::string>& names) =0;

    /**
    * Attach an object to a link of the robot.
    * @param id string that identifies object in gazebo (returned after creation)
    * @param link_name name of a link of the robot
    * @return returns true or false on success failure
    */
    virtual yarp::dev::ReturnValue attach(std::string id, std::string link_name) =0;

    /**
    * Detach a previously attached object.
    * @param id string that identifies object in gazebo (returned after creation)
    * @return returns true or false on success failure
    */
    virtual yarp::dev::ReturnValue detach(std::string id) =0;

    /**
    * Change the names of an object.
    * @param old_name string that identifies object in gazebo
    * @param new_name string that will be used as new name
    * @return returns true or false on success failure
    */
    virtual yarp::dev::ReturnValue rename(std::string old_name, std::string new_name) = 0;
};


#endif // YARP_DEV_ISIMULATEDWORLD_H
