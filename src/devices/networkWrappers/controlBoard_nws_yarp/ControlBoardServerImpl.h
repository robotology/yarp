/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_CONTROLBOARDSERVERIMPL_H
#define YARP_DEV_CONTROLBOARDSERVERIMPL_H

#include <mutex>

#include "ControlBoardMsgs.h"
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/IPreciselyTimed.h>

struct AllInterfacesStruct
{
    yarp::dev::IControlMode*        iControlMode = nullptr;
    yarp::dev::IJointBrake*         iJointBrake = nullptr;
    yarp::dev::IVelocityDirect*     iVelocityDirect = nullptr;
    yarp::dev::IPidControl*         iPidControl = nullptr;
    yarp::dev::IJointFault*         iJointFault = nullptr;
    yarp::dev::IPositionControl*    iPositionControl {nullptr};
    yarp::dev::IPositionDirect*     iPositionDirect {nullptr};
    yarp::dev::IVelocityControl*    iVelocityControl {nullptr};
    yarp::dev::IEncodersTimed*      iEncodersTimed {nullptr};
    yarp::dev::IMotorEncoders*      iMotorEncoders {nullptr};
    yarp::dev::IAmplifierControl*   iAmplifierControl {nullptr};
    yarp::dev::IControlLimits*      iControlLimits {nullptr};
    yarp::dev::ITorqueControl*      iTorqueControl {nullptr};
    yarp::dev::IAxisInfo*           iAxisInfo {nullptr};
    yarp::dev::IRemoteCalibrator*   iRemoteCalibrator {nullptr};
    yarp::dev::IControlCalibration* iControlCalibration {nullptr};
    yarp::dev::IImpedanceControl*   iImpedanceControl {nullptr};
    yarp::dev::IInteractionMode*    iInteractionMode {nullptr};
    yarp::dev::IMotor*              iMotor {nullptr};
    yarp::dev::IRemoteVariables*    iRemoteVariables {nullptr};
    yarp::dev::ICurrentControl*     iCurrentControl {nullptr};
    yarp::dev::IPWMControl*         iPWMControl {nullptr};
    yarp::dev::IPreciselyTimed*     iPreciselyTimed {nullptr};
};

class ControlBoardRPCd : public ControlBoardMsgs
{
    private:
    mutable std::mutex              m_mutex;
    size_t                          m_njoints = 0;
    AllInterfacesStruct             m_allInterfaces;

    public:
    ControlBoardRPCd(size_t njoints,
                     AllInterfacesStruct interfaces)
    {
        m_njoints = njoints;
        m_allInterfaces = interfaces;
    }

    //get CapabilitiesS
    return_getDeviceInterfaces  getDeviceInterfacesRPC() const override;

    //IControlMode
    return_getAvailableControlModes getAvailableControlModesRPC(const std::int16_t j)  const override;
    yarp::dev::ReturnValue setControlModeOneRPC(const std::int32_t j, const yarp::dev::SelectableControlModeEnum mod) override;
    yarp::dev::ReturnValue setControlModeAllRPC(const std::vector<yarp::dev::SelectableControlModeEnum>& modes) override;
    yarp::dev::ReturnValue setControlModeGroupRPC(const std::vector<std::int32_t>& j, const std::vector<yarp::dev::SelectableControlModeEnum>& modes) override;

    //IJointBrake
    return_isJointBraked isJointBrakedRPC(const std::int32_t j) const override;
    yarp::dev::ReturnValue setManualBrakeActiveRPC(const std::int32_t j, const bool active) override;
    yarp::dev::ReturnValue setAutoBrakeEnabledRPC(const std::int32_t j, const bool enabled) override;
    return_getAutoBrakeEnabled getAutoBrakeEnabledRPC(const std::int32_t j) const override;

    // IVelocityDirect
    return_getAxes               getAxesRPC() const override;
    return_getRefVelocityOne getRefVelocityOneRPC(const std::int32_t j) const override;
    return_getRefVelocityAll getRefVelocityAllRPC() const override;
    return_getRefVelocityGroup getRefVelocityGroupRPC(const std::vector<std::int32_t>& jnts) const override;
    yarp::dev::ReturnValue setRefVelocityOneRPC(const std::int32_t j, const double vel) override;
    yarp::dev::ReturnValue setRefVelocityAllRPC(const std::vector<double>& vel) override;
    yarp::dev::ReturnValue setRefVelocityGroupRPC(const std::vector<std::int32_t>& j, const std::vector<double>& vel) override;

    // IPidControl
    return_getAvailablePids getAvailablePidsRPC(const std::int16_t j)  const override;
    yarp::dev::ReturnValue enablePidRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) override;
    yarp::dev::ReturnValue disablePidRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) override;
    yarp::dev::ReturnValue resetPidRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) override;
    return_isPidEnabled isPidEnabledRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j)  const override;
    yarp::dev::ReturnValue setPidRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j, const yarp::dev::Pid& pid) override;
    yarp::dev::ReturnValue setPidsRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::vector<yarp::dev::Pid>& pids) override;
    return_getPid getPidRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j)  const  override;
    return_getPids getPidsRPC(const yarp::dev::PidControlTypeEnum pidtype)  const override;
    return_getPidExtraInfo getPidExtraInfoRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) const  override;
    return_getPidExtraInfos getPidExtraInfosRPC(const yarp::dev::PidControlTypeEnum pidtypeo) const  override;
    return_getPidOffset getPidOffsetRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) const  override;
    return_getPidFeedforward getPidFeedforwardRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) const override;
    yarp::dev::ReturnValue setPidOffsetOneRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j, const double value) override;
    yarp::dev::ReturnValue setPidFeedforwardOneRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j, const double value) override;
    yarp::dev::ReturnValue setPidReferenceRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j, const double ref) override;
    yarp::dev::ReturnValue setPidReferencesRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::vector<double>& refs)override;
    yarp::dev::ReturnValue setPidErrorLimitRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j, const double limit)override;
    yarp::dev::ReturnValue setPidErrorLimitsRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::vector<double>& limits)override;
    return_getPidError getPidErrorRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) const override;
    return_getPidErrors getPidErrorsRPC(const yarp::dev::PidControlTypeEnum pidtype)const override;
    return_getPidReference getPidReferenceRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) const override;
    return_getPidReferences getPidReferencesRPC(const yarp::dev::PidControlTypeEnum pidtype) const override;
    return_getPidErrorLimit getPidErrorLimitRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) const override;
    return_getPidErrorLimits getPidErrorLimitsRPC(const yarp::dev::PidControlTypeEnum pidtype) const override;
    return_getPidOutput getPidOutputRPC(const yarp::dev::PidControlTypeEnum pidtype, const std::int16_t j) const override;
    return_getPidOutputs getPidOutputsRPC(const yarp::dev::PidControlTypeEnum pidtype)const override;

    // ILimits
    yarp::dev::ReturnValue setPosLimitsRPC(const std::int16_t j, const double min, const double max) override;
    yarp::dev::ReturnValue setVelLimitsRPC(const std::int16_t j, const double min, const double max) override;
    return_getPosLimits getPosLimitsRPC(const std::int16_t j) const override;
    return_getVelLimits getVelLimitsRPC(const std::int16_t j) const override;

    //IImpedance
    return_getImpedance getImpedanceRPC(const std::int32_t j) const override;
    yarp::dev::ReturnValue setImpedanceRPC(const std::int32_t j, const double stiffness, const double damping) override;
    yarp::dev::ReturnValue setImpedanceOffsetRPC(const std::int32_t j, const double offset) override;
    return_getImpedanceOffset getImpedanceOffsetRPC(const std::int32_t j) const override;
    return_getCurrentImpedanceLimit getCurrentImpedanceLimitRPC(const std::int32_t j) const override;

    //IJointFault
    return_getLastJointFault getLastJointFaultRPC(const std::int32_t j) const override;

    //IRemoteVariable
    return_getRemoteVariable getRemoteVariableRPC(const std::string& key) const override;
    yarp::dev::ReturnValue setRemoteVariableRPC(const std::string& key, const yarp::os::Bottle& val) override;
    return_getRemoteVariablesList getRemoteVariablesListRPC() const override;

    //IInteractionMode
    return_getInteractionModeOne getInteractionModeOneRPC(const std::int32_t axis) const override;
    return_getInteractionModeGroup getInteractionModesGroupRPC(const std::vector<std::int32_t>& joints) const override;
    return_getInteractionModeAll getInteractionModesAllRPC() const override;
    yarp::dev::ReturnValue setInteractionModeOneRPC(const std::int32_t axis, const yarp::dev::InteractionModeEnum mode) override;
    yarp::dev::ReturnValue setInteractionModesGroupRPC(const std::vector<std::int32_t>& joints, const std::vector<yarp::dev::InteractionModeEnum>& modes) override;
    yarp::dev::ReturnValue setInteractionModesAllRPC(const std::vector<yarp::dev::InteractionModeEnum>& modes) override;

    //ICurrent
    return_getRefCurrentAll getRefCurrentAllRPC() const override;
    return_getRefCurrentOne getRefCurrentOneRPC(const std::int32_t j) const override;
    yarp::dev::ReturnValue setRefCurrentAllRPC(const std::vector<double>& refs) override;
    yarp::dev::ReturnValue setRefCurrentOneRPC(const std::int32_t j, const double ref) override;
    yarp::dev::ReturnValue setRefCurrentGroupRPC(const std::vector<std::int32_t>& j, const std::vector<double>& refs) override;
    return_getCurrentAll getCurrentAllRPC() const override;
    return_getCurrentOne getCurrentOneRPC(const std::int32_t j) const override;
    return_getCurrentRangeOne getCurrentRangeOneRPC(const std::int32_t j) const override;
    return_getCurrentRangeAll getCurrentRangeAllRPC() const override;

    //IPWMControl
    yarp::dev::ReturnValue setRefDutyCycleOneRPC(const std::int32_t j, const double v) override;
    yarp::dev::ReturnValue setRefDutyCycleAllRPC(const std::vector<double>& v) override;
    return_getRefDutyCycleOne getRefDutyCycleOneRPC(const std::int32_t j) const override;
    return_getRefDutyCycleAll getRefDutyCycleAllRPC() const override;
    return_getDutyCycleOne getDutyCycleOneRPC(const std::int32_t j) const override;
    return_getDutyCycleAll getDutyCycleAllRPC() const override;

    //IEncoders
    yarp::dev::ReturnValue resetEncoderOneRPC(const std::int32_t j) override;
    yarp::dev::ReturnValue resetEncoderAllRPC() override;
    yarp::dev::ReturnValue setEncoderOneRPC(const std::int32_t j, const std::int32_t val) override;
    yarp::dev::ReturnValue setEncoderAllRPC(const std::vector<double>& vals) override;
    return_getEncoderOne getEncoderOneRPC(const std::int32_t j) const override;
    return_getEncoderTimedOne getEncoderTimedOneRPC(const std::int32_t j) const override;
    return_getEncoderAll getEncoderAllRPC() const override;
    return_getEncoderTimedAll getEncoderTimedAllRPC() const override;
    return_getEncoderSpeedOne getEncoderSpeedOneRPC(const std::int32_t j) const override;
    return_getEncoderSpeedAll getEncoderSpeedAllRPC() const override;
    return_getEncoderAccelerationOne getEncoderAccelerationOneRPC(const std::int32_t j) const override;
    return_getEncoderAccelerationAll getEncoderAccelerationAllRPC() const override;

    //IMotorEncoders
    yarp::dev::ReturnValue resetMotorEncoderRPC(const std::int32_t j) override;
    yarp::dev::ReturnValue resetMotorEncodersRPC() override;
    yarp::dev::ReturnValue setMotorEncoderRPC(const std::int32_t j, const double val) override;
    yarp::dev::ReturnValue setMotorEncoderCountsPerRevolutionRPC(const std::int32_t j, const double val) override;
    return_getMotorEncoderCountsPerRevolution getMotorEncoderCountsPerRevolutionRPC(const std::int32_t m) const override;
    yarp::dev::ReturnValue setMotorEncodersAllRPC(const std::vector<double>& vals) override;
    return_getMotorEncoderOne getMotorEncoderOneRPC(const std::int32_t j) const override;
    return_getMotorEncoderAll getMotorEncoderAllRPC() const override;
    return_getMotorEncoderTimedOne getMotorEncoderTimedOneRPC(const std::int32_t j) const override;
    return_getMotorEncoderTimedAll getMotorEncodersTimedAllRPC() const override;
    return_getMotorEncoderSpeedOne getMotorEncoderSpeedOneRPC(const std::int32_t j) const override;
    return_getMotorEncoderSpeedAll getMotorEncoderSpeedAllRPC() const override;
    return_getMotorEncoderAccelerationOne getMotorEncoderAccelerationOneRPC(const std::int32_t j) const override;
    return_getMotorEncoderAccelerationAll getMotorEncoderAccelerationAllRPC() const override;
    return_getNumberOfMotorEncoders getNumberOfMotorEncodersRPC() const override;

    //ICalibration
    return_isCalibratorDevicePresent isCalibratorDevicePresentRPC() const override;
    yarp::dev::ReturnValue calibrateSingleJointRPC(const std::int32_t j) override;
    yarp::dev::ReturnValue calibrateWholePartRPC() override;
    yarp::dev::ReturnValue homingSingleJointRPC(const std::int32_t j) override;
    yarp::dev::ReturnValue homingWholePartRPC() override;
    yarp::dev::ReturnValue parkSingleJointRPC(const std::int32_t j, const bool wait) override;
    yarp::dev::ReturnValue parkWholePartRPC() override;
    yarp::dev::ReturnValue quitCalibrateRPC() override;
    yarp::dev::ReturnValue quitParkRPC() override;

    // IPositionDirect
    yarp::dev::ReturnValue     setRefPositionOneRPC(const std::int32_t j, double ref) override;
    yarp::dev::ReturnValue     setRefPositionGroupRPC(const std::vector<std::int32_t>& joints, const std::vector<double>& refs) override;
    yarp::dev::ReturnValue     setRefPositionAllRPC(const std::vector<double>& refs) override;
    return_getRefPositionOne getRefPositionOneRPC(const std::int32_t j) const override;
    return_getRefPositionAll getRefPositionAllRPC() const override;
    return_getRefPositionGroup getRefPositionGroupRPC(const std::vector<std::int32_t>& j) const override;

    //ITorque
    yarp::dev::ReturnValue setRefTorqueOneRPC(const std::int32_t j, const double vel) override;
    yarp::dev::ReturnValue setRefTorqueAllRPC(const std::vector<double>& vel) override;
    yarp::dev::ReturnValue setRefTorqueGroupRPC(const std::vector<std::int32_t>& j, const std::vector<double>& vel) override;
    return_getRefTorqueOne getRefTorqueOneRPC(const std::int32_t j) const override;
    return_getRefTorqueAll getRefTorqueAllRPC() const override;
    return_getRefTorqueGroup getRefTorqueGroupRPC(const std::vector<std::int32_t>& j) const override;
    return_getTorqueRangeOne getTorqueRangeOneRPC(const std::int32_t j) const override;
    return_getTorqueRangeAll getTorqueRangeAllRPC() const override;

    //IAxis
    return_getAxisName getAxisNameRPC(const std::int32_t j) override;
    return_getJointType getJointTypeRPC(const std::int32_t j) override;

    //ICalibration
    yarp::dev::ReturnValue calibrateRobotRPC() override;
    yarp::dev::ReturnValue abortCalibrationRPC() override;
    yarp::dev::ReturnValue abortParkRPC() override;
    yarp::dev::ReturnValue parkRPC(const bool wait) override;
    yarp::dev::ReturnValue calibrateAxisWithParamsRPC(const std::int32_t j, const std::int32_t ui, const double v1, const double v2, const double v3) override;
    yarp::dev::ReturnValue setCalibrationParametersRPC(const std::int32_t j, const yCalibrationParameters& params) override;
    yarp::dev::ReturnValue calibrationDoneRPC(const std::int32_t j) override;

    //IMotor
    return_getNumberOfMotors getNumberOfMotorsRPC() const override;
    return_getTemperatureOne getTemperatureOneRPC(const std::int32_t m) const override;
    return_getTemperatureAll getTemperatureAllRPC() const override;
    return_getTemperatureLimit getTemperatureLimitRPC(const std::int32_t m) const override;
    yarp::dev::ReturnValue setTemperatureLimitRPC(const std::int32_t m, const double val) override;
    return_getGearboxRatio getGearboxRatioRPC(const std::int32_t m) const override;
    yarp::dev::ReturnValue setGearboxRatioRPC(const std::int32_t m, const double val) override;

    // IPositionControl
    yarp::dev::ReturnValue positionMoveOneRPC(const std::int32_t j, const double ref) override;
    yarp::dev::ReturnValue positionMoveGroupRPC(const std::vector<std::int32_t>& joints, const std::vector<double>& refs) override;
    yarp::dev::ReturnValue positionMoveAllRPC(const std::vector<double>& refs) override;
    yarp::dev::ReturnValue relativeMoveOneRPC(const std::int32_t j, const double delta) override;
    yarp::dev::ReturnValue relativeMoveGroupRPC(const std::vector<std::int32_t>& joints, const std::vector<double>& refs) override;
    yarp::dev::ReturnValue relativeMoveAllRPC(const std::vector<double>& deltas) override;
    yarp::dev::ReturnValue setTrajSpeedOneRPC(const std::int32_t j, const double sp) override;
    yarp::dev::ReturnValue setTrajSpeedGroupRPC(const std::vector<std::int32_t>& joints, const std::vector<double>& spds) override;
    yarp::dev::ReturnValue setTrajSpeedAllRPC(const std::vector<double>& spds) override;
    yarp::dev::ReturnValue setTrajAccelerationOneRPC(const std::int32_t j, const double acc) override;
    yarp::dev::ReturnValue setTrajAccelerationsGroupRPC(const std::vector<std::int32_t>& joints, const std::vector<double>& accs) override;
    yarp::dev::ReturnValue setTrajAccelerationsAllRPC(const std::vector<double>& accs) override;
    yarp::dev::ReturnValue stopOneRPC(const std::int32_t j) override;
    yarp::dev::ReturnValue stopGroupRPC(const std::vector<std::int32_t>& joints) override;
    yarp::dev::ReturnValue stopAllRPC() override;

    return_getTargetPositionOne getTargetPositionOneRPC(const std::int32_t j) const override;
    return_getTargetPositionGroup getTargetPositionGroupRPC(const std::vector<std::int32_t>& joints) const override;
    return_getTargetPositionAll getTargetPositionAllRPC() const override;
    return_getTrajSpeedOne getTrajSpeedOneRPC(const std::int32_t j) const override;
    return_getTrajSpeedsGroup getTrajSpeedsGroupRPC(const std::vector<std::int32_t>& joints) const override;
    return_getTrajSpeedsAll getTrajSpeedsAllRPC() const override;
    return_getTrajAccelerationOne getTrajAccelerationOneRPC(const std::int32_t j) const override;
    return_getTrajAccelerationGroup getTrajAccelerationGroupRPC(const std::vector<std::int32_t>& joints) const override;
    return_getTrajAccelerationAll getTrajAccelerationAllRPC() const override;

    return_checkMotionDoneOne checkMotionDoneOneRPC(const std::int32_t j) const override;
    return_checkMotionDoneGroup checkMotionDoneGroupRPC(const std::vector<std::int32_t>& joints) const override;
    return_checkMotionDoneAll checkMotionDoneAllRPC() const override;

    // IVelocityControl
    return_getTargetVelocityOne getTargetVelocityOneRPC(const std::int32_t j) const override;
    return_getTargetVelocityGroup getTargetVelocityGroupRPC(const std::vector<std::int32_t>& joints) const override;
    return_getTargetVelocityAll getTargetVelocityAllRPC() const override;
    yarp::dev::ReturnValue     velocityMoveOneRPC(const std::int32_t j, double ref) override;
    yarp::dev::ReturnValue     velocityMoveGroupRPC(const std::vector<std::int32_t>& joints, const std::vector<double>& refs) override;
    yarp::dev::ReturnValue     velocityMoveAllRPC(const std::vector<double>& refs) override;

    // ??
    yarp::dev::ReturnValue setMotorTorqueParamsRPC(const std::int32_t j, const yMotorTorqueParameters& params) override;
    return_getMotorTorqueParams getMotorTorqueParamsRPC(const std::int32_t j) override;

    //IAmplifierControl
    yarp::dev::ReturnValue enableAmpRPC(const std::int32_t j) override;
    yarp::dev::ReturnValue disableAmpRPC(const std::int32_t j) override;
    return_getAmpStatusAll getAmpStatusAllRPC() const override;
    return_getAmpStatusOne getAmpStatusOneRPC(const std::int32_t j) const override;
    yarp::dev::ReturnValue setMaxCurrentRPC(const std::int32_t j, const double v) override;
    return_getMaxCurrent getMaxCurrentRPC(const std::int32_t j) const override;
    return_getNominalCurrent getNominalCurrentRPC(const std::int32_t m) const override;
    yarp::dev::ReturnValue setNominalCurrentRPC(const std::int32_t m, const double val) override;
    return_getPeakCurrent getPeakCurrentRPC(const std::int32_t m) const override;
    yarp::dev::ReturnValue setPeakCurrentRPC(const std::int32_t m, const double val) override;
    return_getPWM getPWMRPC(const std::int32_t m) const override;
    return_getPWMLimit getPWMLimitRPC(const std::int32_t m) const override;
    yarp::dev::ReturnValue setPWMLimitRPC(const std::int32_t m, const double val) override;
    return_getPowerSupplyVoltage getPowerSupplyVoltageRPC(const std::int32_t m) const override;

    std::mutex* getMutex() {return &m_mutex;}
};

#endif // YARP_DEV_CONTROLBOARDSERVERIMPL_H
