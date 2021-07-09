/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_REMOTECONTROLBOARD_REMOTECONTROLBOARD_H
#define YARP_DEV_REMOTECONTROLBOARD_REMOTECONTROLBOARD_H

#include <yarp/sig/Vector.h>

#include <yarp/dev/IPidControl.h>
#include <yarp/dev/IPositionControl.h>
#include <yarp/dev/IVelocityControl.h>
#include <yarp/dev/IEncodersTimed.h>
#include <yarp/dev/IMotorEncoders.h>
#include <yarp/dev/IMotor.h>
#include <yarp/dev/IAmplifierControl.h>
#include <yarp/dev/IControlLimits.h>
#include <yarp/dev/IAxisInfo.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/dev/IControlCalibration.h>
#include <yarp/dev/ITorqueControl.h>
#include <yarp/dev/IImpedanceControl.h>
#include <yarp/dev/IControlMode.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IPositionDirect.h>
#include <yarp/dev/IInteractionMode.h>
#include <yarp/dev/IRemoteCalibrator.h>
#include <yarp/dev/IRemoteVariables.h>
#include <yarp/dev/IPWMControl.h>
#include <yarp/dev/ICurrentControl.h>
#include <yarp/dev/ControlBoardHelpers.h>

#include "stateExtendedReader.h"

struct ProtocolVersion
{
    int major{0};
    int minor{0};
    int tweak{0};
};

class DiagnosticThread;


/**
* @ingroup dev_impl_network_clients
*
* \brief `remote_controlboard`: The client side of the control board, connects to a remote controlboard using the YARP network.
*
* This device communicates using the YARP ports opened the controlboardwrapper2 device
* to use a device exposing controlboard method even from a different process (or even computer)
* from the one that opened the controlboard device.
*
*  Parameters required by this device are:
* | Parameter name | SubParameter   | Type    | Units | Default Value | Required     | Description                                    | Notes |
* |:--------------:|:--------------:|:-------:|:-----:|:-------------:|:-----------: |:----------------------------------------------:|:-----:|
* | remote         |       -        | string  | -     |   -           | Yes          | Prefix of the port to which to connect.        |       |
* | local          |       -        | string  | -     |   -           | Yes          | Port prefix of the port opened by this device. |       |
* | writeStrict    |       -        | string  | -     | See note      | No           |                                                |       |
*
*/
class RemoteControlBoard :
        public yarp::dev::IPidControl,
        public yarp::dev::IPositionControl,
        public yarp::dev::IVelocityControl,
        public yarp::dev::IEncodersTimed,
        public yarp::dev::IMotorEncoders,
        public yarp::dev::IMotor,
        public yarp::dev::IAmplifierControl,
        public yarp::dev::IControlLimits,
        public yarp::dev::IAxisInfo,
        public yarp::dev::IPreciselyTimed,
        public yarp::dev::IControlCalibration,
        public yarp::dev::ITorqueControl,
        public yarp::dev::IImpedanceControl,
        public yarp::dev::IControlMode,
        public yarp::dev::DeviceDriver,
        public yarp::dev::IPositionDirect,
        public yarp::dev::IInteractionMode,
        public yarp::dev::IRemoteCalibrator,
        public yarp::dev::IRemoteVariables,
        public yarp::dev::IPWMControl,
        public yarp::dev::ICurrentControl
{
protected:
    yarp::os::Port rpc_p;
    yarp::os::Port command_p;
    DiagnosticThread *diagnosticThread{nullptr};

    yarp::os::PortReaderBuffer<yarp::sig::Vector> state_buffer;
    yarp::os::PortWriterBuffer<CommandMessage> command_buffer;
    bool writeStrict_singleJoint{true};
    bool writeStrict_moreJoints{false};

    // Buffer associated to the extendedOutputStatePort port; in this case we will use the type generated
    // from the YARP .thrift file
//  yarp::os::PortReaderBuffer<jointData>           extendedInputState_buffer;  // Buffer storing new data
    StateExtendedInputPort                          extendedIntputStatePort;  // Buffered port storing new data
    std::mutex extendedPortMutex;
    yarp::dev::impl::jointData last_singleJoint;     // tmp to store last received data for a particular joint
//    yarp::os::Port extendedIntputStatePort;         // Port /stateExt:i reading the state of the joints
    yarp::dev::impl::jointData last_wholePart;         // tmp to store last received data for whole part

    std::string remote;
    std::string local;
    mutable Stamp lastStamp;  //this is shared among all calls that read encoders
    size_t nj{0};
    bool njIsKnown{false};

    ProtocolVersion protocolVersion;

    // Check for number of joints, if needed.
    // This is to allow for delayed connection to the remote control board.
    bool isLive();

    bool checkProtocolVersion(bool ignore);

    bool send1V(int v);
    bool send2V(int v1, int v2);
    bool send2V1I(int v1, int v2, int axis);
    bool send1V1I(int v, int axis);
    bool send3V1I(int v1, int v2, int v3, int j);

    /**
     * Send a SET command without parameters and wait for a reply.
     * @param code is the command Vocab identifier.
     * @return true/false on success/failure.
     */
    bool set1V(int code);

    /**
     * Send a SET command and an additional double valued variable
     * and then wait for a reply.
     * @param code is the command to send.
     * @param v is a double valued parameter.
     * @return true/false on success/failure.
     */
    bool set1V2D(int code, double v);

    /**
     * Send a SET command with an additional integer valued variable
     * and then wait for a reply.
     * @param code is the command to send.
     * @param v is an integer valued parameter.
     * @return true/false on success/failure.
     */
    bool set1V1I(int code, int v);

    /**
     * Send a GET command expecting a double value in return.
     * @param code is the Vocab code of the GET command.
     * @param v is a reference to the return variable.
     * @return true/false on success/failure.
     */
    bool get1V1D(int code, double& v) const;

    /**
     * Send a GET command expecting an integer value in return.
     * @param code is the Vocab code of the GET command.
     * @param v is a reference to the return variable.
     * @return true/false on success/failure.
     */
    bool get1V1I(int code, int& v) const;

    /**
     * Helper method to set a double value to a single axis.
     * @param code is the name of the command to be transmitted
     * @param j is the axis
     * @param val is the double value
     * @return true/false on success/failure
     */
    bool set1V1I1D(int code, int j, double val);

    bool set1V1I2D(int code, int j, double val1, double val2);

    /**
     * Helper method used to set an array of double to all axes.
     * @param v is the command to set
     * @param val is the double array (of length nj)
     * @return true/false on success/failure
     */
    bool set1VDA(int v, const double *val);
    bool set2V1DA(int v1, int v2, const double *val);
    bool set2V2DA(int v1, int v2, const double *val1, const double *val2);
    bool set1V1I1IA1DA(int v, const int len, const int *val1, const double *val2);
    bool set2V1I1D(int v1, int v2, int axis, double val);
    bool setValWithPidType(int voc, PidControlTypeEnum type, int axis, double val);
    bool setValWithPidType(int voc, PidControlTypeEnum type, const double* val_arr);
    bool getValWithPidType(int voc, PidControlTypeEnum type, int j, double *val);
    bool getValWithPidType(int voc, PidControlTypeEnum type, double *val);
    bool set2V1I(int v1, int v2, int axis);

    /**
     * Helper method used to get a double value from the remote peer.
     * @param v is the command to query for
     * @param j is the axis number
     * @param val is the return value
     * @return true/false on success/failure
     */
    bool get1V1I1D(int v, int j, double *val);

    /**
     * Helper method used to get an integer value from the remote peer.
     * @param v is the command to query for
     * @param j is the axis number
     * @param val is the return value
     * @return true/false on success/failure
     */
    bool get1V1I1I(int v, int j, int *val);
    bool get2V1I1D(int v1, int v2, int j, double *val);
    bool get2V1I2D(int v1, int v2, int j, double *val1, double *val2);
    bool get1V1I2D(int code, int axis, double *v1, double *v2);

    /**
     * Helper method used to get a double value from the remote peer.
     * @param v is the command to query for
     * @param j is the axis number
     * @param val is the return value
     * @return true/false on success/failure
     */
    bool get1V1I1B(int v, int j, bool &val);
    bool get1V1I1IA1B(int v,  const int len, const int *val1, bool &retVal);
    bool get2V1I1IA1DA(int v1, int v2, const int n_joints, const int *joints, double *retVals, std::string functionName = "");
    bool get1V1B(int v, bool &val);

    /**
     * Helper method to get an array of integers from the remote peer.
     * @param v is the name of the command
     * @param val is the array of double
     * @return true/false on success/failure
     */
    bool get1VIA(int v, int *val);

    /**
     * Helper method to get an array of double from the remote peer.
     * @param v is the name of the command
     * @param val is the array of double
     * @return true/false on success/failure
     */
    bool get1VDA(int v, double *val);

    /**
     * Helper method to get an array of double from the remote peer.
     * @param v1 is the name of the command
     * @param val is the array of double
     * @return true/false on success/failure
     */
    bool get1V1DA(int v1, double *val);

    /**
     * Helper method to get an array of double from the remote peer.
     * @param v1 is the name of the command
     * @param v2 v2
     * @param val is the array of double
     * @return true/false on success/failure
     */
    bool get2V1DA(int v1, int v2, double *val);

    bool get2V2DA(int v1, int v2, double *val1, double *val2);

    bool get1V1I1S(int code, int j, std::string &name);

    bool get1V1I1IA1DA(int v, const int len, const int *val1, double *val2);

public:
    RemoteControlBoard() = default;
    RemoteControlBoard(const RemoteControlBoard&) = delete;
    RemoteControlBoard(RemoteControlBoard&&) = delete;
    RemoteControlBoard& operator=(const RemoteControlBoard&) = delete;
    RemoteControlBoard& operator=(RemoteControlBoard&&) = delete;
    ~RemoteControlBoard() override = default;

    bool open(Searchable& config) override;

    /**
     * Close the device driver and stop the port connections.
     * @return true/false on success/failure.
     */
    bool close() override;

    bool getAxes(int *ax) override;

    // IPidControl
    bool setPid(const PidControlTypeEnum& pidtype, int j, const Pid &pid) override;
    bool setPids(const PidControlTypeEnum& pidtype, const Pid *pids) override;
    bool setPidReference(const PidControlTypeEnum& pidtype, int j, double ref) override;
    bool setPidReferences(const PidControlTypeEnum& pidtype, const double *refs) override;
    bool setPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double limit) override;
    bool setPidErrorLimits(const PidControlTypeEnum& pidtype, const double *limits) override;
    bool getPidError(const PidControlTypeEnum& pidtype, int j, double *err) override;
    bool getPidErrors(const PidControlTypeEnum& pidtype, double *errs) override;
    bool getPid(const PidControlTypeEnum& pidtype, int j, Pid *pid) override;
    bool getPids(const PidControlTypeEnum& pidtype, Pid *pids) override;
    bool getPidReference(const PidControlTypeEnum& pidtype, int j, double *ref) override;
    bool getPidReferences(const PidControlTypeEnum& pidtype, double *refs) override;
    bool getPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double *limit) override;
    bool getPidErrorLimits(const PidControlTypeEnum& pidtype, double *limits) override;
    bool resetPid(const PidControlTypeEnum& pidtype, int j) override;
    bool disablePid(const PidControlTypeEnum& pidtype, int j) override;
    bool enablePid(const PidControlTypeEnum& pidtype, int j) override;
    bool isPidEnabled(const PidControlTypeEnum& pidtype, int j, bool* enabled) override;
    bool getPidOutput(const PidControlTypeEnum& pidtype, int j, double *out) override;
    bool getPidOutputs(const PidControlTypeEnum& pidtype, double *outs) override;
    bool setPidOffset(const PidControlTypeEnum& pidtype, int j, double v) override;

    // IEncoder
    bool resetEncoder(int j) override;
    bool resetEncoders() override;
    bool setEncoder(int j, double val) override;
    bool setEncoders(const double *vals) override;
    bool getEncoder(int j, double *v) override;
    bool getEncoderTimed(int j, double *v, double *t) override;
    bool getEncoders(double *encs) override;
    bool getEncodersTimed(double *encs, double *ts) override;
    bool getEncoderSpeed(int j, double *sp) override;
    bool getEncoderSpeeds(double *spds) override;
    bool getEncoderAcceleration(int j, double *acc) override;
    bool getEncoderAccelerations(double *accs) override;

    // IRemoteVariable
    bool getRemoteVariable(std::string key, yarp::os::Bottle& val) override;
    bool setRemoteVariable(std::string key, const yarp::os::Bottle& val) override;
    bool getRemoteVariablesList(yarp::os::Bottle* listOfKeys) override;

    // IMotor
    bool getNumberOfMotors(int *num) override;
    bool getTemperature(int m, double* val) override;
    bool getTemperatures(double *vals) override;
    bool getTemperatureLimit (int m, double* val) override;
    bool setTemperatureLimit (int m, const double val) override;
    bool getGearboxRatio(int m, double* val) override;
    bool setGearboxRatio(int m, const double val) override;

    // IMotorEncoder
    bool resetMotorEncoder(int j) override;
    bool resetMotorEncoders() override;
    bool setMotorEncoder(int j, const double val) override;
    bool setMotorEncoderCountsPerRevolution(int m, const double cpr) override;
    bool getMotorEncoderCountsPerRevolution(int m, double *cpr) override;
    bool setMotorEncoders(const double *vals) override;
    bool getMotorEncoder(int j, double *v) override;
    bool getMotorEncoderTimed(int j, double *v, double *t) override;
    bool getMotorEncoders(double *encs) override;
    bool getMotorEncodersTimed(double *encs, double *ts) override;
    bool getMotorEncoderSpeed(int j, double *sp) override;
    bool getMotorEncoderSpeeds(double *spds) override;
    bool getMotorEncoderAcceleration(int j, double *acc) override;
    bool getMotorEncoderAccelerations(double *accs) override;
    bool getNumberOfMotorEncoders(int *num) override;

    // IPreciselyTimed
    Stamp getLastInputStamp() override;

    // IPositionControl
    bool positionMove(int j, double ref) override;
    bool positionMove(const int n_joint, const int *joints, const double *refs) override;
    bool positionMove(const double *refs) override;
    bool getTargetPosition(const int joint, double *ref) override;
    bool getTargetPositions(double *refs) override;
    bool getTargetPositions(const int n_joint, const int *joints, double *refs) override;
    bool relativeMove(int j, double delta) override;
    bool relativeMove(const int n_joint, const int *joints, const double *refs) override;
    bool relativeMove(const double *deltas) override;
    bool checkMotionDone(int j, bool *flag) override;
    bool checkMotionDone(const int n_joint, const int *joints, bool *flag) override;
    bool checkMotionDone(bool *flag) override;
    bool setRefSpeed(int j, double sp) override;
    bool setRefSpeeds(const int n_joint, const int *joints, const double *spds) override;
    bool setRefSpeeds(const double *spds) override;
    bool setRefAcceleration(int j, double acc) override;
    bool setRefAccelerations(const int n_joint, const int *joints, const double *accs) override;
    bool setRefAccelerations(const double *accs) override;
    bool getRefSpeed(int j, double *ref) override;
    bool getRefSpeeds(const int n_joint, const int *joints, double *spds) override;
    bool getRefSpeeds(double *spds) override;
    bool getRefAcceleration(int j, double *acc) override;
    bool getRefAccelerations(const int n_joint, const int *joints, double *accs) override;
    bool getRefAccelerations(double *accs) override;
    bool stop(int j) override;
    bool stop(const int len, const int *val1) override;
    bool stop() override;

    // IVelocityControl
    bool velocityMove(int j, double v) override;
    bool velocityMove(const double *v) override;

    // IAmplifierControl
    bool enableAmp(int j) override;
    bool disableAmp(int j) override;
    bool getAmpStatus(int *st) override;
    bool getAmpStatus(int j, int *st) override;
    bool setMaxCurrent(int j, double v) override;
    bool getMaxCurrent(int j, double *v) override;
    bool getNominalCurrent(int m, double *val) override;
    bool setNominalCurrent(int m, const double val) override;
    bool getPeakCurrent(int m, double *val) override;
    bool setPeakCurrent(int m, const double val) override;
    bool getPWM(int m, double* val) override;
    bool getPWMLimit(int m, double* val) override;
    bool setPWMLimit(int m, const double val) override;
    bool getPowerSupplyVoltage(int m, double* val) override;

    // IControlLimits
    bool setLimits(int axis, double min, double max) override;
    bool getLimits(int axis, double *min, double *max) override;
    bool setVelLimits(int axis, double min, double max) override;
    bool getVelLimits(int axis, double *min, double *max) override;

    // IAxisInfo
    bool getAxisName(int j, std::string& name) override;
    bool getJointType(int j, yarp::dev::JointTypeEnum& type) override;

    // IControlCalibration
    bool calibrateRobot() override;
    bool abortCalibration() override;
    bool abortPark() override;
    bool park(bool wait=true) override;
    bool calibrateAxisWithParams(int j, unsigned int ui, double v1, double v2, double v3) override;
    bool setCalibrationParameters(int j, const CalibrationParameters& params) override;
    bool calibrationDone(int j) override;

    // ITorqueControl
    bool getRefTorque(int j, double *t) override;
    bool getRefTorques(double *t) override;
    bool setRefTorques(const double *t) override;
    bool setRefTorque(int j, double v) override;
    bool setRefTorques(const int n_joint, const int *joints, const double *t) override;
    bool setMotorTorqueParams(int j, const MotorTorqueParameters params) override;
    bool getMotorTorqueParams(int j, MotorTorqueParameters *params) override;
    bool getTorque(int j, double *t) override;
    bool getTorques(double *t) override;
    bool getTorqueRange(int j, double *min, double* max) override;
    bool getTorqueRanges(double *min, double *max) override;

    // IImpedanceControl
    bool getImpedance(int j, double *stiffness, double *damping) override;
    bool getImpedanceOffset(int j, double *offset) override;
    bool setImpedance(int j, double stiffness, double damping) override;
    bool setImpedanceOffset(int j, double offset) override;
    bool getCurrentImpedanceLimit(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp) override;

    // IControlMode
    bool getControlMode(int j, int *mode) override;
    bool getControlModes(int *modes) override;
    bool getControlModes(const int n_joint, const int *joints, int *modes) override;
    bool setControlMode(const int j, const int mode) override;
    bool setControlModes(const int n_joint, const int *joints, int *modes) override;
    bool setControlModes(int *modes) override;

    // IPositionDirect
    bool setPosition(int j, double ref) override;
    bool setPositions(const int n_joint, const int *joints, const double *refs) override;
    bool setPositions(const double *refs) override;
    bool getRefPosition(const int joint, double* ref) override;
    bool getRefPositions(double* refs) override;
    bool getRefPositions(const int n_joint, const int* joints, double* refs) override;

    // IVelocityControl
    bool velocityMove(const int n_joint, const int *joints, const double *spds) override;
    bool getRefVelocity(const int joint, double* vel) override;
    bool getRefVelocities(double* vels) override;
    bool getRefVelocities(const int n_joint, const int* joints, double* vels) override;

    // IInteractionMode
    bool getInteractionMode(int axis, yarp::dev::InteractionModeEnum* mode) override;
    bool getInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes) override;
    bool getInteractionModes(yarp::dev::InteractionModeEnum* modes) override;
    bool setInteractionMode(int axis, yarp::dev::InteractionModeEnum mode) override;
    bool setInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes) override;
    bool setInteractionModes(yarp::dev::InteractionModeEnum* modes) override;

    // IRemoteCalibrator
    bool isCalibratorDevicePresent(bool *isCalib) override;
    bool calibrateSingleJoint(int j) override;
    bool calibrateWholePart() override;
    bool homingSingleJoint(int j) override;
    bool homingWholePart() override;
    bool parkSingleJoint(int j, bool _wait=true) override;
    bool parkWholePart() override;
    bool quitCalibrate() override;
    bool quitPark() override;

    // ICurrentControl
    bool getRefCurrents(double *t) override;
    bool getRefCurrent(int j, double *t) override;
    bool setRefCurrents(const double *refs) override;
    bool setRefCurrent(int j, double ref) override;
    bool setRefCurrents(const int n_joint, const int *joints, const double *refs) override;
    bool getCurrents(double *vals) override;
    bool getCurrent(int j, double *val) override;
    bool getCurrentRange(int j, double *min, double *max) override;
    bool getCurrentRanges(double *min, double *max) override;

    // IPWMControl
    bool setRefDutyCycle(int j, double v) override;
    bool setRefDutyCycles(const double *v) override;
    bool getRefDutyCycle(int j, double *ref) override;
    bool getRefDutyCycles(double *refs) override;
    bool getDutyCycle(int j, double *out) override;
    bool getDutyCycles(double *outs) override;
};



#endif // YARP_DEV_REMOTECONTROLBOARD_REMOTECONTROLBOARD_H
