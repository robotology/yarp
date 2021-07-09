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
 * For compatibility with the controlboardwrapper2, the
 * networks keyword can also be used to select the desired joints.
 * For more information on the syntax of the networks, see the
 * ControlBoardWrapper class.
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
    RemappedControlBoards remappedControlBoards;

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
    bool setPid(const yarp::dev::PidControlTypeEnum& pidtype,int j, const yarp::dev::Pid &p) override;

    bool setPids(const yarp::dev::PidControlTypeEnum& pidtype,const yarp::dev::Pid *ps) override;

    bool setPidReference(const yarp::dev::PidControlTypeEnum& pidtype,int j, double ref) override;

    bool setPidReferences(const yarp::dev::PidControlTypeEnum& pidtype,const double *refs) override;

    bool setPidErrorLimit(const yarp::dev::PidControlTypeEnum& pidtype,int j, double limit) override;

    bool setPidErrorLimits(const yarp::dev::PidControlTypeEnum& pidtype,const double *limits) override;

    bool getPidError(const yarp::dev::PidControlTypeEnum& pidtype,int j, double *err) override;

    bool getPidErrors(const yarp::dev::PidControlTypeEnum& pidtype,double *errs) override;

    bool getPidOutput(const yarp::dev::PidControlTypeEnum& pidtype,int j, double *out) override;

    bool getPidOutputs(const yarp::dev::PidControlTypeEnum& pidtype,double *outs) override;

    bool setPidOffset(const yarp::dev::PidControlTypeEnum& pidtype,int j, double v) override;

    bool getPid(const yarp::dev::PidControlTypeEnum& pidtype,int j, yarp::dev::Pid *p) override;

    bool getPids(const yarp::dev::PidControlTypeEnum& pidtype,yarp::dev::Pid *pids) override;

    bool getPidReference(const yarp::dev::PidControlTypeEnum& pidtype,int j, double *ref) override;

    bool getPidReferences(const yarp::dev::PidControlTypeEnum& pidtype,double *refs) override;

    bool getPidErrorLimit(const yarp::dev::PidControlTypeEnum& pidtype,int j, double *limit) override;

    bool getPidErrorLimits(const yarp::dev::PidControlTypeEnum& pidtype,double *limits) override;

    bool resetPid(const yarp::dev::PidControlTypeEnum& pidtype,int j) override;

    bool disablePid(const yarp::dev::PidControlTypeEnum& pidtype,int j) override;

    bool enablePid(const yarp::dev::PidControlTypeEnum& pidtype,int j) override;

    bool isPidEnabled(const yarp::dev::PidControlTypeEnum& pidtype, int j, bool* enabled) override;

    /* IPositionControl */
    bool getAxes(int *ax) override;

    bool positionMove(int j, double ref) override;

    bool positionMove(const double *refs) override;

    bool positionMove(const int n_joints, const int *joints, const double *refs) override;

    bool getTargetPosition(const int joint, double *ref) override;

    bool getTargetPositions(double *refs) override;

    bool getTargetPositions(const int n_joint, const int *joints, double *refs) override;

    bool relativeMove(int j, double delta) override;

    bool relativeMove(const double *deltas) override;

    bool relativeMove(const int n_joints, const int *joints, const double *deltas) override;

    bool checkMotionDone(int j, bool *flag) override;

    bool checkMotionDone(bool *flag) override;

    bool checkMotionDone(const int n_joints, const int *joints, bool *flags) override;

    bool setRefSpeed(int j, double sp) override;

    bool setRefSpeeds(const double *spds) override;

    bool setRefSpeeds(const int n_joints, const int *joints, const double *spds) override;

    bool setRefAcceleration(int j, double acc) override;

    bool setRefAccelerations(const double *accs) override;

    bool setRefAccelerations(const int n_joints, const int *joints, const double *accs) override;

    bool getRefSpeed(int j, double *ref) override;

    bool getRefSpeeds(double *spds) override;

    bool getRefSpeeds(const int n_joints, const int *joints, double *spds) override;

    bool getRefAcceleration(int j, double *acc) override;

    bool getRefAccelerations(double *accs) override;

    bool getRefAccelerations(const int n_joints, const int *joints, double *accs) override;

    bool stop(int j) override;

    bool stop() override;

    bool stop(const int n_joints, const int *joints) override;

    /* IVelocityControl */
    bool velocityMove(int j, double v) override;

    bool velocityMove(const double *v) override;

    /* IEncoders */
    bool resetEncoder(int j) override;

    bool resetEncoders() override;

    bool setEncoder(int j, double val) override;

    bool setEncoders(const double *vals) override;

    bool getEncoder(int j, double *v) override;

    bool getEncoders(double *encs) override;

    bool getEncodersTimed(double *encs, double *t) override;

    bool getEncoderTimed(int j, double *v, double *t) override;

    bool getEncoderSpeed(int j, double *sp) override;

    bool getEncoderSpeeds(double *spds) override;

    bool getEncoderAcceleration(int j, double *acc) override;

    bool getEncoderAccelerations(double *accs) override;

    /* IMotorEncoders */
    bool getNumberOfMotorEncoders(int *num) override;

    bool resetMotorEncoder(int m) override;

    bool resetMotorEncoders() override;

    bool setMotorEncoderCountsPerRevolution(int m, const double cpr) override;

    bool getMotorEncoderCountsPerRevolution(int m, double *cpr) override;

    bool setMotorEncoder(int m, const double val) override;

    bool setMotorEncoders(const double *vals) override;

    bool getMotorEncoder(int m, double *v) override;

    bool getMotorEncoders(double *encs) override;

    bool getMotorEncodersTimed(double *encs, double *t) override;

    bool getMotorEncoderTimed(int m, double *v, double *t) override;

    bool getMotorEncoderSpeed(int m, double *sp) override;

    bool getMotorEncoderSpeeds(double *spds) override;

    bool getMotorEncoderAcceleration(int m, double *acc) override;

    bool getMotorEncoderAccelerations(double *accs) override;

    /* IAmplifierControl */
    bool enableAmp(int j) override;

    bool disableAmp(int j) override;

    bool getAmpStatus(int *st) override;

    bool getAmpStatus(int j, int *v) override;

    bool setMaxCurrent(int j, double v) override;

    bool getMaxCurrent(int j, double *v) override;

    bool getNominalCurrent(int m, double *val) override;

    bool setNominalCurrent(int m, const double val) override;

    bool getPeakCurrent(int m, double *val) override;

    bool setPeakCurrent(int m, const double val) override;

    bool getPWM(int m, double *val) override;

    bool getPWMLimit(int m, double *val) override;

    bool setPWMLimit(int m, const double val) override;

    bool getPowerSupplyVoltage(int m, double *val) override;

    /* IControlLimits */
    bool setLimits(int j, double min, double max) override;

    bool getLimits(int j, double *min, double *max) override;

    bool setVelLimits(int j, double min, double max) override;

    bool getVelLimits(int j, double *min, double *max) override;

    /* IRemoteVariables */

    bool getRemoteVariable(std::string key, yarp::os::Bottle &val) override;

    bool setRemoteVariable(std::string key, const yarp::os::Bottle &val) override;

    bool getRemoteVariablesList(yarp::os::Bottle *listOfKeys) override;

    /* IRemoteCalibrator */

    bool isCalibratorDevicePresent(bool *isCalib) override;

    yarp::dev::IRemoteCalibrator *getCalibratorDevice() override;

    bool calibrateSingleJoint(int j) override;

    bool calibrateWholePart() override;

    bool homingSingleJoint(int j) override;

    bool homingWholePart() override;

    bool parkSingleJoint(int j, bool _wait = true) override;

    bool parkWholePart() override;

    bool quitCalibrate() override;

    bool quitPark() override;

    /* IControlCalibration */

    bool calibrateAxisWithParams(int j, unsigned int ui, double v1, double v2, double v3) override;

    bool setCalibrationParameters(int j, const yarp::dev::CalibrationParameters &params) override;

    bool calibrationDone(int j) override;

    bool abortPark() override;

    bool abortCalibration() override;

    /* IMotor */
    bool getNumberOfMotors(int *num) override;

    bool getTemperature(int m, double *val) override;

    bool getTemperatures(double *vals) override;

    bool getTemperatureLimit(int m, double *val) override;

    bool setTemperatureLimit(int m, const double val) override;

    bool getGearboxRatio(int m, double *val) override;

    bool setGearboxRatio(int m, const double val) override;

    /* IAxisInfo */
    bool getAxisName(int j, std::string &name) override;

    bool getJointType(int j, yarp::dev::JointTypeEnum &type) override;

    bool getRefTorques(double *refs) override;

    bool getRefTorque(int j, double *t) override;

    bool setRefTorques(const double *t) override;

    bool setRefTorque(int j, double t) override;

    bool setRefTorques(const int n_joint, const int *joints, const double *t) override;

    bool getMotorTorqueParams(int j, yarp::dev::MotorTorqueParameters *params) override;

    bool setMotorTorqueParams(int j, const yarp::dev::MotorTorqueParameters params) override;

    bool setImpedance(int j, double stiff, double damp) override;

    bool setImpedanceOffset(int j, double offset) override;

    bool getTorque(int j, double *t) override;

    bool getTorques(double *t) override;

    bool getTorqueRange(int j, double *min, double *max) override;

    bool getTorqueRanges(double *min, double *max) override;

    bool getImpedance(int j, double *stiff, double *damp) override;

    bool getImpedanceOffset(int j, double *offset) override;

    bool getCurrentImpedanceLimit(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp) override;

    bool getControlMode(int j, int *mode) override;

    bool getControlModes(int *modes) override;

    // IControlMode interface
    bool getControlModes(const int n_joint, const int *joints, int *modes) override;

    bool setControlMode(const int j, const int mode) override;

    bool setControlModes(const int n_joints, const int *joints, int *modes) override;

    bool setControlModes(int *modes) override;

    bool setPosition(int j, double ref) override;

    bool setPositions(const int n_joints, const int *joints, const double *dpos) override;

    bool setPositions(const double *refs) override;

    bool getRefPosition(const int joint, double *ref) override;

    bool getRefPositions(double *refs) override;

    bool getRefPositions(const int n_joint, const int *joints, double *refs) override;

    yarp::os::Stamp getLastInputStamp() override;

    // IVelocityControl interface
    bool velocityMove(const int n_joints, const int *joints, const double *spds) override;

    bool getRefVelocity(const int joint, double *vel) override;

    bool getRefVelocities(double *vels) override;

    bool getRefVelocities(const int n_joint, const int *joints, double *vels) override;

    bool getInteractionMode(int j, yarp::dev::InteractionModeEnum *mode) override;

    bool getInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum *modes) override;

    bool getInteractionModes(yarp::dev::InteractionModeEnum *modes) override;

    bool setInteractionMode(int j, yarp::dev::InteractionModeEnum mode) override;

    bool setInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum *modes) override;

    bool setInteractionModes(yarp::dev::InteractionModeEnum *modes) override;

    // IPWMControl
    bool setRefDutyCycle(int m, double ref) override;

    bool setRefDutyCycles(const double *refs) override;

    bool getRefDutyCycle(int m, double *ref) override;

    bool getRefDutyCycles(double *refs) override;

    bool getDutyCycle(int m, double *val) override;

    bool getDutyCycles(double *vals) override;

    // ICurrentControl
    bool getCurrent(int m, double *curr) override;

    bool getCurrents(double *currs) override;

    bool getCurrentRange(int m, double *min, double *max) override;

    bool getCurrentRanges(double *min, double *max) override;

    bool setRefCurrents(const double *currs) override;

    bool setRefCurrent(int m, double curr) override;

    bool setRefCurrents(const int n_motor, const int *motors, const double *currs) override;

    bool getRefCurrents(double *currs) override;

    bool getRefCurrent(int m, double *curr) override;
};

#endif // YARP_DEV_CONTROLBOARDREMAPPER_CONTROLBOARDREMAPPER_H
