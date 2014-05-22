// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright: 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Alberto Cardellino <alberto.cardellino@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/dev/ControlBoardInterfaces.h>

#ifndef IINTERACTION_MODE_H
#define IINTERACTION_MODE_H

namespace yarp {
    namespace dev {
        class IInteractionMode;
        class IInteractionModeRaw;

        enum InteractionModeEnum
        {
            VOCAB_IM_STIFF        =   VOCAB4('s','t','i','f'),
            VOCAB_IM_COMPLIANT    =   VOCAB4('c','o','m','p'),
            VOCAB_IM_UNKNOWN      =   VOCAB4('u','n','k','n')
        };
    }
}

#define VOCAB_INTERFACE_INTERACTION_MODE    VOCAB4('i','n','t','m')
#define VOCAB_INTERACTION_MODE              VOCAB4('m','o','d','e')
#define VOCAB_INTERACTION_MODE_GROUP        VOCAB4('m','o','d','g')
#define VOCAB_INTERACTION_MODES             VOCAB4('m','o','d','s')



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

    /* Get the current interaction mode of the robot, values can be stiff or compliant.
     * @param axis joint number
     * @param mode contains the requested information about interaction mode of the joint
     * @return true or false on success or failure.
     */
    virtual bool getInteractionMode(int axis, yarp::dev::InteractionModeEnum* mode)=0;


    /* Get the current interaction mode of the robot for a set of joints, values can be stiff or compliant.
     * @param n_joints how many joints this command is referring to
     * @param joints list of joints controlled. The size of this array is n_joints
     * @param modes array containing the requested information about interaction mode, one value for each joint, the size is n_joints.
     *          for example:
     *          n_joint  3
     *          joints   0  2  4
     *          refs    VOCAB_IM_STIFF VOCAB_IM_STIFF VOCAB_IM_COMPLIANT
     * @return true or false on success or failure.
     */
    virtual bool getInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes)=0;


    /* Get the current interaction mode of the robot for a all the joints, values can be stiff or compliant.
     * @param mode array containing the requested information about interaction mode, one value for each joint.
     * @return true or false on success or failure.
     */
    virtual bool getInteractionModes(yarp::dev::InteractionModeEnum* modes)=0;


    /* Set the interaction mode of the robot, values can be stiff or compliant.
     * Please note that some robot may not implement certain types of interaction, so always check the return value.
     * @param axis joint number
     * @param mode the desired interaction mode
     * @return true or false on success or failure.
     */
    virtual bool setInteractionMode(int axis, yarp::dev::InteractionModeEnum mode)=0;


    /* Set the interaction mode of the robot for a set of joints, values can be stiff or compliant.
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
    virtual bool setInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes)=0;

    /* Set the interaction mode of the robot for a all the joints, values can be stiff or compliant.
     * Some robot may not implement some types of interaction, so always check the return value
     * @param mode array with the desired interaction mode for all joints, length is the total number of joints for the part
     * @return true or false on success or failure. If one or more joint fails, the return value will be false.
     */
    virtual bool setInteractionModes(yarp::dev::InteractionModeEnum* modes)=0;
};


/**
 * @ingroup dev_iface_motor
 *
 * Interface settings the way the robot interacts with the environment: basic interaction types are Stiff and Compliant.
 * This setting is intended to work in conjunction with other settings like position or velocity control.
 */
class yarp::dev::IInteractionModeRaw
{
public:
    /**
     * Destructor.
     */
    virtual ~IInteractionModeRaw() {}


    /* Get the current interaction mode of the robot, values can be stiff or compliant.
     * @param axis joint number
     * @param mode contains the requested information about interaction mode of the joint
     * @return true or false on success or failure.
     */
    virtual bool getInteractionModeRaw(int axis, yarp::dev::InteractionModeEnum* mode)=0;


    /* Get the current interaction mode of the robot for a set of joints, values can be stiff or compliant.
     * @param n_joints how many joints this command is referring to
     * @param joints list of joints controlled. The size of this array is n_joints
     * @param modes array containing the requested information about interaction mode, one value for each joint, the size is n_joints.
     *          for example:
     *          n_joint  3
     *          joints   0  2  4
     *          refs    VOCAB_IM_STIFF VOCAB_IM_STIFF VOCAB_IM_COMPLIANT
     * @return true or false on success or failure.
     */
    virtual bool getInteractionModesRaw(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes)=0;


    /* Get the current interaction mode of the robot for a all the joints, values can be stiff or compliant.
     * @param mode array containing the requested information about interaction mode, one value for each joint.
     * @return true or false on success or failure.
     */
    virtual bool getInteractionModesRaw(yarp::dev::InteractionModeEnum* modes)=0;


    /* Set the interaction mode of the robot, values can be stiff or compliant.
     * Please note that some robot may not implement certain types of interaction, so always check the return value.
     * @param axis joint number
     * @param mode the desired interaction mode
     * @return true or false on success or failure.
     */
    virtual bool setInteractionModeRaw(int axis, yarp::dev::InteractionModeEnum mode)=0;


    /* Set the interaction mode of the robot for a set of joints, values can be stiff or compliant.
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
    virtual bool setInteractionModesRaw(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes)=0;

    /* Set the interaction mode of the robot for a all the joints, values can be stiff or compliant.
     * Some robot may not implement some types of interaction, so always check the return value
     * @param mode array with the desired interaction mode for all joints, length is the total number of joints for the part
     * @return true or false on success or failure. If one or more joint fails, the return value will be false.
     */
    virtual bool setInteractionModesRaw(yarp::dev::InteractionModeEnum* modes)=0;
};

#endif /* IINTERACTION_MODE_H */
