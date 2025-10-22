/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_ITORQUECONTROL_H
#define YARP_DEV_ITORQUECONTROL_H

#include <yarp/os/Vocab.h>
#include <yarp/dev/api.h>
#include <yarp/dev/ReturnValue.h>

namespace yarp::dev {
class ITorqueControlRaw;
class ITorqueControl;
class MotorTorqueParameters;
}

class YARP_dev_API yarp::dev::MotorTorqueParameters
{
    public:
    double bemf;
    double bemf_scale;
    double ktau;
    double ktau_scale;
    double viscousPos;
    double viscousNeg;
    double coulombPos;
    double coulombNeg;
    double velocityThres;
    MotorTorqueParameters() : bemf(0), bemf_scale(0), ktau(0), ktau_scale(0), viscousPos(0), viscousNeg(0), coulombPos(0), coulombNeg(0), velocityThres(0){};
};

/**
 * @ingroup dev_iface_motor
 *
 * Interface for control boards implementing torque control.
 */
class YARP_dev_API yarp::dev::ITorqueControl
{
public:
    /**
     * Destructor.
     */
    virtual ~ITorqueControl() {}

    /**
     * Get the number of controlled axes. This command asks the number of controlled
     * axes for the current physical interface.
     * @return the number of controlled axes.
     */
    virtual yarp::dev::ReturnValue getAxes(int *ax) = 0;

   /** Get the reference value of the torque for all joints.
     * This is NOT the feedback (see getTorques instead).
     * @param t pointer to the array of torque values
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue getRefTorques(double *t)=0;

    /** Get the reference value of the torque for a given joint.
     * This is NOT the feedback (see getTorque instead).
     * @param j joint number
     * @param t the returned reference torque of joint j
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue getRefTorque(int j, double *t)=0;

    /** Set the reference value of the torque for all joints.
     * @param t pointer to the array of torque values
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue setRefTorques(const double *t)=0;

    /** Set the reference value of the torque for a given joint.
     * @param j joint number
     * @param t new value
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue setRefTorque(int j, double t)=0;

    /** Set new torque reference for a subset of joints.
     * @param joints pointer to the array of joint numbers
     * @param refs   pointer to the array specifying the new torque reference
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue setRefTorques(const int n_joint, const int *joints, const double *t)  =0;

    /** Get a subset of motor parameters (bemf, ktau etc) useful for torque control.
     * @param j joint number
     * @param params a struct containing the motor parameters to be retrieved
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue getMotorTorqueParams(int j,  yarp::dev::MotorTorqueParameters *params)  =0;

    /** Set a subset of motor parameters (bemf, ktau etc) useful for torque control.
     * @param j joint number
     * @param params a struct containing the motor parameters to be set
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue setMotorTorqueParams(int j,  const yarp::dev::MotorTorqueParameters params)  =0;

    /** Get the value of the torque on a given joint (this is the
     * feedback if you have a torque sensor).
     * @param j joint number
     * @param t pointer to the result value
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue getTorque(int j, double *t)=0;

    /** Get the value of the torque for all joints (this is
     * the feedback if you have torque sensors).
     * @param t pointer to the array that will store the output
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue getTorques(double *t)=0;

    /** Get the full scale of the torque sensor of a given joint
     * @param j joint number
     * @param min minimum torque of the joint j
     * @param max maximum torque of the joint j
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue getTorqueRange(int j, double *min, double *max)=0;

    /** Get the full scale of the torque sensors of all joints
     * @param min pointer to the array that will store minimum torques of the joints
     * @param max pointer to the array that will store maximum torques of the joints
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue getTorqueRanges(double *min, double *max)=0;
};

/**
 * @ingroup dev_iface_motor_raw
 *
 * Interface for control boards implementing torque control.
 */
class YARP_dev_API yarp::dev::ITorqueControlRaw
{
public:
    /**
     * Destructor.
     */
    virtual ~ITorqueControlRaw() {}

    /**
     * Get the number of controlled axes. This command asks the number of controlled
     * axes for the current physical interface.
     * @return the number of controlled axes.
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue getAxes(int *ax) = 0;

    /** Get the value of the torque on a given joint (this is the
     * feedback if you have a torque sensor).
     * @param j joint number
     * @return torque value
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue getTorqueRaw(int j, double *t)=0;

    /** Get the value of the torque for all joints (this is
     * the feedback if you have torque sensors).
     * @param t pointer to the array that will store the output
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue getTorquesRaw(double *t)=0;

    /** Get the full scale of the torque sensor of a given joint
     * @param j joint number
     * @param min minimum torque of the joint j
     * @param max maximum torque of the joint j
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue getTorqueRangeRaw(int j, double *min, double *max)=0;

    /** Get the full scale of the torque sensors of all joints
     * @param min pointer to the array that will store minimum torques of the joints
     * @param max pointer to the array that will store maximum torques of the joints
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue getTorqueRangesRaw(double *min, double *max)=0;

    /** Set the reference value of the torque for all joints.
     * @param t pointer to the array of torque values
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue setRefTorquesRaw(const double *t)=0;

    /** Set the reference value of the torque for a given joint.
     * @param j joint number
     * @param t new value
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue setRefTorqueRaw(int j, double t)=0;

    /** Set new torque reference for a subset of joints.
     * @param joints pointer to the array of joint numbers
     * @param refs   pointer to the array specifying the new torque reference
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue setRefTorquesRaw(const int n_joint, const int *joints, const double *t)  =0;

    /** Get the reference value of the torque for all joints.
     * This is NOT the feedback (see getTorques instead).
     * @param t pointer to the array of torque values
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue getRefTorquesRaw(double *t)=0;

    /** Set the reference value of the torque for a given joint.
     * This is NOT the feedback (see getTorque instead).
     * @param j joint number
     * @param t new value
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue getRefTorqueRaw(int j, double *t)=0;

    /** Get the motor parameters.
     * @param j joint number
     * @param params a struct containing the motor parameters to be retrieved
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue getMotorTorqueParamsRaw(int j,  yarp::dev::MotorTorqueParameters *params)  =0;

    /** Set the motor parameters.
     * @param j joint number
     * @param params a struct containing the motor parameters to be set
     * @return true/false on success/failure
     */
    virtual yarp::dev::ReturnValue setMotorTorqueParamsRaw(int j,  const yarp::dev::MotorTorqueParameters params) =0;
};

// Interface name
constexpr yarp::conf::vocab32_t VOCAB_TORQUECONTROL_INTERFACE   = yarp::os::createVocab32('i','t','r','q');

constexpr yarp::conf::vocab32_t VOCAB_TORQUE            = yarp::os::createVocab32('t', 'o', 'r', 'q');
constexpr yarp::conf::vocab32_t VOCAB_TORQUE_MODE       = yarp::os::createVocab32('t', 'r', 'q', 'd');
constexpr yarp::conf::vocab32_t VOCAB_TRQS              = yarp::os::createVocab32('t', 'r', 'q', 's');
constexpr yarp::conf::vocab32_t VOCAB_TRQ               = yarp::os::createVocab32('t', 'r', 'q');
constexpr yarp::conf::vocab32_t VOCAB_BEMF              = yarp::os::createVocab32('b', 'm', 'f');
constexpr yarp::conf::vocab32_t VOCAB_MOTOR_PARAMS      = yarp::os::createVocab32('m', 't', 'p', 's');
constexpr yarp::conf::vocab32_t VOCAB_RANGES            = yarp::os::createVocab32('r', 'n', 'g', 's');
constexpr yarp::conf::vocab32_t VOCAB_RANGE             = yarp::os::createVocab32('r', 'n', 'g');
constexpr yarp::conf::vocab32_t VOCAB_IMP_PARAM         = yarp::os::createVocab32('i', 'p', 'r');
constexpr yarp::conf::vocab32_t VOCAB_IMP_OFFSET        = yarp::os::createVocab32('i', 'o', 'f');
constexpr yarp::conf::vocab32_t VOCAB_IMP_LIMITS        = yarp::os::createVocab32('i', 'l', 'i','m');
constexpr yarp::conf::vocab32_t VOCAB_TORQUES_DIRECTS       = yarp::os::createVocab32('d', 't', 'q', 's'); //This implements the setRefTorques for the whole part
constexpr yarp::conf::vocab32_t VOCAB_TORQUES_DIRECT        = yarp::os::createVocab32('d', 't', 'q'); //This implements the setRefTorque for a single joint
constexpr yarp::conf::vocab32_t VOCAB_TORQUES_DIRECT_GROUP  = yarp::os::createVocab32('d', 't', 'q', 'g'); //This implements the setRefTorques with joint list

#endif // YARP_DEV_ITORQUECONTROL_H
