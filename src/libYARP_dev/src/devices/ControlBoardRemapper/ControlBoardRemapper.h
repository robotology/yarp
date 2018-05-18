/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_CONTROLBOARDREMAPPER_CONTROLBOARDREMAPPER_H
#define YARP_DEV_CONTROLBOARDREMAPPER_CONTROLBOARDREMAPPER_H

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
 *  @ingroup dev_impl_remappers
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
                             public yarp::dev::IPositionControl,
                             public yarp::dev::IPositionDirect,
                             public yarp::dev::IVelocityControl2,
                             public yarp::dev::IPWMControl,
                             public yarp::dev::ICurrentControl,
                             public yarp::dev::IEncodersTimed,
                             public yarp::dev::IMotor,
                             public yarp::dev::IMotorEncoders,
                             public yarp::dev::IAmplifierControl,
                             public yarp::dev::IControlLimits,
                             public yarp::dev::IRemoteCalibrator,
                             public yarp::dev::IControlCalibration,
                             public yarp::dev::ITorqueControl,
                             public yarp::dev::IImpedanceControl,
                             public yarp::dev::IControlMode,
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
    virtual bool close() override;


    /**
    * Open the device driver.
    * @param prop is a Searchable object which contains the parameters.
    * Allowed parameters are described in the class documentation.
    */
    virtual bool open(yarp::os::Searchable &prop) override;

    virtual bool detachAll() override;

    virtual bool attachAll(const yarp::dev::PolyDriverList &l) override;

    /* IPidControl */
    virtual bool setPid(const PidControlTypeEnum& pidtype,int j, const Pid &p) override;

    virtual bool setPids(const PidControlTypeEnum& pidtype,const Pid *ps) override;

    virtual bool setPidReference(const PidControlTypeEnum& pidtype,int j, double ref) override;

    virtual bool setPidReferences(const PidControlTypeEnum& pidtype,const double *refs) override;

    virtual bool setPidErrorLimit(const PidControlTypeEnum& pidtype,int j, double limit) override;

    virtual bool setPidErrorLimits(const PidControlTypeEnum& pidtype,const double *limits) override;

    virtual bool getPidError(const PidControlTypeEnum& pidtype,int j, double *err) override;

    virtual bool getPidErrors(const PidControlTypeEnum& pidtype,double *errs) override;

    virtual bool getPidOutput(const PidControlTypeEnum& pidtype,int j, double *out) override;

    virtual bool getPidOutputs(const PidControlTypeEnum& pidtype,double *outs) override;

    virtual bool setPidOffset(const PidControlTypeEnum& pidtype,int j, double v) override;

    virtual bool getPid(const PidControlTypeEnum& pidtype,int j, Pid *p) override;

    virtual bool getPids(const PidControlTypeEnum& pidtype,Pid *pids) override;

    virtual bool getPidReference(const PidControlTypeEnum& pidtype,int j, double *ref) override;

    virtual bool getPidReferences(const PidControlTypeEnum& pidtype,double *refs) override;

    virtual bool getPidErrorLimit(const PidControlTypeEnum& pidtype,int j, double *limit) override;

    virtual bool getPidErrorLimits(const PidControlTypeEnum& pidtype,double *limits) override;

    virtual bool resetPid(const PidControlTypeEnum& pidtype,int j) override;

    virtual bool disablePid(const PidControlTypeEnum& pidtype,int j) override;

    virtual bool enablePid(const PidControlTypeEnum& pidtype,int j) override;

    virtual bool isPidEnabled(const PidControlTypeEnum& pidtype, int j, bool* enabled) override;

    /* IPositionControl */
    virtual bool getAxes(int *ax) override;

    virtual bool positionMove(int j, double ref) override;

    virtual bool positionMove(const double *refs) override;

    virtual bool positionMove(const int n_joints, const int *joints, const double *refs) override;

    virtual bool getTargetPosition(const int joint, double *ref) override;

    virtual bool getTargetPositions(double *refs) override;

    virtual bool getTargetPositions(const int n_joint, const int *joints, double *refs) override;

    virtual bool relativeMove(int j, double delta) override;

    virtual bool relativeMove(const double *deltas) override;

    virtual bool relativeMove(const int n_joints, const int *joints, const double *deltas) override;

    virtual bool checkMotionDone(int j, bool *flag) override;

    virtual bool checkMotionDone(bool *flag) override;

    virtual bool checkMotionDone(const int n_joints, const int *joints, bool *flags) override;

    virtual bool setRefSpeed(int j, double sp) override;

    virtual bool setRefSpeeds(const double *spds) override;

    virtual bool setRefSpeeds(const int n_joints, const int *joints, const double *spds) override;

    virtual bool setRefAcceleration(int j, double acc) override;

    virtual bool setRefAccelerations(const double *accs) override;

    virtual bool setRefAccelerations(const int n_joints, const int *joints, const double *accs) override;

    virtual bool getRefSpeed(int j, double *ref) override;

    virtual bool getRefSpeeds(double *spds) override;

    virtual bool getRefSpeeds(const int n_joints, const int *joints, double *spds) override;

    virtual bool getRefAcceleration(int j, double *acc) override;

    virtual bool getRefAccelerations(double *accs) override;

    virtual bool getRefAccelerations(const int n_joints, const int *joints, double *accs) override;

    virtual bool stop(int j) override;

    virtual bool stop() override;

    virtual bool stop(const int n_joints, const int *joints) override;

    /* IVelocityControl */
    virtual bool velocityMove(int j, double v) override;

    virtual bool velocityMove(const double *v) override;

    /* IEncoders */
    virtual bool resetEncoder(int j) override;

    virtual bool resetEncoders() override;

    virtual bool setEncoder(int j, double val) override;

    virtual bool setEncoders(const double *vals) override;

    virtual bool getEncoder(int j, double *v) override;

    virtual bool getEncoders(double *encs) override;

    virtual bool getEncodersTimed(double *encs, double *t) override;

    virtual bool getEncoderTimed(int j, double *v, double *t) override;

    virtual bool getEncoderSpeed(int j, double *sp) override;

    virtual bool getEncoderSpeeds(double *spds) override;

    virtual bool getEncoderAcceleration(int j, double *acc) override;

    virtual bool getEncoderAccelerations(double *accs) override;

    /* IMotorEncoders */
    virtual bool getNumberOfMotorEncoders(int *num) override;

    virtual bool resetMotorEncoder(int m) override;

    virtual bool resetMotorEncoders() override;

    virtual bool setMotorEncoderCountsPerRevolution(int m, const double cpr) override;

    virtual bool getMotorEncoderCountsPerRevolution(int m, double *cpr) override;

    virtual bool setMotorEncoder(int m, const double val) override;

    virtual bool setMotorEncoders(const double *vals) override;

    virtual bool getMotorEncoder(int m, double *v) override;

    virtual bool getMotorEncoders(double *encs) override;

    virtual bool getMotorEncodersTimed(double *encs, double *t) override;

    virtual bool getMotorEncoderTimed(int m, double *v, double *t) override;

    virtual bool getMotorEncoderSpeed(int m, double *sp) override;

    virtual bool getMotorEncoderSpeeds(double *spds) override;

    virtual bool getMotorEncoderAcceleration(int m, double *acc) override;

    virtual bool getMotorEncoderAccelerations(double *accs) override;

    /* IAmplifierControl */
    virtual bool enableAmp(int j) override;

    virtual bool disableAmp(int j) override;

    virtual bool getAmpStatus(int *st) override;

    virtual bool getAmpStatus(int j, int *v) override;

    virtual bool setMaxCurrent(int j, double v) override;

    virtual bool getMaxCurrent(int j, double *v) override;

    virtual bool getNominalCurrent(int m, double *val) override;

    virtual bool setNominalCurrent(int m, const double val) override;

    virtual bool getPeakCurrent(int m, double *val) override;

    virtual bool setPeakCurrent(int m, const double val) override;

    virtual bool getPWM(int m, double *val) override;

    virtual bool getPWMLimit(int m, double *val) override;

    virtual bool setPWMLimit(int m, const double val) override;

    virtual bool getPowerSupplyVoltage(int m, double *val) override;

    /* IControlLimits */
    virtual bool setLimits(int j, double min, double max) override;

    virtual bool getLimits(int j, double *min, double *max) override;

    virtual bool setVelLimits(int j, double min, double max) override;

    virtual bool getVelLimits(int j, double *min, double *max) override;

    /* IRemoteVariables */

    virtual bool getRemoteVariable(std::string key, yarp::os::Bottle &val) override;

    virtual bool setRemoteVariable(std::string key, const yarp::os::Bottle &val) override;

    virtual bool getRemoteVariablesList(yarp::os::Bottle *listOfKeys) override;

    /* IRemoteCalibrator */

    bool isCalibratorDevicePresent(bool *isCalib) override;

    virtual yarp::dev::IRemoteCalibrator *getCalibratorDevice() override;

    virtual bool calibrateSingleJoint(int j) override;

    virtual bool calibrateWholePart() override;

    virtual bool homingSingleJoint(int j) override;

    virtual bool homingWholePart() override;

    virtual bool parkSingleJoint(int j, bool _wait = true) override;

    virtual bool parkWholePart() override;

    virtual bool quitCalibrate() override;

    virtual bool quitPark() override;

    /* IControlCalibration */

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
    virtual bool calibrate(int j, double p) override;
#endif

    virtual bool calibrate(int j, unsigned int ui, double v1, double v2, double v3) override;

    virtual bool setCalibrationParameters(int j, const CalibrationParameters &params) override;

    virtual bool done(int j) override;

    virtual bool abortPark() override;

    virtual bool abortCalibration() override;

    /* IMotor */
    virtual bool getNumberOfMotors(int *num) override;

    virtual bool getTemperature(int m, double *val) override;

    virtual bool getTemperatures(double *vals) override;

    virtual bool getTemperatureLimit(int m, double *val) override;

    virtual bool setTemperatureLimit(int m, const double val) override;

    virtual bool getGearboxRatio(int m, double *val) override;

    virtual bool setGearboxRatio(int m, const double val) override;

    /* IAxisInfo */
    virtual bool getAxisName(int j, std::string &name) override;

    virtual bool getJointType(int j, yarp::dev::JointTypeEnum &type) override;

    virtual bool getRefTorques(double *refs) override;

    virtual bool getRefTorque(int j, double *t) override;

    virtual bool setRefTorques(const double *t) override;

    virtual bool setRefTorque(int j, double t) override;

    virtual bool setRefTorques(const int n_joint, const int *joints, const double *t) override;

    virtual bool getMotorTorqueParams(int j, yarp::dev::MotorTorqueParameters *params) override;

    virtual bool setMotorTorqueParams(int j, const yarp::dev::MotorTorqueParameters params) override;

    virtual bool setImpedance(int j, double stiff, double damp) override;

    virtual bool setImpedanceOffset(int j, double offset) override;

    virtual bool getTorque(int j, double *t) override;

    virtual bool getTorques(double *t) override;

    virtual bool getTorqueRange(int j, double *min, double *max) override;

    virtual bool getTorqueRanges(double *min, double *max) override;

    virtual bool getImpedance(int j, double *stiff, double *damp) override;

    virtual bool getImpedanceOffset(int j, double *offset) override;

    virtual bool getCurrentImpedanceLimit(int j, double *min_stiff, double *max_stiff, double *min_damp,
                                          double *max_damp) override;

    virtual bool getControlMode(int j, int *mode) override;

    virtual bool getControlModes(int *modes) override;

    // iControlMode2
    virtual bool getControlModes(const int n_joint, const int *joints, int *modes) override;

    virtual bool setControlMode(const int j, const int mode) override;

    virtual bool setControlModes(const int n_joints, const int *joints, int *modes) override;

    virtual bool setControlModes(int *modes) override;

    virtual bool setPosition(int j, double ref) override;

    virtual bool setPositions(const int n_joints, const int *joints, double *dpos) override;

    virtual bool setPositions(const double *refs) override;

    virtual bool getRefPosition(const int joint, double *ref) override;

    virtual bool getRefPositions(double *refs) override;

    virtual bool getRefPositions(const int n_joint, const int *joints, double *refs) override;

    virtual yarp::os::Stamp getLastInputStamp() override;

    //
    // IVelocityControl2 Interface
    //
    virtual bool velocityMove(const int n_joints, const int *joints, const double *spds) override;

    virtual bool getRefVelocity(const int joint, double *vel) override;

    virtual bool getRefVelocities(double *vels) override;

    virtual bool getRefVelocities(const int n_joint, const int *joints, double *vels) override;

    virtual bool getInteractionMode(int j, yarp::dev::InteractionModeEnum *mode) override;

    virtual bool getInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum *modes) override;

    virtual bool getInteractionModes(yarp::dev::InteractionModeEnum *modes) override;

    virtual bool setInteractionMode(int j, yarp::dev::InteractionModeEnum mode) override;

    virtual bool setInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum *modes) override;

    virtual bool setInteractionModes(yarp::dev::InteractionModeEnum *modes) override;

    // IPWMControl
    virtual bool setRefDutyCycle(int m, double ref) override;

    virtual bool setRefDutyCycles(const double *refs) override;

    virtual bool getRefDutyCycle(int m, double *ref) override;

    virtual bool getRefDutyCycles(double *refs) override;

    virtual bool getDutyCycle(int m, double *val) override;

    virtual bool getDutyCycles(double *vals) override;

    // ICurrentControl
    virtual bool getCurrent(int m, double *curr) override;

    virtual bool getCurrents(double *currs) override;

    virtual bool getCurrentRange(int m, double *min, double *max) override;

    virtual bool getCurrentRanges(double *min, double *max) override;

    virtual bool setRefCurrents(const double *currs) override;

    virtual bool setRefCurrent(int m, double curr) override;

    virtual bool setRefCurrents(const int n_motor, const int *motors, const double *currs) override;

    virtual bool getRefCurrents(double *currs) override;

    virtual bool getRefCurrent(int m, double *curr) override;
};

}
}

#endif // YARP_DEV_CONTROLBOARDREMAPPER_CONTROLBOARDREMAPPER_H
