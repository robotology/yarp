/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDREMAPPER_CONTROLBOARDREMAPPER_H
#define YARP_DEV_CONTROLBOARDREMAPPER_CONTROLBOARDREMAPPER_H

#include <yarp/os/Network.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/os/Semaphore.h>
#include <yarp/dev/IMultipleWrapper.h>

#include <string>
#include <vector>

#include "ControlBoardRemapperHelpers.h"

#ifdef MSVC
    #pragma warning(disable:4355)
#endif

/**
 * @ingroup dev_impl_remappers
 *
 * @brief `controlboardremapper` : device that takes a list of axes from multiple controlboards and expose them as a single controlboard.
 *
 * | YARP device name |
 * |:-----------------:|
 * | `controlboardremapper` |
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
 * For compatibility with the controlBoard_nws_yarp, the
 * networks keyword can also be used to select the desired joints.
 * For more information on the syntax of the networks, see the
 * controlBoard_nws_yarp class.
 *
 * \code{.unparsed}
 *  networks (net_larm net_lhand)
 *  joints 16
 *  net_larm    0 3  0 3
 *  net_lhand   4 6  0 2
 * \endcode
 *
 */

class ControlBoardRemapper :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IPidControl,
        public yarp::dev::IPositionControl,
        public yarp::dev::IPositionDirect,
        public yarp::dev::IVelocityControl,
        public yarp::dev::IVelocityDirect,
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
        public yarp::dev::IRemoteVariables,
        public yarp::dev::IJointFault,
        public yarp::dev::IJointBrake
{
private:
    std::vector<std::string> axesNames;
    mutable RemappedControlBoards remappedControlBoards;

    /** number of axes controlled by this controlboard */
    int controlledJoints{0};

    /** Verbosity of the class */
    bool _verb{false};

    // to open ports and print more detailed debug messages
    std::string partName;

    // Buffer data used to simplify implementation of multi joint methods
    ControlBoardRemapperBuffers buffers;

    // Buffer data used for full controlboard methods
    ControlBoardSubControlBoardAxesDecomposition allJointsBuffers;

    // Buffer data for multiple arbitrary joint methods
    ControlBoardArbitraryAxesDecomposition selectedJointsBuffers;

    /**
     * Set the number of controlled axes, resizing appropriately
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

    bool usingAxesNamesForAttachAll{false};
    bool usingNetworksForAttachAll{false};

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
    ControlBoardRemapper() = default;
    ControlBoardRemapper(const ControlBoardRemapper&) = delete;
    ControlBoardRemapper(ControlBoardRemapper&&) = delete;
    ControlBoardRemapper& operator=(const ControlBoardRemapper&) = delete;
    ControlBoardRemapper& operator=(ControlBoardRemapper&&) = delete;
    ~ControlBoardRemapper() override = default;

    /**
    * Return the value of the verbose flag.
    * @return the verbose flag.
    */
    bool verbose() const { return _verb; }

    /**
    * Close the device driver by deallocating all resources and closing ports.
    * @return true if successful or false otherwise.
    */
    bool close() override;


    /**
    * Open the device driver.
    * @param prop is a Searchable object which contains the parameters.
    * Allowed parameters are described in the class documentation.
    */
    bool open(yarp::os::Searchable &prop) override;

    bool detachAll() override;

    bool attachAll(const yarp::dev::PolyDriverList &l) override;

    /* IPidControl */
    yarp::dev::ReturnValue setPid(const yarp::dev::PidControlTypeEnum& pidtype,int j, const yarp::dev::Pid &p) override;
    yarp::dev::ReturnValue setPids(const yarp::dev::PidControlTypeEnum& pidtype,const yarp::dev::Pid *ps) override;
    yarp::dev::ReturnValue setPidReference(const yarp::dev::PidControlTypeEnum& pidtype,int j, double ref) override;
    yarp::dev::ReturnValue setPidReferences(const yarp::dev::PidControlTypeEnum& pidtype,const double *refs) override;
    yarp::dev::ReturnValue setPidErrorLimit(const yarp::dev::PidControlTypeEnum& pidtype,int j, double limit) override;
    yarp::dev::ReturnValue setPidErrorLimits(const yarp::dev::PidControlTypeEnum& pidtype,const double *limits) override;
    yarp::dev::ReturnValue getPidError(const yarp::dev::PidControlTypeEnum& pidtype,int j, double *err) override;
    yarp::dev::ReturnValue getPidErrors(const yarp::dev::PidControlTypeEnum& pidtype,double *errs) override;
    yarp::dev::ReturnValue getPidOutput(const yarp::dev::PidControlTypeEnum& pidtype,int j, double *out) override;
    yarp::dev::ReturnValue getPidOutputs(const yarp::dev::PidControlTypeEnum& pidtype,double *outs) override;
    yarp::dev::ReturnValue setPidOffset(const yarp::dev::PidControlTypeEnum& pidtype,int j, double v) override;
    yarp::dev::ReturnValue setPidFeedforward(const yarp::dev::PidControlTypeEnum& pidtype,int j, double v) override;
    yarp::dev::ReturnValue getPidOffset(const yarp::dev::PidControlTypeEnum& pidtype,int j, double& v) override;
    yarp::dev::ReturnValue getPidFeedforward(const yarp::dev::PidControlTypeEnum& pidtype,int j, double& v) override;
    yarp::dev::ReturnValue getPid(const yarp::dev::PidControlTypeEnum& pidtype,int j, yarp::dev::Pid *p) override;
    yarp::dev::ReturnValue getPids(const yarp::dev::PidControlTypeEnum& pidtype,yarp::dev::Pid *pids) override;
    yarp::dev::ReturnValue getPidReference(const yarp::dev::PidControlTypeEnum& pidtype,int j, double *ref) override;
    yarp::dev::ReturnValue getPidReferences(const yarp::dev::PidControlTypeEnum& pidtype,double *refs) override;
    yarp::dev::ReturnValue getPidErrorLimit(const yarp::dev::PidControlTypeEnum& pidtype,int j, double *limit) override;
    yarp::dev::ReturnValue getPidErrorLimits(const yarp::dev::PidControlTypeEnum& pidtype,double *limits) override;
    yarp::dev::ReturnValue resetPid(const yarp::dev::PidControlTypeEnum& pidtype,int j) override;
    yarp::dev::ReturnValue disablePid(const yarp::dev::PidControlTypeEnum& pidtype,int j) override;
    yarp::dev::ReturnValue enablePid(const yarp::dev::PidControlTypeEnum& pidtype,int j) override;
    yarp::dev::ReturnValue isPidEnabled(const yarp::dev::PidControlTypeEnum& pidtype, int j, bool& enabled) override;
    yarp::dev::ReturnValue getPidExtraInfo(const yarp::dev::PidControlTypeEnum& pidtype, int j, yarp::dev::PidExtraInfo& units) override;
    yarp::dev::ReturnValue getPidExtraInfos(const yarp::dev::PidControlTypeEnum& pidtype, std::vector<yarp::dev::PidExtraInfo>& units) override;

    /* IPositionControl */
    yarp::dev::ReturnValue getAxes(int *ax) override;
    yarp::dev::ReturnValue positionMove(int j, double ref) override;
    yarp::dev::ReturnValue positionMove(const double *refs) override;
    yarp::dev::ReturnValue positionMove(const int n_joints, const int *joints, const double *refs) override;
    yarp::dev::ReturnValue getTargetPosition(const int joint, double *ref) override;
    yarp::dev::ReturnValue getTargetPositions(double *refs) override;
    yarp::dev::ReturnValue getTargetPositions(const int n_joint, const int *joints, double *refs) override;
    yarp::dev::ReturnValue relativeMove(int j, double delta) override;
    yarp::dev::ReturnValue relativeMove(const double *deltas) override;
    yarp::dev::ReturnValue relativeMove(const int n_joints, const int *joints, const double *deltas) override;
    yarp::dev::ReturnValue checkMotionDone(int j, bool *flag) override;
    yarp::dev::ReturnValue checkMotionDone(bool *flag) override;
    yarp::dev::ReturnValue checkMotionDone(const int n_joints, const int *joints, bool *flags) override;
    yarp::dev::ReturnValue setTrajSpeed(int j, double sp) override;
    yarp::dev::ReturnValue setTrajSpeeds(const double *spds) override;
    yarp::dev::ReturnValue setTrajSpeeds(const int n_joints, const int *joints, const double *spds) override;
    yarp::dev::ReturnValue setTrajAcceleration(int j, double acc) override;
    yarp::dev::ReturnValue setTrajAccelerations(const double *accs) override;
    yarp::dev::ReturnValue setTrajAccelerations(const int n_joints, const int *joints, const double *accs) override;
    yarp::dev::ReturnValue getTrajSpeed(int j, double *ref) override;
    yarp::dev::ReturnValue getTrajSpeeds(double *spds) override;
    yarp::dev::ReturnValue getTrajSpeeds(const int n_joints, const int *joints, double *spds) override;
    yarp::dev::ReturnValue getTrajAcceleration(int j, double *acc) override;
    yarp::dev::ReturnValue getTrajAccelerations(double *accs) override;
    yarp::dev::ReturnValue getTrajAccelerations(const int n_joints, const int *joints, double *accs) override;
    yarp::dev::ReturnValue stop(int j) override;
    yarp::dev::ReturnValue stop() override;
    yarp::dev::ReturnValue stop(const int n_joints, const int *joints) override;

    /* IJointFault */
    yarp::dev::ReturnValue getLastJointFault(int j, int& fault, std::string& message) override;

    /* IVelocityControl */
    yarp::dev::ReturnValue velocityMove(int j, double v) override;
    yarp::dev::ReturnValue velocityMove(const double *v) override;

    /* IEncoders */
    yarp::dev::ReturnValue resetEncoder(int j) override;
    yarp::dev::ReturnValue resetEncoders() override;
    yarp::dev::ReturnValue setEncoder(int j, double val) override;
    yarp::dev::ReturnValue setEncoders(const double *vals) override;
    yarp::dev::ReturnValue getEncoder(int j, double *v) override;
    yarp::dev::ReturnValue getEncoders(double *encs) override;
    yarp::dev::ReturnValue getEncodersTimed(double *encs, double *t) override;
    yarp::dev::ReturnValue getEncoderTimed(int j, double *v, double *t) override;
    yarp::dev::ReturnValue getEncoderSpeed(int j, double *sp) override;
    yarp::dev::ReturnValue getEncoderSpeeds(double *spds) override;
    yarp::dev::ReturnValue getEncoderAcceleration(int j, double *acc) override;
    yarp::dev::ReturnValue getEncoderAccelerations(double *accs) override;

    /* IMotorEncoders */
    yarp::dev::ReturnValue getNumberOfMotorEncoders(int *num) override;
    yarp::dev::ReturnValue resetMotorEncoder(int m) override;
    yarp::dev::ReturnValue resetMotorEncoders() override;
    yarp::dev::ReturnValue setMotorEncoderCountsPerRevolution(int m, const double cpr) override;
    yarp::dev::ReturnValue getMotorEncoderCountsPerRevolution(int m, double *cpr) override;
    yarp::dev::ReturnValue setMotorEncoder(int m, const double val) override;
    yarp::dev::ReturnValue setMotorEncoders(const double *vals) override;
    yarp::dev::ReturnValue getMotorEncoder(int m, double *v) override;
    yarp::dev::ReturnValue getMotorEncoders(double *encs) override;
    yarp::dev::ReturnValue getMotorEncodersTimed(double *encs, double *t) override;
    yarp::dev::ReturnValue getMotorEncoderTimed(int m, double *v, double *t) override;
    yarp::dev::ReturnValue getMotorEncoderSpeed(int m, double *sp) override;
    yarp::dev::ReturnValue getMotorEncoderSpeeds(double *spds) override;
    yarp::dev::ReturnValue getMotorEncoderAcceleration(int m, double *acc) override;
    yarp::dev::ReturnValue getMotorEncoderAccelerations(double *accs) override;

    /* IAmplifierControl */
    yarp::dev::ReturnValue enableAmp(int j) override;
    yarp::dev::ReturnValue disableAmp(int j) override;
    yarp::dev::ReturnValue getAmpStatus(int *st) override;
    yarp::dev::ReturnValue getAmpStatus(int j, int *v) override;
    yarp::dev::ReturnValue setMaxCurrent(int j, double v) override;
    yarp::dev::ReturnValue getMaxCurrent(int j, double *v) override;
    yarp::dev::ReturnValue getNominalCurrent(int m, double *val) override;
    yarp::dev::ReturnValue setNominalCurrent(int m, const double val) override;
    yarp::dev::ReturnValue getPeakCurrent(int m, double *val) override;
    yarp::dev::ReturnValue setPeakCurrent(int m, const double val) override;
    yarp::dev::ReturnValue getPWM(int m, double *val) override;
    yarp::dev::ReturnValue getPWMLimit(int m, double *val) override;
    yarp::dev::ReturnValue setPWMLimit(int m, const double val) override;
    yarp::dev::ReturnValue getPowerSupplyVoltage(int m, double *val) override;

    /* IControlLimits */
    yarp::dev::ReturnValue setPosLimits(int j, double min, double max) override;
    yarp::dev::ReturnValue getPosLimits(int j, double *min, double *max) override;
    yarp::dev::ReturnValue setVelLimits(int j, double min, double max) override;
    yarp::dev::ReturnValue getVelLimits(int j, double *min, double *max) override;

    /* IRemoteVariables */
    yarp::dev::ReturnValue getRemoteVariable(std::string key, yarp::os::Bottle &val) override;
    yarp::dev::ReturnValue setRemoteVariable(std::string key, const yarp::os::Bottle &val) override;
    yarp::dev::ReturnValue getRemoteVariablesList(yarp::os::Bottle *listOfKeys) override;

    /* IRemoteCalibrator */
    yarp::dev::ReturnValue isCalibratorDevicePresent(bool *isCalib) override;
    yarp::dev::IRemoteCalibrator *getCalibratorDevice() override;
    yarp::dev::ReturnValue calibrateSingleJoint(int j) override;
    yarp::dev::ReturnValue calibrateWholePart() override;
    yarp::dev::ReturnValue homingSingleJoint(int j) override;
    yarp::dev::ReturnValue homingWholePart() override;
    yarp::dev::ReturnValue parkSingleJoint(int j, bool _wait = true) override;
    yarp::dev::ReturnValue parkWholePart() override;
    yarp::dev::ReturnValue quitCalibrate() override;
    yarp::dev::ReturnValue quitPark() override;

    /* IControlCalibration */
    yarp::dev::ReturnValue calibrateAxisWithParams(int j, unsigned int ui, double v1, double v2, double v3) override;
    yarp::dev::ReturnValue setCalibrationParameters(int j, const yarp::dev::CalibrationParameters &params) override;
    yarp::dev::ReturnValue calibrationDone(int j) override;
    yarp::dev::ReturnValue abortPark() override;
    yarp::dev::ReturnValue abortCalibration() override;

    /* IMotor */
    yarp::dev::ReturnValue getNumberOfMotors(int *num) override;
    yarp::dev::ReturnValue getTemperature(int m, double *val) override;
    yarp::dev::ReturnValue getTemperatures(double *vals) override;
    yarp::dev::ReturnValue getTemperatureLimit(int m, double *val) override;
    yarp::dev::ReturnValue setTemperatureLimit(int m, const double val) override;
    yarp::dev::ReturnValue getGearboxRatio(int m, double *val) override;
    yarp::dev::ReturnValue setGearboxRatio(int m, const double val) override;

    /* IAxisInfo */
    yarp::dev::ReturnValue getAxisName(int j, std::string &name) override;
    yarp::dev::ReturnValue getJointType(int j, yarp::dev::JointTypeEnum &type) override;

    /* ITorqueControl */
    yarp::dev::ReturnValue getRefTorques(double *refs) override;
    yarp::dev::ReturnValue getRefTorque(int j, double *t) override;
    yarp::dev::ReturnValue setRefTorques(const double *t) override;
    yarp::dev::ReturnValue setRefTorque(int j, double t) override;
    yarp::dev::ReturnValue setRefTorques(const int n_joint, const int *joints, const double *t) override;
    yarp::dev::ReturnValue getMotorTorqueParams(int j, yarp::dev::MotorTorqueParameters *params) override;
    yarp::dev::ReturnValue setMotorTorqueParams(int j, const yarp::dev::MotorTorqueParameters params) override;
    yarp::dev::ReturnValue setImpedance(int j, double stiff, double damp) override;
    yarp::dev::ReturnValue setImpedanceOffset(int j, double offset) override;
    yarp::dev::ReturnValue getTorque(int j, double *t) override;
    yarp::dev::ReturnValue getTorques(double *t) override;
    yarp::dev::ReturnValue getTorqueRange(int j, double *min, double *max) override;
    yarp::dev::ReturnValue getTorqueRanges(double *min, double *max) override;
    yarp::dev::ReturnValue getImpedance(int j, double *stiff, double *damp) override;
    yarp::dev::ReturnValue getImpedanceOffset(int j, double *offset) override;
    yarp::dev::ReturnValue getCurrentImpedanceLimit(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp) override;
    yarp::dev::ReturnValue getControlMode(int j, int *mode) override;
    yarp::dev::ReturnValue getControlModes(int *modes) override;

    // IControlMode interface
    yarp::dev::ReturnValue getControlModes(const int n_joint, const int *joints, int *modes) override;
    yarp::dev::ReturnValue setControlMode(const int j, const int mode) override;
    yarp::dev::ReturnValue setControlModes(const int n_joints, const int *joints, int *modes) override;
    yarp::dev::ReturnValue setControlModes(int *modes) override;
    yarp::dev::ReturnValue setPosition(int j, double ref) override;
    yarp::dev::ReturnValue setPositions(const int n_joints, const int *joints, const double *dpos) override;
    yarp::dev::ReturnValue setPositions(const double *refs) override;
    yarp::dev::ReturnValue getRefPosition(const int joint, double *ref) override;
    yarp::dev::ReturnValue getRefPositions(double *refs) override;
    yarp::dev::ReturnValue getRefPositions(const int n_joint, const int *joints, double *refs) override;

    yarp::os::Stamp getLastInputStamp() override;

    // IVelocityControl interface
    yarp::dev::ReturnValue velocityMove(const int n_joints, const int *joints, const double *spds) override;
    yarp::dev::ReturnValue getTargetVelocity(const int joint, double *vel) override;
    yarp::dev::ReturnValue getTargetVelocities(double *vels) override;
    yarp::dev::ReturnValue getTargetVelocities(const int n_joint, const int *joints, double *vels) override;
    yarp::dev::ReturnValue getInteractionMode(int j, yarp::dev::InteractionModeEnum *mode) override;
    yarp::dev::ReturnValue getInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum *modes) override;
    yarp::dev::ReturnValue getInteractionModes(yarp::dev::InteractionModeEnum *modes) override;
    yarp::dev::ReturnValue setInteractionMode(int j, yarp::dev::InteractionModeEnum mode) override;
    yarp::dev::ReturnValue setInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum *modes) override;
    yarp::dev::ReturnValue setInteractionModes(yarp::dev::InteractionModeEnum *modes) override;

    // IPWMControl
    yarp::dev::ReturnValue setRefDutyCycle(int m, double ref) override;
    yarp::dev::ReturnValue setRefDutyCycles(const double *refs) override;
    yarp::dev::ReturnValue getRefDutyCycle(int m, double *ref) override;
    yarp::dev::ReturnValue getRefDutyCycles(double *refs) override;
    yarp::dev::ReturnValue getDutyCycle(int m, double *val) override;
    yarp::dev::ReturnValue getDutyCycles(double *vals) override;

    // ICurrentControl
    yarp::dev::ReturnValue getCurrent(int m, double *curr) override;
    yarp::dev::ReturnValue getCurrents(double *currs) override;
    yarp::dev::ReturnValue getCurrentRange(int m, double *min, double *max) override;
    yarp::dev::ReturnValue getCurrentRanges(double *min, double *max) override;
    yarp::dev::ReturnValue setRefCurrents(const double *currs) override;
    yarp::dev::ReturnValue setRefCurrent(int m, double curr) override;
    yarp::dev::ReturnValue setRefCurrents(const int n_motor, const int *motors, const double *currs) override;
    yarp::dev::ReturnValue getRefCurrents(double *currs) override;
    yarp::dev::ReturnValue getRefCurrent(int m, double *curr) override;

    // IJointBrake
    yarp::dev::ReturnValue isJointBraked(int j, bool& braked) const override;
    yarp::dev::ReturnValue setManualBrakeActive(int j, bool active) override;
    yarp::dev::ReturnValue setAutoBrakeEnabled(int j, bool enabled) override;
    yarp::dev::ReturnValue getAutoBrakeEnabled(int j, bool& enabled) const override;

    // IVelocityDirect
    yarp::dev::ReturnValue getAxes(size_t& axes) override;
    yarp::dev::ReturnValue setRefVelocity(int jnt, double vel) override;
    yarp::dev::ReturnValue setRefVelocity(const std::vector<double>& vels) override;
    yarp::dev::ReturnValue setRefVelocity(const std::vector<int>& jnts, const std::vector<double>& vels) override;
    yarp::dev::ReturnValue getRefVelocity(const int jnt, double& vel) override;
    yarp::dev::ReturnValue getRefVelocity(std::vector<double>& vels) override;
    yarp::dev::ReturnValue getRefVelocity(const std::vector<int>& jnts, std::vector<double>& vels) override;
};

#endif // YARP_DEV_CONTROLBOARDREMAPPER_CONTROLBOARDREMAPPER_H
