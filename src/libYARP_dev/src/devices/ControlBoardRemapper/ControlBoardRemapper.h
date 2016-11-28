/*
* Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
* Author: Lorenzo Natale, Silvio Traversaro
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDREMAPPER_H
#define YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDREMAPPER_H

#include <yarp/os/Network.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/os/Semaphore.h>
#include <yarp/dev/Wrapper.h>

#include <string>
#include <vector>

#include "ControlBoardRemapperHelpers.h"

#ifdef MSVC
    #pragma warning(disable:4355)
#endif

namespace yarp {
namespace dev {


/**
 *  @ingroup dev_impl_wrapper
 *
 * \section ControlBoardRemapper
 * A device that takes a list of axes from multiple controlboards and
 *  expose them as a single controlboard.
 *
 *
 *  Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value | Required                    | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:-------------:|:--------------------------: |:-----------------------------------------------------------------:|:-----:|
 * | axesNames     |      -         | vector of strings  | -      |   -           | Yes     | Ordered list of the axes that are part of the remapped device. |  |
 *
 * The axes are then mapped to the wrapped controlboard in the attachAll method, using the
 * values returned by the getAxisName method of the controlboard. If different axes
 * in two attached controlboard have the same name, the behaviour of this device is undefined.
 *
 * Configuration file using .ini format.
 *
 * \code{.unparsed}
 *  device controlboardremapper
 *  axesNames (joint1 joint2 joint3)
 *
 * ...
 * \endcode
 *
 * For compatibility with the controlboardwrapper2, the
 * networks keyword can also be used to select the desired joints.
 * For more information on the syntax of the networks, see the
 * yarp::dev::ControlBoardWrapper class.
 *
 * \code{.unparsed}
 *  networks (net_larm net_lhand)
 *  joints 16
 *  net_larm    0 3  0 3
 *  net_lhand   4 6  0 2
 * \endcode
 *
 */

class ControlBoardRemapper : public yarp::dev::DeviceDriver,
                             public yarp::dev::IPidControl,
                             public yarp::dev::IPositionControl2,
                             public yarp::dev::IPositionDirect,
                             public yarp::dev::IVelocityControl2,
                             public yarp::dev::IEncodersTimed,
                             public yarp::dev::IMotor,
                             public yarp::dev::IMotorEncoders,
                             public yarp::dev::IAmplifierControl,
                             public yarp::dev::IControlLimits2,
                             public yarp::dev::IRemoteCalibrator,
                             public yarp::dev::IControlCalibration,
                             public yarp::dev::IControlCalibration2,
                             public yarp::dev::IOpenLoopControl,
                             public yarp::dev::ITorqueControl,
                             public yarp::dev::IImpedanceControl,
                             public yarp::dev::IControlMode2,
                             public yarp::dev::IMultipleWrapper,
                             public yarp::dev::IAxisInfo,
                             public yarp::dev::IPreciselyTimed,
                             public yarp::dev::IInteractionMode,
                             public yarp::dev::IRemoteVariables {
private:
    std::vector<std::string> axesNames;
    yarp::dev::RemappedControlBoards remappedControlBoards;

    /** number of axes controlled by this controlboard */
    int controlledJoints;

    /** Verbosity of the class */
    bool _verb;

    // to open ports and print more detailed debug messages
    std::string partName;

    // Buffer data used to simplify implementation of multi joint methods
    ControlBoardRemapperBuffers buffers;

    // Buffer data used for full controlboard methods
    ControlBoardSubControlBoardAxesDecomposition allJointsBuffers;

    // Buffer data for multiple arbitary joint methods
    ControlBoardArbitraryAxesDecomposition selectedJointsBuffers;

    /**
     * Set the number of controlled axes, resizing appropriatly
     * all the necessary buffers.
     */
    void setNrOfControlledAxes(const size_t nrOfControlledAxes);

    /**
     * If the class was configured using the networks format,
     * call this method to update the vector containing the
     * axesName .
     */
    bool updateAxesName();

    /**
     * Configure buffers used by the device
     */
    void configureBuffers();

    // Parse device options
    bool parseOptions(yarp::os::Property &prop);

    bool usingAxesNamesForAttachAll;
    bool usingNetworksForAttachAll;

    /***
     * Parse device options if networks option is passed
     *
     * This will fill the axesNames and controlledJoints attributes, while it
     * leaves empty the remappedDevices attribute that will be then
     * filled only at the attachAll method.
     */
    bool parseAxesNames(const yarp::os::Property &prop);

    /***
     * Parse device options if networks option is passed
     *
     * This will fill the remappedDevices and controlledJoints attributes, while it
     * leaves empty the axesNames attribute that will be then
     * filled only at the attachAll method.
     */
    bool parseNetworks(const yarp::os::Property &prop);

    /**
     * attachAll if the networks option is used for configuration.
     */
    bool attachAllUsingNetworks(const yarp::dev::PolyDriverList &l);

    /**
     * attachAll if the axesNames option is used for configuration.
     */
    bool attachAllUsingAxesNames(const yarp::dev::PolyDriverList &l);

    /**
     * Helper for setting the same control mode in all the axes
     * of the controlboard.
     */
    bool setControlModeAllAxes(const int cm);


public:
    /**
    * Constructor.
    */
    ControlBoardRemapper();

    virtual ~ControlBoardRemapper();

    /**
    * Return the value of the verbose flag.
    * @return the verbose flag.
    */
    bool verbose() const { return _verb; }

    /**
    * Default open() method.
    * @return always false since initialization requires parameters.
    */
    virtual bool open() { return false; }

    /**
    * Close the device driver by deallocating all resources and closing ports.
    * @return true if successful or false otherwise.
    */
    virtual bool close();


    /**
    * Open the device driver.
    * @param prop is a Searchable object which contains the parameters.
    * Allowed parameters are described in the class documentation.
    */
    virtual bool open(yarp::os::Searchable &prop);

    virtual bool detachAll();

    virtual bool attachAll(const yarp::dev::PolyDriverList &l);

    /* IPidControl */
    virtual bool setPid(int j, const Pid &p);

    virtual bool setPids(const Pid *ps);

    virtual bool setReference(int j, double ref);

    virtual bool setReferences(const double *refs);

    virtual bool setErrorLimit(int j, double limit);

    virtual bool setErrorLimits(const double *limits);

    virtual bool getError(int j, double *err);

    virtual bool getErrors(double *errs);

    virtual bool getOutput(int j, double *out);

    virtual bool getOutputs(double *outs);

    virtual bool setOffset(int j, double v);

    virtual bool getPid(int j, Pid *p);

    virtual bool getPids(Pid *pids);

    virtual bool getReference(int j, double *ref);

    virtual bool getReferences(double *refs);

    virtual bool getErrorLimit(int j, double *limit);

    virtual bool getErrorLimits(double *limits);

    virtual bool resetPid(int j);

    virtual bool disablePid(int j);

    virtual bool enablePid(int j);

    /* IPositionControl */
    virtual bool getAxes(int *ax);

    virtual bool setPositionMode();

    virtual bool setOpenLoopMode();

    virtual bool positionMove(int j, double ref);

    virtual bool positionMove(const double *refs);

    virtual bool positionMove(const int n_joints, const int *joints, const double *refs);

    virtual bool getTargetPosition(const int joint, double *ref);

    virtual bool getTargetPositions(double *refs);

    virtual bool getTargetPositions(const int n_joint, const int *joints, double *refs);

    virtual bool relativeMove(int j, double delta);

    virtual bool relativeMove(const double *deltas);

    virtual bool relativeMove(const int n_joints, const int *joints, const double *deltas);

    virtual bool checkMotionDone(int j, bool *flag);

    virtual bool checkMotionDone(bool *flag);

    virtual bool checkMotionDone(const int n_joints, const int *joints, bool *flags);

    virtual bool setRefSpeed(int j, double sp);

    virtual bool setRefSpeeds(const double *spds);

    virtual bool setRefSpeeds(const int n_joints, const int *joints, const double *spds);

    virtual bool setRefAcceleration(int j, double acc);

    virtual bool setRefAccelerations(const double *accs);

    virtual bool setRefAccelerations(const int n_joints, const int *joints, const double *accs);

    virtual bool getRefSpeed(int j, double *ref);

    virtual bool getRefSpeeds(double *spds);

    virtual bool getRefSpeeds(const int n_joints, const int *joints, double *spds);

    virtual bool getRefAcceleration(int j, double *acc);

    virtual bool getRefAccelerations(double *accs);

    virtual bool getRefAccelerations(const int n_joints, const int *joints, double *accs);

    virtual bool stop(int j);

    virtual bool stop();

    virtual bool stop(const int n_joints, const int *joints);

    /* IVelocityControl */
    virtual bool velocityMove(int j, double v);

    virtual bool velocityMove(const double *v);

    virtual bool setVelocityMode();

    /* IEncoders */
    virtual bool resetEncoder(int j);

    virtual bool resetEncoders();

    virtual bool setEncoder(int j, double val);

    virtual bool setEncoders(const double *vals);

    virtual bool getEncoder(int j, double *v);

    virtual bool getEncoders(double *encs);

    virtual bool getEncodersTimed(double *encs, double *t);

    virtual bool getEncoderTimed(int j, double *v, double *t);

    virtual bool getEncoderSpeed(int j, double *sp);

    virtual bool getEncoderSpeeds(double *spds);

    virtual bool getEncoderAcceleration(int j, double *acc);

    virtual bool getEncoderAccelerations(double *accs);

    /* IMotorEncoders */
    virtual bool getNumberOfMotorEncoders(int *num);

    virtual bool resetMotorEncoder(int m);

    virtual bool resetMotorEncoders();

    virtual bool setMotorEncoderCountsPerRevolution(int m, const double cpr);

    virtual bool getMotorEncoderCountsPerRevolution(int m, double *cpr);

    virtual bool setMotorEncoder(int m, const double val);

    virtual bool setMotorEncoders(const double *vals);

    virtual bool getMotorEncoder(int m, double *v);

    virtual bool getMotorEncoders(double *encs);

    virtual bool getMotorEncodersTimed(double *encs, double *t);

    virtual bool getMotorEncoderTimed(int m, double *v, double *t);

    virtual bool getMotorEncoderSpeed(int m, double *sp);

    virtual bool getMotorEncoderSpeeds(double *spds);

    virtual bool getMotorEncoderAcceleration(int m, double *acc);

    virtual bool getMotorEncoderAccelerations(double *accs);

    /* IAmplifierControl */
    virtual bool enableAmp(int j);

    virtual bool disableAmp(int j);

    virtual bool getAmpStatus(int *st);

    virtual bool getAmpStatus(int j, int *v);

    virtual bool getCurrents(double *vals);

    virtual bool getCurrent(int j, double *val);

    virtual bool setMaxCurrent(int j, double v);

    virtual bool getMaxCurrent(int j, double *v);

    virtual bool getNominalCurrent(int m, double *val);

    virtual bool getPeakCurrent(int m, double *val);

    virtual bool setPeakCurrent(int m, const double val);

    virtual bool getPWM(int m, double *val);

    virtual bool getPWMLimit(int m, double *val);

    virtual bool setPWMLimit(int m, const double val);

    virtual bool getPowerSupplyVoltage(int m, double *val);

    /* IControlLimits */
    virtual bool setLimits(int j, double min, double max);

    virtual bool getLimits(int j, double *min, double *max);

    virtual bool setVelLimits(int j, double min, double max);

    virtual bool getVelLimits(int j, double *min, double *max);

    /* IRemoteVariables */

    virtual bool getRemoteVariable(yarp::os::ConstString key, yarp::os::Bottle &val);

    virtual bool setRemoteVariable(yarp::os::ConstString key, const yarp::os::Bottle &val);

    virtual bool getRemoteVariablesList(yarp::os::Bottle *listOfKeys);

    /* IRemoteCalibrator */

    bool isCalibratorDevicePresent(bool *isCalib);

    virtual yarp::dev::IRemoteCalibrator *getCalibratorDevice();

    virtual bool calibrateSingleJoint(int j);

    virtual bool calibrateWholePart();

    virtual bool homingSingleJoint(int j);

    virtual bool homingWholePart();

    virtual bool parkSingleJoint(int j, bool _wait = true);

    virtual bool parkWholePart();

    virtual bool quitCalibrate();

    virtual bool quitPark();

    /* IControlCalibration */

    using yarp::dev::IControlCalibration2::calibrate;

    virtual bool calibrate(int j, double p);

    virtual bool calibrate2(int j, unsigned int ui, double v1, double v2, double v3);

    virtual bool setCalibrationParameters(int j, const CalibrationParameters &params);

    virtual bool done(int j);

    virtual bool abortPark();

    virtual bool abortCalibration();

    /* IMotor */
    virtual bool getNumberOfMotors(int *num);

    virtual bool getTemperature(int m, double *val);

    virtual bool getTemperatures(double *vals);

    virtual bool getTemperatureLimit(int m, double *val);

    virtual bool setTemperatureLimit(int m, const double val);

    virtual bool getGearboxRatio(int m, double *val);

    virtual bool setGearboxRatio(int m, const double val);

    /* IAxisInfo */
    virtual bool getAxisName(int j, yarp::os::ConstString &name);

    virtual bool getJointType(int j, yarp::dev::JointTypeEnum &type);

    virtual bool setTorqueMode();

    virtual bool getRefTorques(double *refs);

    virtual bool getRefTorque(int j, double *t);

    virtual bool setRefTorques(const double *t);

    virtual bool setRefTorque(int j, double t);

    virtual bool setRefTorques(const int n_joint, const int *joints, const double *t);

    virtual bool getBemfParam(int j, double *t);

    virtual bool setBemfParam(int j, double t);

    virtual bool getMotorTorqueParams(int j, yarp::dev::MotorTorqueParameters *params);

    virtual bool setMotorTorqueParams(int j, const yarp::dev::MotorTorqueParameters params);

    virtual bool setTorquePid(int j, const Pid &pid);

    virtual bool setImpedance(int j, double stiff, double damp);

    virtual bool setImpedanceOffset(int j, double offset);

    virtual bool getTorque(int j, double *t);

    virtual bool getTorques(double *t);

    virtual bool getTorqueRange(int j, double *min, double *max);

    virtual bool getTorqueRanges(double *min, double *max);

    virtual bool setTorquePids(const Pid *pids);

    virtual bool setTorqueErrorLimit(int j, double limit);

    virtual bool setTorqueErrorLimits(const double *limits);

    virtual bool getTorqueError(int j, double *err);

    virtual bool getTorqueErrors(double *errs);

    virtual bool getTorquePidOutput(int j, double *out);

    virtual bool getTorquePidOutputs(double *outs);

    virtual bool getTorquePid(int j, Pid *pid);

    virtual bool getImpedance(int j, double *stiff, double *damp);

    virtual bool getImpedanceOffset(int j, double *offset);

    virtual bool getCurrentImpedanceLimit(int j, double *min_stiff, double *max_stiff, double *min_damp,
                                          double *max_damp);

    virtual bool getTorquePids(Pid *pids);

    virtual bool getTorqueErrorLimit(int j, double *limit);

    virtual bool getTorqueErrorLimits(double *limits);

    virtual bool resetTorquePid(int j);

    virtual bool disableTorquePid(int j);

    virtual bool enableTorquePid(int j);

    virtual bool setTorqueOffset(int j, double v);

    virtual bool setPositionMode(int j);

    virtual bool setTorqueMode(int j);

    virtual bool setImpedancePositionMode(int j);

    virtual bool setImpedanceVelocityMode(int j);

    virtual bool setVelocityMode(int j);

    virtual bool setOpenLoopMode(int j);

    virtual bool getControlMode(int j, int *mode);

    virtual bool getControlModes(int *modes);

    // iControlMode2
    virtual bool getControlModes(const int n_joint, const int *joints, int *modes);

    virtual bool setControlMode(const int j, const int mode);

    virtual bool setControlModes(const int n_joints, const int *joints, int *modes);

    virtual bool setControlModes(int *modes);

    virtual bool setRefOutput(int j, double v);

    virtual bool setRefOutputs(const double *outs);

    virtual bool setPositionDirectMode();

    virtual bool setPosition(int j, double ref);

    virtual bool setPositions(const int n_joints, const int *joints, double *dpos);

    virtual bool setPositions(const double *refs);

    virtual bool getRefPosition(const int joint, double *ref);

    virtual bool getRefPositions(double *refs);

    virtual bool getRefPositions(const int n_joint, const int *joints, double *refs);

    virtual yarp::os::Stamp getLastInputStamp();

    //
    // IVelocityControl2 Interface
    //
    virtual bool velocityMove(const int n_joints, const int *joints, const double *spds);

    virtual bool getRefVelocity(const int joint, double *vel);

    virtual bool getRefVelocities(double *vels);

    virtual bool getRefVelocities(const int n_joint, const int *joints, double *vels);

    virtual bool setVelPid(int j, const Pid &pid);

    virtual bool setVelPids(const Pid *pids);

    virtual bool getVelPid(int j, Pid *pid);

    virtual bool getVelPids(Pid *pids);

    virtual bool getInteractionMode(int j, yarp::dev::InteractionModeEnum *mode);

    virtual bool getInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum *modes);

    virtual bool getInteractionModes(yarp::dev::InteractionModeEnum *modes);

    virtual bool setInteractionMode(int j, yarp::dev::InteractionModeEnum mode);

    virtual bool setInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum *modes);

    virtual bool setInteractionModes(yarp::dev::InteractionModeEnum *modes);

    virtual bool getRefOutput(int j, double *out);

    virtual bool getRefOutputs(double *outs);
};

}
}

#endif
