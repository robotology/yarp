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
#include <yarp/dev/IJointFault.h>
#include <yarp/dev/IJointBrake.h>
#include <yarp/dev/ControlBoardHelpers.h>

#include "stateExtendedReader.h"
#include "RemoteControlBoard_ParamsParser.h"
#include "ControlBoardMsgs.h"

class DiagnosticThread;


/**
* @ingroup dev_impl_network_clients
*
* \brief `remote_controlboard`: The client side of the control board, connects to a remote controlboard using the YARP network.
*
* This device communicates using the YARP ports opened the controlBoard_nws_yarp device
* to use a device exposing controlboard method even from a different process (or even computer)
* from the one that opened the controlboard device.
*
* Parameters required by this device are shown in class: RemoteControlBoard_ParamsParser
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
        public yarp::dev::IVelocityDirect,
        public yarp::dev::IInteractionMode,
        public yarp::dev::IRemoteCalibrator,
        public yarp::dev::IRemoteVariables,
        public yarp::dev::IPWMControl,
        public yarp::dev::ICurrentControl,
        public yarp::dev::IJointFault,
        public yarp::dev::IJointBrake,
        public RemoteControlBoard_ParamsParser
{
protected:
    yarp::os::Port rpc_p;
    yarp::os::Port command_p;
    DiagnosticThread *diagnosticThread{nullptr};
    yarp::os::Port m_rpcPort;

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

    mutable Stamp lastStamp;  //this is shared among all calls that read encoders
    size_t nj{0};
    bool njIsKnown{false};

    ControlBoardMsgs m_RPC;

    // Check for number of joints, if needed.
    // This is to allow for delayed connection to the remote control board.
    bool isLive();

    yarp::dev::ReturnValue send1V(int v);
    yarp::dev::ReturnValue send2V(int v1, int v2);
    yarp::dev::ReturnValue send2V1I(int v1, int v2, int axis);
    yarp::dev::ReturnValue send1V1I(int v, int axis);
    yarp::dev::ReturnValue send3V1I(int v1, int v2, int v3, int j);
    yarp::dev::ReturnValue set1V(int code);
    yarp::dev::ReturnValue set1V2D(int code, double v);
    yarp::dev::ReturnValue set1V1I(int code, int v);
    yarp::dev::ReturnValue get1V1D(int code, double& v) const;
    yarp::dev::ReturnValue get1V1I(int code, int& v) const;
    yarp::dev::ReturnValue set1V1I1D(int code, int j, double val);
    yarp::dev::ReturnValue set1V1I2D(int code, int j, double val1, double val2);
    yarp::dev::ReturnValue set1VDA(int v, const double *val);
    yarp::dev::ReturnValue set2V1DA(int v1, int v2, const double *val);
    yarp::dev::ReturnValue set2V2DA(int v1, int v2, const double *val1, const double *val2);
    yarp::dev::ReturnValue set1V1I1IA1DA(int v, const int len, const int *val1, const double *val2);
    yarp::dev::ReturnValue set2V1I1D(int v1, int v2, int axis, double val);
    yarp::dev::ReturnValue set2V1I(int v1, int v2, int axis);
    yarp::dev::ReturnValue get1V1I1D(int v, int j, double *val);
    yarp::dev::ReturnValue get1V1I1I(int v, int j, int *val);
    yarp::dev::ReturnValue get2V1I1D(int v1, int v2, int j, double *val);
    yarp::dev::ReturnValue get2V1I2D(int v1, int v2, int j, double *val1, double *val2);
    yarp::dev::ReturnValue get1V1I2D(int code, int axis, double *v1, double *v2);
    yarp::dev::ReturnValue get1V1I1B(int v, int j, bool &val);
    yarp::dev::ReturnValue get1V1I1IA1B(int v,  const int len, const int *val1, bool &retVal);
    yarp::dev::ReturnValue get2V1I1IA1DA(int v1, int v2, const int n_joints, const int *joints, double *retVals, std::string functionName = "");
    yarp::dev::ReturnValue get1V1B(int v, bool &val);
    yarp::dev::ReturnValue get1VIA(int v, int *val);
    yarp::dev::ReturnValue get1VDA(int v, double *val);
    yarp::dev::ReturnValue get1V1DA(int v1, double *val);
    yarp::dev::ReturnValue get2V1DA(int v1, int v2, double *val);
    yarp::dev::ReturnValue get2V2DA(int v1, int v2, double *val1, double *val2);
    yarp::dev::ReturnValue get1V1I1S(int code, int j, std::string &name);
    yarp::dev::ReturnValue get1V1I1IA1DA(int v, const int len, const int *val1, double *val2);

    bool setValWithPidType(int voc, PidControlTypeEnum type, int axis, double val);
    bool setValWithPidType(int voc, PidControlTypeEnum type, const double* val_arr);
    bool getValWithPidType(int voc, PidControlTypeEnum type, int j, double *val);
    bool getValWithPidType(int voc, PidControlTypeEnum type, double *val);

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

    yarp::dev::ReturnValue getAxes(int *ax) override;

    // IPidControl
    yarp::dev::ReturnValue getAvailablePids(int j, std::vector<yarp::dev::PidControlTypeEnum>& avail) override;
    yarp::dev::ReturnValue setPid(const PidControlTypeEnum& pidtype, int j, const Pid &pid) override;
    yarp::dev::ReturnValue setPids(const PidControlTypeEnum& pidtype, const Pid *pids) override;
    yarp::dev::ReturnValue setPidReference(const PidControlTypeEnum& pidtype, int j, double ref) override;
    yarp::dev::ReturnValue setPidReferences(const PidControlTypeEnum& pidtype, const double *refs) override;
    yarp::dev::ReturnValue setPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double limit) override;
    yarp::dev::ReturnValue setPidErrorLimits(const PidControlTypeEnum& pidtype, const double *limits) override;
    yarp::dev::ReturnValue getPidError(const PidControlTypeEnum& pidtype, int j, double *err) override;
    yarp::dev::ReturnValue getPidErrors(const PidControlTypeEnum& pidtype, double *errs) override;
    yarp::dev::ReturnValue getPid(const PidControlTypeEnum& pidtype, int j, Pid *pid) override;
    yarp::dev::ReturnValue getPids(const PidControlTypeEnum& pidtype, Pid *pids) override;
    yarp::dev::ReturnValue getPidReference(const PidControlTypeEnum& pidtype, int j, double *ref) override;
    yarp::dev::ReturnValue getPidReferences(const PidControlTypeEnum& pidtype, double *refs) override;
    yarp::dev::ReturnValue getPidErrorLimit(const PidControlTypeEnum& pidtype, int j, double *limit) override;
    yarp::dev::ReturnValue getPidErrorLimits(const PidControlTypeEnum& pidtype, double *limits) override;
    yarp::dev::ReturnValue resetPid(const PidControlTypeEnum& pidtype, int j) override;
    yarp::dev::ReturnValue disablePid(const PidControlTypeEnum& pidtype, int j) override;
    yarp::dev::ReturnValue enablePid(const PidControlTypeEnum& pidtype, int j) override;
    yarp::dev::ReturnValue isPidEnabled(const PidControlTypeEnum& pidtype, int j, bool& enabled) override;
    yarp::dev::ReturnValue getPidOutput(const PidControlTypeEnum& pidtype, int j, double *out) override;
    yarp::dev::ReturnValue getPidOutputs(const PidControlTypeEnum& pidtype, double *outs) override;
    yarp::dev::ReturnValue setPidOffset(const PidControlTypeEnum& pidtype, int j, double v) override;
    yarp::dev::ReturnValue setPidFeedforward(const PidControlTypeEnum& pidtype, int j, double v) override;
    yarp::dev::ReturnValue getPidOffset(const PidControlTypeEnum& pidtype, int j, double& v) override;
    yarp::dev::ReturnValue getPidFeedforward(const PidControlTypeEnum& pidtype, int j, double& v) override;
    yarp::dev::ReturnValue getPidExtraInfo(const PidControlTypeEnum& pidtype, int j, PidExtraInfo& info) override;
    yarp::dev::ReturnValue getPidExtraInfos(const PidControlTypeEnum& pidtype, std::vector<PidExtraInfo>& info) override;

    // IEncoder
    yarp::dev::ReturnValue resetEncoder(int j) override;
    yarp::dev::ReturnValue resetEncoders() override;
    yarp::dev::ReturnValue setEncoder(int j, double val) override;
    yarp::dev::ReturnValue setEncoders(const double *vals) override;
    yarp::dev::ReturnValue getEncoder(int j, double *v) override;
    yarp::dev::ReturnValue getEncoderTimed(int j, double *v, double *t) override;
    yarp::dev::ReturnValue getEncoders(double *encs) override;
    yarp::dev::ReturnValue getEncodersTimed(double *encs, double *ts) override;
    yarp::dev::ReturnValue getEncoderSpeed(int j, double *sp) override;
    yarp::dev::ReturnValue getEncoderSpeeds(double *spds) override;
    yarp::dev::ReturnValue getEncoderAcceleration(int j, double *acc) override;
    yarp::dev::ReturnValue getEncoderAccelerations(double *accs) override;

    // IRemoteVariable
    yarp::dev::ReturnValue getRemoteVariable(std::string key, yarp::os::Bottle& val) override;
    yarp::dev::ReturnValue setRemoteVariable(std::string key, const yarp::os::Bottle& val) override;
    yarp::dev::ReturnValue getRemoteVariablesList(yarp::os::Bottle* listOfKeys) override;

    // IMotor
    yarp::dev::ReturnValue getNumberOfMotors(int *num) override;
    yarp::dev::ReturnValue getTemperature(int m, double* val) override;
    yarp::dev::ReturnValue getTemperatures(double *vals) override;
    yarp::dev::ReturnValue getTemperatureLimit (int m, double* val) override;
    yarp::dev::ReturnValue setTemperatureLimit (int m, const double val) override;
    yarp::dev::ReturnValue getGearboxRatio(int m, double* val) override;
    yarp::dev::ReturnValue setGearboxRatio(int m, const double val) override;

    // IMotorEncoder
    yarp::dev::ReturnValue resetMotorEncoder(int j) override;
    yarp::dev::ReturnValue resetMotorEncoders() override;
    yarp::dev::ReturnValue setMotorEncoder(int j, const double val) override;
    yarp::dev::ReturnValue setMotorEncoderCountsPerRevolution(int m, const double cpr) override;
    yarp::dev::ReturnValue getMotorEncoderCountsPerRevolution(int m, double *cpr) override;
    yarp::dev::ReturnValue setMotorEncoders(const double *vals) override;
    yarp::dev::ReturnValue getMotorEncoder(int j, double *v) override;
    yarp::dev::ReturnValue getMotorEncoderTimed(int j, double *v, double *t) override;
    yarp::dev::ReturnValue getMotorEncoders(double *encs) override;
    yarp::dev::ReturnValue getMotorEncodersTimed(double *encs, double *ts) override;
    yarp::dev::ReturnValue getMotorEncoderSpeed(int j, double *sp) override;
    yarp::dev::ReturnValue getMotorEncoderSpeeds(double *spds) override;
    yarp::dev::ReturnValue getMotorEncoderAcceleration(int j, double *acc) override;
    yarp::dev::ReturnValue getMotorEncoderAccelerations(double *accs) override;
    yarp::dev::ReturnValue getNumberOfMotorEncoders(int *num) override;

    // IPreciselyTimed
    Stamp getLastInputStamp() override;

    // IPositionControl
    yarp::dev::ReturnValue positionMove(int j, double ref) override;
    yarp::dev::ReturnValue positionMove(const int n_joint, const int *joints, const double *refs) override;
    yarp::dev::ReturnValue positionMove(const double *refs) override;
    yarp::dev::ReturnValue getTargetPosition(const int joint, double *ref) override;
    yarp::dev::ReturnValue getTargetPositions(double *refs) override;
    yarp::dev::ReturnValue getTargetPositions(const int n_joint, const int *joints, double *refs) override;
    yarp::dev::ReturnValue relativeMove(int j, double delta) override;
    yarp::dev::ReturnValue relativeMove(const int n_joint, const int *joints, const double *refs) override;
    yarp::dev::ReturnValue relativeMove(const double *deltas) override;
    yarp::dev::ReturnValue checkMotionDone(int j, bool *flag) override;
    yarp::dev::ReturnValue checkMotionDone(const int n_joint, const int *joints, bool *flag) override;
    yarp::dev::ReturnValue checkMotionDone(bool *flag) override;
    yarp::dev::ReturnValue setTrajSpeed(int j, double sp) override;
    yarp::dev::ReturnValue setTrajSpeeds(const int n_joint, const int *joints, const double *spds) override;
    yarp::dev::ReturnValue setTrajSpeeds(const double *spds) override;
    yarp::dev::ReturnValue setTrajAcceleration(int j, double acc) override;
    yarp::dev::ReturnValue setTrajAccelerations(const int n_joint, const int *joints, const double *accs) override;
    yarp::dev::ReturnValue setTrajAccelerations(const double *accs) override;
    yarp::dev::ReturnValue getTrajSpeed(int j, double *ref) override;
    yarp::dev::ReturnValue getTrajSpeeds(const int n_joint, const int *joints, double *spds) override;
    yarp::dev::ReturnValue getTrajSpeeds(double *spds) override;
    yarp::dev::ReturnValue getTrajAcceleration(int j, double *acc) override;
    yarp::dev::ReturnValue getTrajAccelerations(const int n_joint, const int *joints, double *accs) override;
    yarp::dev::ReturnValue getTrajAccelerations(double *accs) override;
    yarp::dev::ReturnValue stop(int j) override;
    yarp::dev::ReturnValue stop(const int len, const int *val1) override;
    yarp::dev::ReturnValue stop() override;

    // IJointFault
    yarp::dev::ReturnValue getLastJointFault(int j, int& fault, std::string& message) override;

    // IAmplifierControl
    yarp::dev::ReturnValue enableAmp(int j) override;
    yarp::dev::ReturnValue disableAmp(int j) override;
    yarp::dev::ReturnValue getAmpStatus(int *st) override;
    yarp::dev::ReturnValue getAmpStatus(int j, int *st) override;
    yarp::dev::ReturnValue setMaxCurrent(int j, double v) override;
    yarp::dev::ReturnValue getMaxCurrent(int j, double *v) override;
    yarp::dev::ReturnValue getNominalCurrent(int m, double *val) override;
    yarp::dev::ReturnValue setNominalCurrent(int m, const double val) override;
    yarp::dev::ReturnValue getPeakCurrent(int m, double *val) override;
    yarp::dev::ReturnValue setPeakCurrent(int m, const double val) override;
    yarp::dev::ReturnValue getPWM(int m, double* val) override;
    yarp::dev::ReturnValue getPWMLimit(int m, double* val) override;
    yarp::dev::ReturnValue setPWMLimit(int m, const double val) override;
    yarp::dev::ReturnValue getPowerSupplyVoltage(int m, double* val) override;

    // IControlLimits
    yarp::dev::ReturnValue setPosLimits(int axis, double min, double max) override;
    yarp::dev::ReturnValue getPosLimits(int axis, double *min, double *max) override;
    yarp::dev::ReturnValue setVelLimits(int axis, double min, double max) override;
    yarp::dev::ReturnValue getVelLimits(int axis, double *min, double *max) override;

    // IAxisInfo
    yarp::dev::ReturnValue getAxisName(int j, std::string& name) override;
    yarp::dev::ReturnValue getJointType(int j, yarp::dev::JointTypeEnum& type) override;

    // IControlCalibration
    yarp::dev::ReturnValue calibrateRobot() override;
    yarp::dev::ReturnValue abortCalibration() override;
    yarp::dev::ReturnValue abortPark() override;
    yarp::dev::ReturnValue park(bool wait=true) override;
    yarp::dev::ReturnValue calibrateAxisWithParams(int j, unsigned int ui, double v1, double v2, double v3) override;
    yarp::dev::ReturnValue setCalibrationParameters(int j, const CalibrationParameters& params) override;
    yarp::dev::ReturnValue calibrationDone(int j) override;

    // ITorqueControl
    yarp::dev::ReturnValue getRefTorque(int j, double *t) override;
    yarp::dev::ReturnValue getRefTorques(double *t) override;
    yarp::dev::ReturnValue setRefTorques(const double *t) override;
    yarp::dev::ReturnValue setRefTorque(int j, double v) override;
    yarp::dev::ReturnValue setRefTorques(const int n_joint, const int *joints, const double *t) override;
    yarp::dev::ReturnValue setMotorTorqueParams(int j, const MotorTorqueParameters params) override;
    yarp::dev::ReturnValue getMotorTorqueParams(int j, MotorTorqueParameters *params) override;
    yarp::dev::ReturnValue getTorque(int j, double *t) override;
    yarp::dev::ReturnValue getTorques(double *t) override;
    yarp::dev::ReturnValue getTorqueRange(int j, double *min, double* max) override;
    yarp::dev::ReturnValue getTorqueRanges(double *min, double *max) override;

    // IImpedanceControl
    yarp::dev::ReturnValue getImpedance(int j, double *stiffness, double *damping) override;
    yarp::dev::ReturnValue getImpedanceOffset(int j, double *offset) override;
    yarp::dev::ReturnValue setImpedance(int j, double stiffness, double damping) override;
    yarp::dev::ReturnValue setImpedanceOffset(int j, double offset) override;
    yarp::dev::ReturnValue getCurrentImpedanceLimit(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp) override;

    // IControlMode
    yarp::dev::ReturnValue getAvailableControlModes(int j, std::vector<yarp::dev::SelectableControlModeEnum>& avail) override;
    yarp::dev::ReturnValue getControlMode(int j, int *mode) override;
    yarp::dev::ReturnValue getControlModes(int *modes) override;
    yarp::dev::ReturnValue getControlModes(const int n_joint, const int *joints, int *modes) override;
    yarp::dev::ReturnValue setControlMode(const int j, const int mode) override;
    yarp::dev::ReturnValue setControlModes(const int n_joint, const int *joints, int *modes) override;
    yarp::dev::ReturnValue setControlModes(int *modes) override;

    // IPositionDirect
    yarp::dev::ReturnValue setPosition(int j, double ref) override;
    yarp::dev::ReturnValue setPositions(const int n_joint, const int *joints, const double *refs) override;
    yarp::dev::ReturnValue setPositions(const double *refs) override;
    yarp::dev::ReturnValue getRefPosition(const int joint, double* ref) override;
    yarp::dev::ReturnValue getRefPositions(double* refs) override;
    yarp::dev::ReturnValue getRefPositions(const int n_joint, const int* joints, double* refs) override;

    // IVelocityControl
    yarp::dev::ReturnValue velocityMove(int j, double v) override;
    yarp::dev::ReturnValue velocityMove(const double *v) override;
    yarp::dev::ReturnValue velocityMove(const int n_joint, const int *joints, const double *spds) override;
    yarp::dev::ReturnValue getTargetVelocity(const int joint, double* vel) override;
    yarp::dev::ReturnValue getTargetVelocities(double* vels) override;
    yarp::dev::ReturnValue getTargetVelocities(const int n_joint, const int* joints, double* vels) override;

    // IInteractionMode
    yarp::dev::ReturnValue getInteractionMode(int axis, yarp::dev::InteractionModeEnum* mode) override;
    yarp::dev::ReturnValue getInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes) override;
    yarp::dev::ReturnValue getInteractionModes(yarp::dev::InteractionModeEnum* modes) override;
    yarp::dev::ReturnValue setInteractionMode(int axis, yarp::dev::InteractionModeEnum mode) override;
    yarp::dev::ReturnValue setInteractionModes(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes) override;
    yarp::dev::ReturnValue setInteractionModes(yarp::dev::InteractionModeEnum* modes) override;

    // IRemoteCalibrator
    yarp::dev::ReturnValue isCalibratorDevicePresent(bool *isCalib) override;
    yarp::dev::ReturnValue calibrateSingleJoint(int j) override;
    yarp::dev::ReturnValue calibrateWholePart() override;
    yarp::dev::ReturnValue homingSingleJoint(int j) override;
    yarp::dev::ReturnValue homingWholePart() override;
    yarp::dev::ReturnValue parkSingleJoint(int j, bool _wait=true) override;
    yarp::dev::ReturnValue parkWholePart() override;
    yarp::dev::ReturnValue quitCalibrate() override;
    yarp::dev::ReturnValue quitPark() override;

    // ICurrentControl
    yarp::dev::ReturnValue getRefCurrents(double *t) override;
    yarp::dev::ReturnValue getRefCurrent(int j, double *t) override;
    yarp::dev::ReturnValue setRefCurrents(const double *refs) override;
    yarp::dev::ReturnValue setRefCurrent(int j, double ref) override;
    yarp::dev::ReturnValue setRefCurrents(const int n_joint, const int *joints, const double *refs) override;
    yarp::dev::ReturnValue getCurrents(double *vals) override;
    yarp::dev::ReturnValue getCurrent(int j, double *val) override;
    yarp::dev::ReturnValue getCurrentRange(int j, double *min, double *max) override;
    yarp::dev::ReturnValue getCurrentRanges(double *min, double *max) override;

    // IPWMControl
    yarp::dev::ReturnValue setRefDutyCycle(int j, double v) override;
    yarp::dev::ReturnValue setRefDutyCycles(const double *v) override;
    yarp::dev::ReturnValue getRefDutyCycle(int j, double *ref) override;
    yarp::dev::ReturnValue getRefDutyCycles(double *refs) override;
    yarp::dev::ReturnValue getDutyCycle(int j, double *out) override;
    yarp::dev::ReturnValue getDutyCycles(double *outs) override;

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
    yarp::dev::ReturnValue getRefVelocity(std::vector<double>& vels)  override;
    yarp::dev::ReturnValue getRefVelocity(const std::vector<int>& jnts, std::vector<double>& vels) override;
};



#endif // YARP_DEV_REMOTECONTROLBOARD_REMOTECONTROLBOARD_H
