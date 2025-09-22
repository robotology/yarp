/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IVELOCITYDIRECT_H
#define YARP_DEV_IVELOCITYDIRECT_H

#include <yarp/os/Vocab.h>
#include <yarp/dev/api.h>
#include <yarp/dev/ReturnValue.h>
#include <vector>

namespace yarp::dev {
class IVelocityDirect;
class IVelocityDirectRaw;
}

/**
 * @ingroup dev_iface_motor_raw
 *
 * Interface for control boards implementing direct velocity control.
 */
class YARP_dev_API yarp::dev::IVelocityDirectRaw
{
public:
    /**
     * Destructor.
     */
    virtual ~IVelocityDirectRaw() {}

    /**
     * Get the number of controlled axes.
     * @param axes returned number of controllable axes
     * @return true on success
     */
    virtual yarp::dev::ReturnValue getAxes(size_t& axes) = 0;

    /**
     * Set the velocity of single joint.
     * @param jnt joint number
     * @param vel speed value
     * @return true on success
     */
    virtual yarp::dev::ReturnValue setDesiredVelocityRaw(int jnt, double vel)=0;

    /**
     * Set the velocity of all joints.
     * @param vels vector containing joint velocities for all joints
     * @return true on success
     */
    virtual yarp::dev::ReturnValue setDesiredVelocityRaw(const std::vector<double>& vels)=0;

    /**
     * Set the velocity of a subset of joints.
     * @param jnts vector containing the ids of the joints to control.
     * @param vels vector containing the joint velocities for the required subset.
     * @return true on success
     */
    virtual yarp::dev::ReturnValue setDesiredVelocityRaw(const std::vector<int>& jnts, const std::vector<double>& vels)=0;

    /** Get the last reference velocity set by setDesiredVelocity() for a single joint.
     * @param jnt joint number
     * @param vel returns the velocity reference for the specified joint.
     * @return true on success
     */
    virtual yarp::dev::ReturnValue getDesiredVelocityRaw(const int jnt, double& vel) = 0;

    /** Get the last reference velocity set by setDesiredVelocity() for all joints.
     * @param vels vector containing the velocity references of all joints.
     * @return true on success
     */
    virtual yarp::dev::ReturnValue getDesiredVelocityRaw(std::vector<double>& vels) = 0;

    /** Get the last reference velocity set by setDesiredVelocity() for a group of joints.
     * @param jnts vector containing the ids of the joints to control.
     * @param vels vector containing the velocity references of the joints belonging to the required subset.
     * @return true on success
     */
    virtual yarp::dev::ReturnValue getDesiredVelocityRaw(const std::vector<int>& jnts, std::vector<double>& vels)=0;
};

/**
 * @ingroup dev_iface_motor
 *
 * Interface for control boards implementing direct velocity control.
 */
class YARP_dev_API yarp::dev::IVelocityDirect
{
public:
    /**
     * Destructor.
     */
    virtual ~IVelocityDirect() {}

    /**
     * Get the number of controlled axes.
     * @param axes returned number of controllable axes
     * @return true on success
     */
    virtual yarp::dev::ReturnValue getAxes(size_t& axes) = 0;

    /**
     * Set the velocity of single joint.
     * @param jnt joint number
     * @param vel speed value
     * @return true on success
     */
    virtual yarp::dev::ReturnValue setDesiredVelocity(int jnt, double vel)=0;

    /**
     * Set the velocity of all joints.
     * @param vels vector containing joint velocities for all joints
     * @return true on success
     */
    virtual yarp::dev::ReturnValue setDesiredVelocity(const std::vector<double>& vels)=0;

    /**
     * Set the velocity of a subset of joints.
     * @param jnts vector containing the ids of the joints to control.
     * @param vels vector containing the joint velocities for the required subset.
     * @return true on success
     */
    virtual yarp::dev::ReturnValue setDesiredVelocity(const std::vector<int>& jnts, const std::vector<double>& vels)=0;

    /** Get the last reference velocity set by setDesiredVelocity() for a single joint.
     * @param jnt joint number
     * @param vel returns the velocity reference for the specified joint.
     * @return true on success
     */
    virtual yarp::dev::ReturnValue getDesiredVelocity(const int jnt, double& vel) = 0;

    /** Get the last reference velocity set by setDesiredVelocity() for all joints.
     * @param vels vector containing the velocity references of all joints.
     * @return true on success
     */
    virtual yarp::dev::ReturnValue getDesiredVelocity(std::vector<double>& vels) = 0;

    /** Get the last reference velocity set by setDesiredVelocity() for a group of joints.
     * @param jnts vector containing the ids of the joints to control.
     * @param vels vector containing the velocity references of the joints belonging to the required subset.
     * @return true on success
     */
    virtual yarp::dev::ReturnValue getDesiredVelocity(const std::vector<int>& jnts, std::vector<double>& vels)=0;
};


// Interface name
constexpr yarp::conf::vocab32_t VOCAB_VELOCITYDIRECTCONTROL_INTERFACE   = yarp::os::createVocab32('i','v','e','d');

constexpr yarp::conf::vocab32_t VOCAB_VELOCITY_DIRECT_SET_ONE = yarp::os::createVocab32('v', 'd', 's','o');
constexpr yarp::conf::vocab32_t VOCAB_VELOCITY_DIRECT_SET_ALL   = yarp::os::createVocab32('v','d','s','a');
constexpr yarp::conf::vocab32_t VOCAB_VELOCITY_DIRECT_SET_GROUP = yarp::os::createVocab32('v','d','s','g') ;

#endif // YARP_DEV_IVELOCITYDIRECT_H
