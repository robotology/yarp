/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IINTERACTIONMODE_H
#define YARP_DEV_IINTERACTIONMODE_H

#include <yarp/os/Vocab.h>
#include <yarp/dev/api.h>
#include <yarp/dev/ReturnValue.h>

namespace yarp::dev {
class IInteractionMode;
class IInteractionModeRaw;

enum InteractionModeEnum
{
    VOCAB_IM_STIFF = yarp::os::createVocab32('s', 't', 'i', 'f'),
    VOCAB_IM_COMPLIANT = yarp::os::createVocab32('c', 'o', 'm', 'p'),
    VOCAB_IM_UNKNOWN = yarp::os::createVocab32('u', 'n', 'k', 'n')
};
} // namespace yarp::dev

constexpr yarp::conf::vocab32_t VOCAB_INTERFACE_INTERACTION_MODE    = yarp::os::createVocab32('i','n','t','m');
constexpr yarp::conf::vocab32_t VOCAB_INTERACTION_MODE              = yarp::os::createVocab32('m','o','d','e');
constexpr yarp::conf::vocab32_t VOCAB_INTERACTION_MODE_GROUP        = yarp::os::createVocab32('m','o','d','g');
constexpr yarp::conf::vocab32_t VOCAB_INTERACTION_MODES             = yarp::os::createVocab32('m','o','d','s');



/**
 * @ingroup dev_iface_motor
 *
 * Interface settings the way the robot interacts with the environment: basic interaction types are Stiff and Compliant.
 * This setting is intended to work in conjunction with other settings like position or velocity control.
 */
class YARP_dev_API yarp::dev::IInteractionMode
{
public:
    /**
     * Destructor.
     */
    virtual ~IInteractionMode() {}

    /**
     * Get the current interaction mode of the robot, values can be stiff or compliant.
     * @param axis joint number
     * @param mode contains the requested information about interaction mode of the joint
     * @return true or false on success or failure.
     */
    virtual yarp::dev::ReturnValue getInteractionMode(int axis, yarp::dev::InteractionModeEnum* mode)=0;


    /**
     * Get the current interaction mode of the robot for a set of joints, values can be stiff or compliant.
     * @param n_joints how many joints this command is referring to
     * @param joints list of joints controlled. The size of this array is n_joints
     * @param modes array containing the requested information about interaction mode, one value for each joint, the size is n_joints.
     *          for example:
     *          n_joint  3
     *          joints   0  2  4
     *          refs    VOCAB_IM_STIFF VOCAB_IM_STIFF VOCAB_IM_COMPLIANT
     * @return true or false on success or failure.
     */
    virtual yarp::dev::ReturnValue getInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes)=0;


    /**
     * Get the current interaction mode of the robot for a all the joints, values can be stiff or compliant.
     * @param mode array containing the requested information about interaction mode, one value for each joint.
     * @return true or false on success or failure.
     */
    virtual yarp::dev::ReturnValue getInteractionModes(yarp::dev::InteractionModeEnum* modes)=0;


    /**
     * Set the interaction mode of the robot, values can be stiff or compliant.
     * Please note that some robot may not implement certain types of interaction, so always check the return value.
     * @param axis joint number
     * @param mode the desired interaction mode
     * @return true or false on success or failure.
     */
    virtual yarp::dev::ReturnValue setInteractionMode(int axis, yarp::dev::InteractionModeEnum mode)=0;


    /**
     * Set the interaction mode of the robot for a set of joints, values can be stiff or compliant.
     * Please note that some robot may not implement certain types of interaction, so always check the return value.
     * @param n_joints how many joints this command is referring to
     * @param joints list of joints controlled. The size of this array is n_joints
     * @param modes array containing the desired interaction mode, one value for each joint, the size is n_joints.
     *          for example:
     *          n_joint  3
     *          joints   0  2  4
     *          refs    VOCAB_IM_STIFF VOCAB_IM_STIFF VOCAB_IM_COMPLIANT
     * @return true or false on success or failure. If one or more joint fails, the return value will be false.
     */
    virtual yarp::dev::ReturnValue setInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes)=0;

    /**
     * Set the interaction mode of the robot for a all the joints, values can be stiff or compliant.
     * Some robot may not implement some types of interaction, so always check the return value
     * @param mode array with the desired interaction mode for all joints, length is the total number of joints for the part
     * @return true or false on success or failure. If one or more joint fails, the return value will be false.
     */
    virtual yarp::dev::ReturnValue setInteractionModes(yarp::dev::InteractionModeEnum* modes)=0;
};


/**
 * @ingroup dev_iface_motor_raw
 *
 * Interface settings the way the robot interacts with the environment: basic interaction types are Stiff and Compliant.
 * This setting is intended to work in conjunction with other settings like position or velocity control.
 */
class YARP_dev_API yarp::dev::IInteractionModeRaw
{
public:
    /**
     * Destructor.
     */
    virtual ~IInteractionModeRaw() {}


    /**
     * Get the current interaction mode of the robot, values can be stiff or compliant.
     * @param axis joint number
     * @param mode contains the requested information about interaction mode of the joint
     * @return true or false on success or failure.
     */
    virtual yarp::dev::ReturnValue getInteractionModeRaw(int axis, yarp::dev::InteractionModeEnum* mode)=0;


    /**
     * Get the current interaction mode of the robot for a set of joints, values can be stiff or compliant.
     * @param n_joints how many joints this command is referring to
     * @param joints list of joints controlled. The size of this array is n_joints
     * @param modes array containing the requested information about interaction mode, one value for each joint, the size is n_joints.
     *          for example:
     *          n_joint  3
     *          joints   0  2  4
     *          refs    VOCAB_IM_STIFF VOCAB_IM_STIFF VOCAB_IM_COMPLIANT
     * @return true or false on success or failure.
     */
    virtual yarp::dev::ReturnValue getInteractionModesRaw(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes)=0;


    /**
     * Get the current interaction mode of the robot for a all the joints, values can be stiff or compliant.
     * @param mode array containing the requested information about interaction mode, one value for each joint.
     * @return true or false on success or failure.
     */
    virtual yarp::dev::ReturnValue getInteractionModesRaw(yarp::dev::InteractionModeEnum* modes)=0;


    /**
     * Set the interaction mode of the robot, values can be stiff or compliant.
     * Please note that some robot may not implement certain types of interaction, so always check the return value.
     * @param axis joint number
     * @param mode the desired interaction mode
     * @return true or false on success or failure.
     */
    virtual yarp::dev::ReturnValue setInteractionModeRaw(int axis, yarp::dev::InteractionModeEnum mode)=0;


    /**
     * Set the interaction mode of the robot for a set of joints, values can be stiff or compliant.
     * Please note that some robot may not implement certain types of interaction, so always check the return value.
     * @param n_joints how many joints this command is referring to
     * @param joints list of joints controlled. The size of this array is n_joints
     * @param modes array containing the desired interaction mode, one value for each joint, the size is n_joints.
     *          for example:
     *          n_joint  3
     *          joints   0  2  4
     *          refs    VOCAB_IM_STIFF VOCAB_IM_STIFF VOCAB_IM_COMPLIANT
     * @return true or false on success or failure. If one or more joint fails, the return value will be false.
     */
    virtual yarp::dev::ReturnValue setInteractionModesRaw(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes)=0;

    /**
     * Set the interaction mode of the robot for a all the joints, values can be stiff or compliant.
     * Some robot may not implement some types of interaction, so always check the return value
     * @param mode array with the desired interaction mode for all joints, length is the total number of joints for the part
     * @return true or false on success or failure. If one or more joint fails, the return value will be false.
     */
    virtual yarp::dev::ReturnValue setInteractionModesRaw(yarp::dev::InteractionModeEnum* modes)=0;
};

#endif // YARP_DEV_IINTERACTIONMODE_H
