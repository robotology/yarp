/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

enum yJointTypeEnum {
} (
  yarp.name = "yarp::dev::JointTypeEnum"
  yarp.includefile = "yarp/dev/IAxisInfo.h"
  yarp.enumbase = "int32_t"
)

struct yReturnValue {
} (
  yarp.name = "yarp::dev::ReturnValue"
  yarp.includefile = "yarp/dev/ReturnValue.h"
)

struct yCalibrationParameters {
  1: i32 type;
  2: double param1;
  3: double param2;
  4: double param3;
  5: double param4;
  6: double param5;
  7: double paramZero;
}

struct yMotorTorqueParameters
{
  1: double bemf;
  2: double bemf_scale;
  3: double ktau;
  4: double ktau_scale;
  5: double viscousPos;
  6: double viscousNeg;
  7: double coulombPos;
  8: double coulombNeg;
  9: double velocityThres;
}

struct yInfoPid {
} (
  yarp.name = "yarp::dev::PidExtraInfo"
  yarp.includefile = "yarp/dev/ControlBoardPid.h"
)

struct yBottle {
} (
  yarp.name = "yarp::os::Bottle"
  yarp.includefile = "yarp/os/Bottle.h"
)

struct yPid {
} (
  yarp.name = "yarp::dev::Pid"
  yarp.includefile = "yarp/dev/ControlBoardPid.h"
)

enum yPidControlTypeEnum {
} (
  yarp.name = "yarp::dev::PidControlTypeEnum"
  yarp.includefile = "yarp/dev/PidEnums.h"
  yarp.enumbase = "int32_t"
)

enum yInteractionModeEnum {
} (
  yarp.name = "yarp::dev::InteractionModeEnum"
  yarp.includefile = "yarp/dev/IInteractionMode.h"
  yarp.enumbase = "int32_t"
)

enum ySelectableControlModeEnum {
} (
  yarp.name = "yarp::dev::SelectableControlModeEnum"
  yarp.includefile = "yarp/dev/IControlMode.h"
  yarp.enumbase = "int32_t"
)

//-------------------------------------------------
// IJointBrake
struct return_isJointBraked {
  1: yReturnValue ret;
  2: bool isBraked;
}

struct return_getAutoBrakeEnabled {
  1: yReturnValue ret;
  2: bool enabled;
}

//-------------------------------------------------
// IVelocityDirect
struct return_getRefVelocityOne {
  1: yReturnValue ret;
  2: double vel;
}

struct return_getRefVelocityAll {
  1: yReturnValue ret;
  2: list<double> vel;
}

struct return_getRefVelocityGroup {
  1: yReturnValue ret;
  2: list<double> vel;
}

//-------------------------------------------------
struct return_getAxes {
  1: yReturnValue ret;
  2: i32 axes;
}

//-------------------------------------------------
// IControlModes
struct return_getAvailableControlModes {
  1: yReturnValue ret;
  2: list<ySelectableControlModeEnum> avail;
}

//-------------------------------------------------
// IPidControl
struct return_getAvailablePids {
  1: yReturnValue ret;
  2: list<yPidControlTypeEnum> avail;
}

struct return_getPid {
  1: yReturnValue ret;
  2: yPid pid;
}

struct return_getPids {
  1: yReturnValue ret;
  2: list<yPid> pids;
}

struct return_getPidExtraInfo {
  1: yReturnValue ret;
  2: yInfoPid info;
}

struct return_getPidExtraInfos {
  1: yReturnValue ret;
  2: list<yInfoPid> info;
}

struct return_getPidOffset {
  1: yReturnValue ret;
  2: double offset;
}

struct return_getPidFeedforward {
  1: yReturnValue ret;
  2: double feedforward;
}

struct return_isPidEnabled {
  1: yReturnValue ret;
  2: bool isEnabled;
}

struct return_getPidError {
  1: yReturnValue ret;
  2: double err;
}

struct return_getPidErrors {
  1: yReturnValue ret;
  2: list<double> errs;
}

struct return_getPidReference {
  1: yReturnValue ret;
  2: double ref;
}

struct return_getPidReferences {
  1: yReturnValue ret;
  2: list<double> refs;
}

struct return_getPidErrorLimit {
  1: yReturnValue ret;
  2: double lim;
}

struct return_getPidErrorLimits {
  1: yReturnValue ret;
  2: list<double> lims;
}

struct return_getPidOutput {
  1: yReturnValue ret;
  2: double out;
}

struct return_getPidOutputs {
  1: yReturnValue ret;
  2: list<double> outs;
}

//-------------------------------------------------
// ILimits

struct return_getPosLimits {
  1: yReturnValue ret;
  2: double min;
  3: double max;
}

struct return_getVelLimits {
  1: yReturnValue ret;
  2: double min;
  3: double max;
}

//-------------------------------------------------
struct return_getImpedance
{
  1: yReturnValue ret;
  2: double stiffness;
  3: double damping;
}

struct return_getImpedanceOffset
{
  1: yReturnValue ret;
  2: double offset;
}

struct return_getCurrentImpedanceLimit
{
  1: yReturnValue ret;
  2: double min_stiffness;
  3: double max_stiffness;
  4: double min_damping;
  5: double max_damping;
}

//-------------------------------------------------
struct return_getLastJointFault
{
  1: yReturnValue ret;
  2: i32 fault;
  3: string message;
}

//-------------------------------------------------
struct return_getRemoteVariable
{
  1: yReturnValue ret;
  2: yBottle val;
}

struct return_getRemoteVariablesList
{
  1: yReturnValue ret;
  2: list<yBottle> listOfKeys;
}

//-------------------------------------------------
struct return_getInteractionModeOne
{
  1: yReturnValue ret;
  2: yInteractionModeEnum mode;
}

struct return_getInteractionModeGroup
{
  1: yReturnValue ret;
  2: list<yInteractionModeEnum> modes;
}

struct return_getInteractionModeAll
{
  1: yReturnValue ret;
  2: list<yInteractionModeEnum> modes;
}

//-------------------------------------------------

struct return_getRefCurrentOne
{
  1: yReturnValue ret;
  2: double ref;
}

struct return_getRefCurrentAll
{
  1: yReturnValue ret;
  2: list<double> refs;
}

struct return_getRefCurrentGroup {
  1: yReturnValue ret;
  2: list<double> vel;
}

struct return_getCurrentOne
{
  1: yReturnValue ret;
  2: double curr;
}

struct return_getCurrentAll
{
  1: yReturnValue ret;
  2: list<double> currs
}

struct return_getCurrentRangeOne
{
  1: yReturnValue ret;
  2: double min;
  3: double max;
}

struct return_getCurrentRangeAll
{
  1: yReturnValue ret;
  2: list<double> mins;
  3: list<double> maxs;
}
//-------------------------------------------------

struct return_getEncoderOne
{
  1: yReturnValue ret;
  2: double val;
}

struct return_getEncoderTimedOne
{
  1: yReturnValue ret;
  2: double val;
  3: double time;
}

struct return_getEncoderAll
{
  1: yReturnValue ret;
  2: list<double> val;
}

struct return_getEncoderTimedAll
{
  1: yReturnValue ret;
  2: list<double> val;
  3: list<double> time;
}

struct return_getEncoderSpeedOne
{
  1: yReturnValue ret;
  2: double val;
}

struct return_getEncoderSpeedAll
{
  1: yReturnValue ret;
  2: list<double> val;
}

struct return_getEncoderAccelerationOne
{
  1: yReturnValue ret;
  2: double val;
}

struct return_getEncoderAccelerationAll
{
  1: yReturnValue ret;
  2: list<double> val;
}

//-------------------------------------------------

struct return_getRefDutyCycleOne
{
  1: yReturnValue ret;
  2: double val;
}

struct return_getRefDutyCycleAll
{
  1: yReturnValue ret;
  2: list<double> val;
}

struct return_getDutyCycleOne
{
  1: yReturnValue ret;
  2: double val;
}

struct return_getDutyCycleAll
{
  1: yReturnValue ret;
  2: list<double> val;
}

//-------------------------------------------------

struct return_getMotorEncoderOne
{
  1: yReturnValue ret;
  2: double val;
}

struct return_getMotorEncoderTimedOne
{
  1: yReturnValue ret;
  2: double val;
  3: double time;
}

struct return_getMotorEncoderAll
{
  1: yReturnValue ret;
  2: list<double> val;
}

struct return_getMotorEncoderTimedAll
{
  1: yReturnValue ret;
  2: list<double> val;
  3: list<double> time;
}

struct return_getMotorEncoderSpeedOne
{
  1: yReturnValue ret;
  2: double val;
}

struct return_getMotorEncoderSpeedAll
{
  1: yReturnValue ret;
  2: list<double> val;
}

struct return_getMotorEncoderAccelerationOne
{
  1: yReturnValue ret;
  2: double val;
}

struct return_getMotorEncoderAccelerationAll
{
  1: yReturnValue ret;
  2: list<double> val;
}

struct return_getNumberOfMotorEncoders
{
  1: yReturnValue ret;
  2: i32 num;
}

struct return_getMotorEncoderCountsPerRevolution
{
  1: yReturnValue ret;
  2: double cpr;
}

//-------------------------------------------------

struct return_isCalibratorDevicePresent
{
  1: yReturnValue ret;
  2: bool isPresent;
}

//-------------------------------------------------

struct return_getMotorTorqueParams
{
  1: yReturnValue ret;
  2: yMotorTorqueParameters params;
}

//-------------------------------------------------

struct return_getAxisName
{
  1: yReturnValue ret;
  2: string name;
}

struct return_getJointType
{
  1: yReturnValue ret;
  2: yJointTypeEnum joint;
}

//-------------------------------------------------

struct return_getNumberOfMotors
{
  1: yReturnValue ret;
  2: i32 val;
}

struct return_getTemperatureOne
{
  1: yReturnValue ret;
  2: double val;
}

struct return_getTemperatureAll
{
  1: yReturnValue ret;
  2: list<double> val;
}

struct return_getTemperatureLimit
{
  1: yReturnValue ret;
  2: double val;
}

struct return_getGearboxRatio
{
  1: yReturnValue ret;
  2: double val;
}

//-------------------------------------------------

struct return_getAmpStatusAll
{
  1: yReturnValue ret;
  2: list<i32> val;
}
struct return_getAmpStatusOne
{
  1: yReturnValue ret;
  2: i32 val;
}
struct return_getMaxCurrent
{
  1: yReturnValue ret;
  2: double val;
}
struct return_getNominalCurrent
{
  1: yReturnValue ret;
  2: double val;
}
struct return_getPeakCurrent
{
  1: yReturnValue ret;
  2: double val;
}
struct return_getPWM
{
  1: yReturnValue ret;
  2: double val;
}
struct return_getPWMLimit
{
  1: yReturnValue ret;
  2: double val;
}
struct return_getPowerSupplyVoltage
{
  1: yReturnValue ret;
  2: double val;
}

struct return_getTargetPositionOne
{
  1: yReturnValue ret;
  2: double val;
}

struct return_getTargetPositionGroup
{
  1: yReturnValue ret;
  2: list<double> val;
}

struct return_getTargetPositionAll
{
  1: yReturnValue ret;
  2: list<double> val;
}

struct return_getTrajSpeedOne
{
  1: yReturnValue ret;
  2: double val;
}

struct return_getTrajSpeedsGroup
{
  1: yReturnValue ret;
  2: list<double> val;
}

struct return_getTrajSpeedsAll
{
  1: yReturnValue ret;
  2: list<double> val;
}

struct return_getTrajAccelerationOne
{
  1: yReturnValue ret;
  2: double val;
}

struct return_getTrajAccelerationGroup
{
  1: yReturnValue ret;
  2: list<double> val;
}

struct return_getTrajAccelerationAll
{
  1: yReturnValue ret;
  2: list<double> val;
}

struct return_checkMotionDoneOne
{
  1: yReturnValue ret;
  2: bool flag;
}

struct return_checkMotionDoneGroup
{
  1: yReturnValue ret;
  2: bool flag;
}

struct return_checkMotionDoneAll
{
  1: yReturnValue ret;
  2: bool flag;
}


//-------------------------------------------------

struct return_getTargetVelocityOne
{
  1: yReturnValue ret;
  2: double val;
}

struct return_getTargetVelocityGroup
{
  1: yReturnValue ret;
  2: list<double> val;
}

struct return_getTargetVelocityAll
{
  1: yReturnValue ret;
  2: list<double> val;
}

//-------------------------------------------------

struct return_getRefPositionOne
{
  1: yReturnValue ret;
  2: double val;
}

struct return_getRefPositionGroup
{
  1: yReturnValue ret;
  2: list<double> val;
}

struct return_getRefPositionAll
{
  1: yReturnValue ret;
  2: list<double> val;
}

//-------------------------------------------------

struct return_getRefTorqueOne
{
  1: yReturnValue ret;
  2: double val;
}

struct return_getRefTorqueGroup
{
  1: yReturnValue ret;
  2: list<double> val;
}

struct return_getRefTorqueAll
{
  1: yReturnValue ret;
  2: list<double> val;
}

struct return_getTorqueRangeOne
{
  1: yReturnValue ret;
  2: double min;
  3: double max;
}

struct return_getTorqueRangeAll
{
  1: yReturnValue ret;
  2: list<double> mins;
  3: list<double> maxs;
}

struct return_getDeviceInterfaces
{
  1: yReturnValue ret;
  2: map<string, bool> interfaces;
}

//-------------------------------------------------
service ControlBoardMsgs
{
    return_getDeviceInterfaces         getDeviceInterfacesRPC()(yarp.qualifier = "const");

    return_getAvailableControlModes    getAvailableControlModesRPC(1: i16 j) (yarp.qualifier = "const");
    return_getImpedance                getImpedanceRPC(1:i32 j) (yarp.qualifier = "const");
    yReturnValue                       setImpedanceRPC(1:i32 j, 2:double stiffness, 3:double damping);
    yReturnValue                       setImpedanceOffsetRPC(1:i32 j, 2:double offset);
    return_getImpedanceOffset          getImpedanceOffsetRPC(1: i32 j) (yarp.qualifier = "const");
    return_getCurrentImpedanceLimit    getCurrentImpedanceLimitRPC(1: i32 j) (yarp.qualifier = "const");

    return_getLastJointFault           getLastJointFaultRPC(1: i32 j) (yarp.qualifier = "const");

    return_getRemoteVariable           getRemoteVariableRPC(1:string key) (yarp.qualifier = "const");
    yReturnValue                       setRemoteVariableRPC(1:string key, 2: yBottle val);
    return_getRemoteVariablesList      getRemoteVariablesListRPC() (yarp.qualifier = "const");

    return_getInteractionModeOne       getInteractionModeOneRPC(1: i32 axis) (yarp.qualifier = "const");
    return_getInteractionModeGroup     getInteractionModesGroupRPC(1: list<i32> joints) (yarp.qualifier = "const");
    return_getInteractionModeAll       getInteractionModesAllRPC() (yarp.qualifier = "const");
    yReturnValue                       setInteractionModeOneRPC(1: i32 axis, 2: yInteractionModeEnum mode);
    yReturnValue                       setInteractionModesGroupRPC(1: list<i32> joints, 2: list<yInteractionModeEnum> modes);
    yReturnValue                       setInteractionModesAllRPC(1: list<yInteractionModeEnum> modes);

    return_getRefCurrentAll            getRefCurrentAllRPC() (yarp.qualifier = "const");
    return_getRefCurrentOne            getRefCurrentOneRPC(1: i32 j) (yarp.qualifier = "const");
    yReturnValue                       setRefCurrentAllRPC(1: list<double> refs);
    yReturnValue                       setRefCurrentOneRPC(1: i32 j, 2: double ref);
    yReturnValue                       setRefCurrentGroupRPC(1: list<i32> j, 2: list<double> refs);
    return_getCurrentAll               getCurrentAllRPC() (yarp.qualifier = "const");
    return_getCurrentOne               getCurrentOneRPC(1: i32 j) (yarp.qualifier = "const");
    return_getCurrentRangeOne          getCurrentRangeOneRPC(1: i32 j) (yarp.qualifier = "const");
    return_getCurrentRangeAll          getCurrentRangeAllRPC() (yarp.qualifier = "const");

    yReturnValue                       setRefDutyCycleOneRPC(1: i32 j, 2: double v);
    yReturnValue                       setRefDutyCycleAllRPC(1: list<double> v);
    yReturnValue                       setRefDutyCycleGroupRPC(1: list<i32> j, 2: list<double> v);
    return_getRefDutyCycleOne          getRefDutyCycleOneRPC(1: i32 j) (yarp.qualifier = "const");
    return_getRefDutyCycleAll          getRefDutyCycleAllRPC() (yarp.qualifier = "const");
    return_getDutyCycleOne             getDutyCycleOneRPC(1: i32 j) (yarp.qualifier = "const");
    return_getDutyCycleAll             getDutyCycleAllRPC() (yarp.qualifier = "const");

    yReturnValue                     resetEncoderOneRPC(1: i32 j);
    yReturnValue                     resetEncoderAllRPC();
    yReturnValue                     setEncoderOneRPC(1: i32 j, 2: i32 val);
    yReturnValue                     setEncoderAllRPC(1: list<double> vals);
    return_getEncoderOne             getEncoderOneRPC(1: i32 j) (yarp.qualifier = "const");
    return_getEncoderTimedOne        getEncoderTimedOneRPC(1: i32 j) (yarp.qualifier = "const");
    return_getEncoderAll             getEncoderAllRPC() (yarp.qualifier = "const");
    return_getEncoderTimedAll        getEncoderTimedAllRPC() (yarp.qualifier = "const");
    return_getEncoderSpeedOne        getEncoderSpeedOneRPC(1: i32 j) (yarp.qualifier = "const");
    return_getEncoderSpeedAll        getEncoderSpeedAllRPC() (yarp.qualifier = "const");
    return_getEncoderAccelerationOne getEncoderAccelerationOneRPC(1: i32 j) (yarp.qualifier = "const");
    return_getEncoderAccelerationAll getEncoderAccelerationAllRPC() (yarp.qualifier = "const");

    yReturnValue                           resetMotorEncoderRPC(1: i32 j);
    yReturnValue                           resetMotorEncodersRPC();
    yReturnValue                           setMotorEncoderRPC(1: i32 j, 2: double val);
    yReturnValue                           setMotorEncoderCountsPerRevolutionRPC(1: i32 j, 2: double val);
    return_getMotorEncoderCountsPerRevolution getMotorEncoderCountsPerRevolutionRPC(1: i32 m) (yarp.qualifier = "const");
    yReturnValue                           setMotorEncodersAllRPC(1: list<double> vals);
    return_getMotorEncoderOne              getMotorEncoderOneRPC(1: i32 j) (yarp.qualifier = "const");
    return_getMotorEncoderAll              getMotorEncoderAllRPC() (yarp.qualifier = "const");
    return_getMotorEncoderTimedOne         getMotorEncoderTimedOneRPC(1: i32 j) (yarp.qualifier = "const");
    return_getMotorEncoderTimedAll         getMotorEncodersTimedAllRPC() (yarp.qualifier = "const");
    return_getMotorEncoderSpeedOne         getMotorEncoderSpeedOneRPC(1: i32 j) (yarp.qualifier = "const");
    return_getMotorEncoderSpeedAll         getMotorEncoderSpeedAllRPC() (yarp.qualifier = "const");
    return_getMotorEncoderAccelerationOne  getMotorEncoderAccelerationOneRPC(1: i32 j) (yarp.qualifier = "const");
    return_getMotorEncoderAccelerationAll  getMotorEncoderAccelerationAllRPC() (yarp.qualifier = "const");
    return_getNumberOfMotorEncoders        getNumberOfMotorEncodersRPC() (yarp.qualifier = "const");

    return_isCalibratorDevicePresent     isCalibratorDevicePresentRPC() (yarp.qualifier = "const");
    yReturnValue                         calibrateSingleJointRPC(1: i32 j);
    yReturnValue                         calibrateWholePartRPC();
    yReturnValue                         homingSingleJointRPC(1: i32 j);
    yReturnValue                         homingWholePartRPC();
    yReturnValue                         parkSingleJointRPC(1: i32 j, 2: bool wait);
    yReturnValue                         parkWholePartRPC();
    yReturnValue                         quitCalibrateRPC();
    yReturnValue                         quitParkRPC();

    yReturnValue               setManualBrakeActiveRPC(1: i32 j, 2:bool active);
    yReturnValue               setAutoBrakeEnabledRPC(1: i32 j, 2:bool enabled);
    return_isJointBraked       isJointBrakedRPC(1:i32 j) (yarp.qualifier = "const");
    return_getAutoBrakeEnabled getAutoBrakeEnabledRPC(1: i32 j) (yarp.qualifier = "const");

    return_getAxes              getAxesRPC() (yarp.qualifier = "const");

    return_getRefTorqueOne    getRefTorqueOneRPC(1:i32 j) (yarp.qualifier = "const");
    return_getRefTorqueAll    getRefTorqueAllRPC() (yarp.qualifier = "const");
    return_getRefTorqueGroup  getRefTorqueGroupRPC(1: list<i32> j) (yarp.qualifier = "const");
    yReturnValue              setRefTorqueOneRPC(1:i32 j, 2: double val);
    yReturnValue              setRefTorqueAllRPC(1: list<double> val);
    yReturnValue              setRefTorqueGroupRPC(1: list<i32> j, 2: list<double> val);

    yReturnValue                setRefPositionOneRPC(1:i32 j, 2: double value);
    yReturnValue                setRefPositionAllRPC(1: list<double> value);
    yReturnValue                setRefPositionGroupRPC(1: list<i32> j, 2: list<double> value);
    return_getRefPositionOne    getRefPositionOneRPC(1:i32 j) (yarp.qualifier = "const");
    return_getRefPositionAll    getRefPositionAllRPC() (yarp.qualifier = "const");
    return_getRefPositionGroup  getRefPositionGroupRPC(1: list<i32> j) (yarp.qualifier = "const");

    yReturnValue               setControlModeOneRPC(1:i32 j, 2:ySelectableControlModeEnum mod);
    yReturnValue               setControlModeAllRPC(1:list<ySelectableControlModeEnum> modes);
    yReturnValue               setControlModeGroupRPC(1: list<i32> j, 2: list<ySelectableControlModeEnum> modes);

    return_getAxisName         getAxisNameRPC(1: i32 j);
    return_getJointType        getJointTypeRPC(1: i32 j);

    yReturnValue               calibrateRobotRPC();
    yReturnValue               abortCalibrationRPC();
    yReturnValue               abortParkRPC();
    yReturnValue               parkRPC(1:bool wait);
    yReturnValue               calibrateAxisWithParamsRPC(1: i32 j, 2: i32 ui, 3: double v1, 4: double v2, 5: double v3);
    yReturnValue               setCalibrationParametersRPC(1: i32 j, 2: yCalibrationParameters params);
    yReturnValue               calibrationDoneRPC(1: i32 j);

    yReturnValue               setPosLimitsRPC(1: i16 j, 2: double min, 3: double max);
    yReturnValue               setVelLimitsRPC(1: i16 j, 2: double min, 3: double max);
    return_getPosLimits        getPosLimitsRPC(1: i16 j) (yarp.qualifier = "const");
    return_getVelLimits        getVelLimitsRPC(2: i16 j) (yarp.qualifier = "const");

    return_getNumberOfMotors   getNumberOfMotorsRPC() (yarp.qualifier = "const");
    return_getTemperatureOne   getTemperatureOneRPC(1: i32 m) (yarp.qualifier = "const");
    return_getTemperatureAll   getTemperatureAllRPC() (yarp.qualifier = "const");
    return_getTemperatureLimit getTemperatureLimitRPC (1: i32 m) (yarp.qualifier = "const");
    yReturnValue               setTemperatureLimitRPC (1: i32 m, 2: double val);
    return_getGearboxRatio     getGearboxRatioRPC(1: i32 m) (yarp.qualifier = "const");
    yReturnValue               setGearboxRatioRPC(1: i32 m, 2: double val);

    yReturnValue               enablePidRPC(1: yPidControlTypeEnum pidtype, 2: i16 j);
    yReturnValue               disablePidRPC(1: yPidControlTypeEnum pidtype, 2: i16 j);
    yReturnValue               resetPidRPC(1: yPidControlTypeEnum pidtype, 2: i16 j);
    yReturnValue               setPidRPC(1: yPidControlTypeEnum pidtype, 2: i16 j, 3: yPid pid);
    yReturnValue               setPidsRPC(1: yPidControlTypeEnum pidtype, 2: list<yPid> pids);
    yReturnValue               setPidOffsetOneRPC(1: yPidControlTypeEnum pidtype, 2: i16 j, 3: double off);
    yReturnValue               setPidFeedforwardOneRPC(1: yPidControlTypeEnum pidtype, 2: i16 j, 3: double off);
    yReturnValue               setPidReferenceRPC(1: yPidControlTypeEnum pidtype, 2: i16 j, 3: double ref);
    yReturnValue               setPidReferencesRPC(1: yPidControlTypeEnum pidtype, 2: list<double> refs);
    yReturnValue               setPidErrorLimitRPC(1: yPidControlTypeEnum pidtype, 2: i16 j, 3: double limit);
    yReturnValue               setPidErrorLimitsRPC(1: yPidControlTypeEnum pidtype, 2: list<double> limits);
    return_getAvailablePids    getAvailablePidsRPC(1: i16 j) (yarp.qualifier = "const");
    return_isPidEnabled        isPidEnabledRPC(1: yPidControlTypeEnum pidtype, 2: i16 j) (yarp.qualifier = "const");
    return_getPidError         getPidErrorRPC(1: yPidControlTypeEnum pidtype, 2: i16 j) (yarp.qualifier = "const");
    return_getPidErrors        getPidErrorsRPC(1: yPidControlTypeEnum pidtype) (yarp.qualifier = "const");
    return_getPidReference     getPidReferenceRPC(1: yPidControlTypeEnum pidtype, 2: i16 j) (yarp.qualifier = "const");
    return_getPidReferences    getPidReferencesRPC(1: yPidControlTypeEnum pidtype) (yarp.qualifier = "const");
    return_getPidErrorLimit    getPidErrorLimitRPC(1: yPidControlTypeEnum pidtype, 2: i16 j) (yarp.qualifier = "const");
    return_getPidErrorLimits   getPidErrorLimitsRPC(1: yPidControlTypeEnum pidtype) (yarp.qualifier = "const");
    return_getPidOutput        getPidOutputRPC(1: yPidControlTypeEnum pidtype, 2: i16 j) (yarp.qualifier = "const");
    return_getPidOutputs       getPidOutputsRPC(1: yPidControlTypeEnum pidtype) (yarp.qualifier = "const");
    return_getPid              getPidRPC(1: yPidControlTypeEnum pidtype, 2: i16 j) (yarp.qualifier = "const");
    return_getPids             getPidsRPC(1: yPidControlTypeEnum pidtype) (yarp.qualifier = "const");
    return_getPidExtraInfo     getPidExtraInfoRPC(1: yPidControlTypeEnum pidtype, 2: i16 j) (yarp.qualifier = "const");
    return_getPidExtraInfos    getPidExtraInfosRPC(1: yPidControlTypeEnum pidtype) (yarp.qualifier = "const");
    return_getPidOffset        getPidOffsetRPC(1: yPidControlTypeEnum pidtype, 2: i16 j) (yarp.qualifier = "const");
    return_getPidFeedforward   getPidFeedforwardRPC(1: yPidControlTypeEnum pidtype, 2: i16 j) (yarp.qualifier = "const");

    yReturnValue                    positionMoveOneRPC(1: i32 j, 2: double ref);
    yReturnValue                    positionMoveGroupRPC(1: list<i32> joints, 2: list<double> refs);
    yReturnValue                    positionMoveAllRPC(1: list<double> refs);
    yReturnValue                    relativeMoveOneRPC(1: i32 j, 2: double delta);
    yReturnValue                    relativeMoveGroupRPC(1: list<i32> joints, 2: list<double> refs);
    yReturnValue                    relativeMoveAllRPC(1: list<double> deltas);
    yReturnValue                    setTrajSpeedOneRPC(1: i32 j, 2: double sp);
    yReturnValue                    setTrajSpeedGroupRPC(1: list<i32> joints, 2: list<double> spds);
    yReturnValue                    setTrajSpeedAllRPC(1: list<double> spds);
    yReturnValue                    setTrajAccelerationOneRPC(1: i32 j, 2: double acc);
    yReturnValue                    setTrajAccelerationsGroupRPC(1: list<i32> joints, 2: list<double> accs);
    yReturnValue                    setTrajAccelerationsAllRPC(1: list<double> accs);
    yReturnValue                    stopOneRPC(1: i32 j);
    yReturnValue                    stopGroupRPC(1: list<i32> joints);
    yReturnValue                    stopAllRPC();
    return_getTargetPositionOne     getTargetPositionOneRPC(1: i32 j)  (yarp.qualifier = "const");
    return_getTargetPositionGroup   getTargetPositionGroupRPC(1: list<i32> joints) (yarp.qualifier = "const");
    return_getTargetPositionAll     getTargetPositionAllRPC() (yarp.qualifier = "const");
    return_getTrajSpeedOne          getTrajSpeedOneRPC(1: i32 j)  (yarp.qualifier = "const");
    return_getTrajSpeedsGroup       getTrajSpeedsGroupRPC(1: list<i32> joints)  (yarp.qualifier = "const");
    return_getTrajSpeedsAll         getTrajSpeedsAllRPC()  (yarp.qualifier = "const");
    return_getTrajAccelerationOne   getTrajAccelerationOneRPC(1: i32 j)  (yarp.qualifier = "const");
    return_getTrajAccelerationGroup getTrajAccelerationGroupRPC(1: list<i32> joints)  (yarp.qualifier = "const");
    return_getTrajAccelerationAll   getTrajAccelerationAllRPC()  (yarp.qualifier = "const");
    return_checkMotionDoneOne       checkMotionDoneOneRPC(1: i32 j)  (yarp.qualifier = "const");
    return_checkMotionDoneGroup     checkMotionDoneGroupRPC(1: list<i32> joints) (yarp.qualifier = "const");
    return_checkMotionDoneAll       checkMotionDoneAllRPC()  (yarp.qualifier = "const");

    yReturnValue                    velocityMoveOneRPC(1: i32 j, 2: double ref);
    yReturnValue                    velocityMoveGroupRPC(1: list<i32> joints, 2: list<double> refs);
    yReturnValue                    velocityMoveAllRPC(1: list<double> refs);
    return_getTargetVelocityOne     getTargetVelocityOneRPC(1: i32 j)  (yarp.qualifier = "const");
    return_getTargetVelocityGroup   getTargetVelocityGroupRPC(1: list<i32> joints) (yarp.qualifier = "const");
    return_getTargetVelocityAll     getTargetVelocityAllRPC() (yarp.qualifier = "const");

    yReturnValue                    setRefVelocityOneRPC(1:i32 j, 2:double ref);
    yReturnValue                    setRefVelocityAllRPC(1:list<double> ref);
    yReturnValue                    setRefVelocityGroupRPC(1: list<i32> j, 2:list<double> ref);
    return_getRefVelocityOne    getRefVelocityOneRPC(1:i32 j) (yarp.qualifier = "const");
    return_getRefVelocityAll    getRefVelocityAllRPC() (yarp.qualifier = "const");
    return_getRefVelocityGroup  getRefVelocityGroupRPC(1: list<i32> j) (yarp.qualifier = "const");

    return_getTorqueRangeOne       getTorqueRangeOneRPC(1: i32 j)  (yarp.qualifier = "const");
    return_getTorqueRangeAll       getTorqueRangeAllRPC() (yarp.qualifier = "const");

    yReturnValue                  setMotorTorqueParamsRPC(1: i32 j, 2: yMotorTorqueParameters params);
    return_getMotorTorqueParams   getMotorTorqueParamsRPC(1: i32 j);

    yReturnValue                  enableAmpRPC(1: i32 j);
    yReturnValue                  disableAmpRPC(1: i32 j);
    return_getAmpStatusAll        getAmpStatusAllRPC() (yarp.qualifier = "const");
    return_getAmpStatusOne        getAmpStatusOneRPC(1: i32 j) (yarp.qualifier = "const");
    yReturnValue                  setMaxCurrentRPC(1: i32 j, 2: double v);
    return_getMaxCurrent          getMaxCurrentRPC(1: i32 j) (yarp.qualifier = "const");
    return_getNominalCurrent      getNominalCurrentRPC(1: i32 m) (yarp.qualifier = "const");
    yReturnValue                  setNominalCurrentRPC(1: i32 m, 2: double val);
    return_getPeakCurrent         getPeakCurrentRPC(1: i32 m) (yarp.qualifier = "const");
    yReturnValue                  setPeakCurrentRPC(1: i32 m, 2: double val);
    return_getPWM                 getPWMRPC(1: i32 m) (yarp.qualifier = "const");
    return_getPWMLimit            getPWMLimitRPC(1: i32 m) (yarp.qualifier = "const");
    yReturnValue                  setPWMLimitRPC(1: i32 m, 2: double val);
    return_getPowerSupplyVoltage  getPowerSupplyVoltageRPC(1: i32 m) (yarp.qualifier = "const");
}
