/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEVICE_FAKE_MOTIONCONTROL
#define YARP_DEVICE_FAKE_MOTIONCONTROL

#include <yarp/os/Time.h>
#include <yarp/os/Bottle.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/IVirtualAnalogSensor.h>
#include <yarp/dev/ImplementVirtualAnalogSensor.h>
#include <yarp/dev/ImplementPositionControl.h>
#include <yarp/dev/ImplementVelocityControl.h>
#include <yarp/dev/ImplementJointFault.h>
#include <yarp/dev/ImplementJointBrake.h>

#include <mutex>
#include "FakeMotionControl_ParamsParser.h"

struct ImpedanceLimits
{
    double min_stiff;
    double max_stiff;
    double min_damp;
    double max_damp;
    double param_a;
    double param_b;
    double param_c;

public:
    ImpedanceLimits()
    {
        min_stiff=0;
        max_stiff=0;
        min_damp=0;
        max_damp=0;
        param_a=0;
        param_b=0;
        param_c=0;
    }

    double get_min_stiff()
    {
        return min_stiff;
    }
    double get_max_stiff()
    {
        return max_stiff;
    }
    double get_min_damp()
    {
        return min_damp;
    }
    double get_max_damp()
    {
        return max_damp;
    }
};

struct ImpedanceParameters
{
    double stiffness;
    double damping;
    ImpedanceLimits limits;
    ImpedanceParameters() {stiffness=0; damping=0;}
};

/**
 * @ingroup dev_impl_fake dev_impl_motor
 *
 * \brief `fakeMotionControl`: The aim of this device is to mimic the expected behavior of a
 * real motion control device to help testing the high level software.
 *
 * This device is implementing last version of interfaces and it is compatible
 * with controlBoard_nws_yarp device.
 *
 * Parameters required by this device are shown in class: FakeMotionControl_ParamsParser
 *
 * WIP - Some interfaces could be not implemented.
 */
class FakeMotionControl :
        public FakeMotionControl_ParamsParser,
        public yarp::dev::DeviceDriver,
        public yarp::os::PeriodicThread,
        public yarp::dev::IPidControlRaw,
        public yarp::dev::IControlCalibrationRaw,
        public yarp::dev::IAmplifierControlRaw,
        public yarp::dev::IEncodersTimedRaw,
        public yarp::dev::IMotorEncodersRaw,
        public yarp::dev::IMotorRaw,
        public yarp::dev::IPositionControlRaw,
        public yarp::dev::IVelocityControlRaw,
        public yarp::dev::IVelocityDirectRaw,
        public yarp::dev::IControlModeRaw,
        public yarp::dev::IControlLimitsRaw,
        public yarp::dev::IPositionDirectRaw,
        public yarp::dev::ITorqueControlRaw,
        public yarp::dev::ICurrentControlRaw,
        public yarp::dev::IPWMControlRaw,
        public yarp::dev::IImpedanceControlRaw,
        public yarp::dev::IInteractionModeRaw,
        public yarp::dev::IAxisInfoRaw,
        public yarp::dev::IVirtualAnalogSensorRaw, //*
        public yarp::dev::IJointBrakeRaw,
        public yarp::dev::IJointFaultRaw,
        public yarp::dev::ImplementControlCalibration,
        public yarp::dev::ImplementAmplifierControl,
        public yarp::dev::ImplementPidControl,
        public yarp::dev::ImplementEncodersTimed,
        public yarp::dev::ImplementPositionControl,
        public yarp::dev::ImplementVelocityControl,
        public yarp::dev::ImplementVelocityDirect,
        public yarp::dev::ImplementControlMode,
        public yarp::dev::ImplementImpedanceControl,
        public yarp::dev::ImplementJointBrake,
        public yarp::dev::ImplementJointFault,
        public yarp::dev::ImplementMotorEncoders,
        public yarp::dev::ImplementTorqueControl,
        public yarp::dev::ImplementControlLimits,
        public yarp::dev::ImplementPositionDirect,
        public yarp::dev::ImplementInteractionMode,
        public yarp::dev::ImplementCurrentControl,
        public yarp::dev::ImplementPWMControl,
        public yarp::dev::ImplementMotor,
        public yarp::dev::ImplementAxisInfo,
        public yarp::dev::ImplementVirtualAnalogSensor //*
{
private:
    enum VerboseLevel
    {
        MUTE                = 0,    // only errors that prevent device from working
        QUIET               = 1,    // adds errors that can cause malfunctioning
        DEFAULT             = 2,    // adds warnings // DEFAULT // show noisy messages about back-compatible changes
        CHATTY              = 3,    // adds info messages
        VERBOSE             = 4,    // adds debug messages
        VERY_VERBOSE        = 5,    // adds trace of events (shows thread running and catch if they get stuck)
        VERY_VERY_VERBOSE   = 6     // adds messages printed every cycle, so too much verbose for usage, only for deep debugging
    };

    const int npids = 3;

    std::recursive_mutex _mutex;
    double _cycleTimestamp;
    int  _njoints;
    int *_axisMap= nullptr;                              /** axis remapping lookup-table */
    double* _angleToEncoder = nullptr; /** angle to iCubDegrees conversion factors */
    double* _encodersStamp = nullptr;                    /** keep information about acquisition time for encoders read */
    double* _ampsToSensor = nullptr;
    double* _dutycycleToPWM = nullptr;
    double* _tempLimit = nullptr;
    float* _DEPRECATED_encoderconversionfactor = nullptr; /** iCubDegrees to encoder conversion factors */
    float* _DEPRECATED_encoderconversionoffset = nullptr;            /** iCubDegrees offset */
//     uint8_t *_jointEncoderType;              /** joint encoder type*/
    int* _jointEncoderRes = nullptr;            /** joint encoder resolution */
    int* _rotorEncoderRes = nullptr;            /** rotor encoder resolution */
                                                //     uint8_t *_rotorEncoderType= nullptr;                  /** rotor encoder type*/
    double* _gearbox = nullptr;                 /** the gearbox ratio */
    bool* _hasHallSensor = nullptr;             /** */
    bool* _hasTempSensor = nullptr;             /** */
    bool* _hasRotorEncoder = nullptr;           /** */
    bool* _hasRotorEncoderIndex = nullptr;      /** */
    int* _rotorIndexOffset = nullptr;           /** */
    int* _motorPoles = nullptr;                 /** */
    double* _rotorlimits_max = nullptr;         /** */
    double* _rotorlimits_min = nullptr;         /** */
    std::vector<yarp::dev::PidWithExtraInfo>* _ppids = nullptr;        /** initial position gains */
    std::vector<yarp::dev::PidWithExtraInfo>* _tpids = nullptr;        /** initial torque gains */
    std::vector<yarp::dev::PidWithExtraInfo>* _cpids = nullptr;        /** initial current gains */
    std::vector<yarp::dev::PidWithExtraInfo>* _vpids = nullptr;        /** initial velocity gains */
    std::vector<yarp::dev::PidWithExtraInfo>* _pdpids = nullptr;       /** initial position direct gains */
    std::vector<yarp::dev::PidWithExtraInfo>* _vdpids = nullptr;       /** initial velocity direct gains */
    std::vector<yarp::dev::PidWithExtraInfo>* _mpids = nullptr;        /** initial mixed gains */

    std::vector<bool> *_ppids_ena    = nullptr;
    std::vector<bool> *_tpids_ena    = nullptr;
    std::vector<bool> *_cpids_ena    = nullptr;
    std::vector<bool> *_vpids_ena    = nullptr;
    std::vector<bool> *_pdpids_ena   = nullptr;
    std::vector<bool> *_vdpids_ena   = nullptr;
    std::vector<bool> *_mpids_ena    = nullptr;

    std::vector<double> *_ppids_lim  = nullptr;
    std::vector<double> *_tpids_lim  = nullptr;
    std::vector<double> *_cpids_lim  = nullptr;
    std::vector<double> *_vpids_lim  = nullptr;
    std::vector<double> *_pdpids_lim  = nullptr;
    std::vector<double> *_vdpids_lim  = nullptr;
    std::vector<double> *_mpids_lim  = nullptr;

    std::vector<double> *_ppids_ref  = nullptr;
    std::vector<double> *_tpids_ref  = nullptr;
    std::vector<double> *_cpids_ref  = nullptr;
    std::vector<double> *_vpids_ref  = nullptr;
    std::vector<double> *_pdpids_ref  = nullptr;
    std::vector<double> *_vdpids_ref  = nullptr;
    std::vector<double> *_mpids_ref  = nullptr;

    std::vector<double> *_ppids_ffd  = nullptr;
    std::vector<double> *_tpids_ffd  = nullptr;
    std::vector<double> *_cpids_ffd  = nullptr;
    std::vector<double> *_vpids_ffd  = nullptr;
    std::vector<double> *_pdpids_ffd  = nullptr;
    std::vector<double> *_vdpids_ffd  = nullptr;
    std::vector<double> *_mpids_ffd  = nullptr;

    std::vector<yarp::dev::SelectableControlModeEnum>* _availableControlModes = nullptr;
    std::vector<yarp::dev::PidControlTypeEnum>*        _availablePids = nullptr;

    std::string* _axisName = nullptr;                      /** axis name */
    yarp::dev::JointTypeEnum* _jointType = nullptr;        /** axis type */
//    ImpedanceLimits     *_impedance_limits= nullptr;     /** impedance limits */
    double* _limitsMin = nullptr;               /** joint limits, max*/
    double* _limitsMax = nullptr;               /** joint limits, min*/
    double* _kinematic_mj = nullptr;            /** the kinematic coupling matrix from joints space to motor space */
    //double *_currentLimits= nullptr;          /** current limits */
//     MotorCurrentLimits *_currentLimits= nullptr;
    double* _minJntCmdVelocity = nullptr;       /** max joint commanded velocity */
    double* _maxJntCmdVelocity = nullptr;       /** max joint commanded velocity */
    double* _maxMotorVelocity = nullptr;        /** max motor velocity */
    int* _velocityShifts = nullptr;             /** velocity shifts */
    int* _velocityTimeout = nullptr;            /** velocity shifts */
    double* _kbemf = nullptr;                   /** back-emf compensation parameter */
    double* _ktau = nullptr;                    /** motor torque constant */
    int* _kbemf_scale = nullptr;                /** back-emf compensation parameter */
    int* _ktau_scale = nullptr;                 /** motor torque constant */
    double* _viscousPos = nullptr;              /** viscous pos friction  */
    double* _viscousNeg = nullptr;              /** viscous neg friction  */
    double* _coulombPos = nullptr;              /** coulomb up friction  */
    double* _coulombNeg = nullptr;              /** coulomb neg friction */
    double* _velocityThres = nullptr;           /** velocity threshold for torque control */
    int* _filterType = nullptr;                 /** the filter type (int value) used by the force control algorithm */
    int* _torqueSensorId = nullptr;             /** Id of associated Joint Torque Sensor */
    int* _torqueSensorChan = nullptr;           /** Channel of associated Joint Torque Sensor */
    double* _maxTorque = nullptr;               /** Max torque of a joint */
    double* _newtonsToSensor = nullptr;         /** Newtons to force sensor units conversion factors */
    bool* checking_motiondone = nullptr;        /* flag telling if I'm already waiting for motion done */
    double* _last_position_move_time = nullptr; /** time stamp for last received position move command*/
    double* _motorPwmLimits = nullptr;          /** motors PWM limits*/
    double* _torques = nullptr;                 /** joint torques */
    bool* _braked= nullptr;
    bool* _autobraked= nullptr;

//     ImpedanceParameters *_impedance_params;     /** impedance parameters */

    bool        verbosewhenok;
    bool        useRawEncoderData;
    bool        _pwmIsLimited;                         /** set to true if pwm is limited */
    bool        _torqueControlEnabled;                 /** set to true if the torque control parameters are successfully loaded. If false, boards cannot switch in torque mode */

    enum       torqueControlUnitsType {T_MACHINE_UNITS=0, T_METRIC_UNITS=1};
    torqueControlUnitsType _torqueControlUnits;

    enum       positionControlUnitsType {P_MACHINE_UNITS=0, P_METRIC_UNITS=1};
    positionControlUnitsType _positionControlUnits;

    // internal stuff
    bool    velocity_watchdog_enabled = false; //false for testing purposes. On the real robot is true.
    bool    openloop_watchdog_enabled = false; //false for testing purposes. On the real robot is true.
    int     *_controlModes = nullptr;
    int     *_hwfault_code = nullptr;
    std::string  *_hwfault_message = nullptr;
    int     *_interactMode = nullptr;
    bool    *_enabledAmp = nullptr;           // Middle step toward a full enabled motor controller. Amp (pwm) plus Pid enable command must be sent in order to get the joint into an active state.
    bool    *_enabledPid = nullptr;           // Depends on enabledAmp. When both are set, the joint exits the idle mode and goes into position mode. If one of them is disabled, it falls to idle.
    bool    *_calibrated = nullptr;           // Flag to know if the calibrate function has been called for the joint
    double  *_posCtrl_references = nullptr;   // used for position control.
    double  *_posDir_references = nullptr;    // used for position Direct control.
    double  *_ref_speeds = nullptr;           // used for position control.
    double  *_command_speeds = nullptr;       // used for velocity control.
    double  *_dir_vel_commands = nullptr;      // used for direct velocity control.
    double  *_ref_accs = nullptr;             // for velocity control, in position min jerk eq is used.
    double  *_ref_torques = nullptr;          // for torque control.
    double  *_ref_currents = nullptr;

    double  *_stiffness = nullptr;
    double  *_damping = nullptr;
    double  *_force_offset = nullptr;

    yarp::sig::Vector       current, nominalCurrent, maxCurrent, peakCurrent;
    yarp::sig::Vector       pwm, pwmLimit, refpwm, supplyVoltage,last_velocity_command, last_pwm_command;
    yarp::sig::Vector pos, dpos, vel, speed, acc, loc, amp;
    double prev_time;
    bool opened;

    // debugging
    VerboseLevel verbose;
public:

    FakeMotionControl();
    ~FakeMotionControl();

  // Device Driver
    bool open(yarp::os::Searchable &par) override;
    bool close() override;
    bool fromConfig(yarp::os::Searchable &config);

    virtual bool initialised();

    /**
     * Allocated buffers.
     */
    bool alloc(int njoints);

    /**
     * Resize previously allocated buffers.
     */
    void resizeBuffers();

    bool threadInit() override;
    void threadRelease() override;

    /////////   PID INTERFACE   /////////
    yarp::dev::ReturnValue getAvailablePidsRaw(int j, std::vector<yarp::dev::PidControlTypeEnum>& avail) override;
    yarp::dev::ReturnValue setPidRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j, const yarp::dev::Pid &pid) override;
    yarp::dev::ReturnValue setPidsRaw(const yarp::dev::PidControlTypeEnum& pidtype,const yarp::dev::Pid *pids) override;
    yarp::dev::ReturnValue setPidReferenceRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j, double ref) override;
    yarp::dev::ReturnValue setPidReferencesRaw(const yarp::dev::PidControlTypeEnum& pidtype,const double *refs) override;
    yarp::dev::ReturnValue setPidErrorLimitRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j, double limit) override;
    yarp::dev::ReturnValue setPidErrorLimitsRaw(const yarp::dev::PidControlTypeEnum& pidtype,const double *limits) override;
    yarp::dev::ReturnValue getPidErrorRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j, double *err) override;
    yarp::dev::ReturnValue getPidErrorsRaw(const yarp::dev::PidControlTypeEnum& pidtype, double *errs) override;
    yarp::dev::ReturnValue getPidOutputRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j, double *out) override;
    yarp::dev::ReturnValue getPidOutputsRaw(const yarp::dev::PidControlTypeEnum& pidtype,double *outs) override;
    yarp::dev::ReturnValue getPidRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j, yarp::dev::Pid *pid) override;
    yarp::dev::ReturnValue getPidsRaw(const yarp::dev::PidControlTypeEnum& pidtype, yarp::dev::Pid *pids) override;
    yarp::dev::ReturnValue getPidReferenceRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j, double *ref) override;
    yarp::dev::ReturnValue getPidReferencesRaw(const yarp::dev::PidControlTypeEnum& pidtype,double *refs) override;
    yarp::dev::ReturnValue getPidErrorLimitRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j, double *limit) override;
    yarp::dev::ReturnValue getPidErrorLimitsRaw(const yarp::dev::PidControlTypeEnum& pidtype,double *limits) override;
    yarp::dev::ReturnValue resetPidRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j) override;
    yarp::dev::ReturnValue disablePidRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j) override;
    yarp::dev::ReturnValue enablePidRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j) override;
    yarp::dev::ReturnValue setPidOffsetRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j, double v) override;
    yarp::dev::ReturnValue setPidFeedforwardRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j, double v) override;
    yarp::dev::ReturnValue getPidOffsetRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j, double& v) override;
    yarp::dev::ReturnValue getPidFeedforwardRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j, double& v) override;
    yarp::dev::ReturnValue isPidEnabledRaw(const yarp::dev::PidControlTypeEnum& pidtype, int j, bool& enabled) override;
    yarp::dev::ReturnValue getPidExtraInfoRaw(const yarp::dev::PidControlTypeEnum& pidtype, int j, yarp::dev::PidExtraInfo& units) override;
    yarp::dev::ReturnValue getPidExtraInfosRaw(const yarp::dev::PidControlTypeEnum& pidtype, std::vector<yarp::dev::PidExtraInfo>& units) override;

    // POSITION CONTROL INTERFACE RAW
    yarp::dev::ReturnValue getAxes(int *ax) override;
    yarp::dev::ReturnValue positionMoveRaw(int j, double ref) override;
    yarp::dev::ReturnValue positionMoveRaw(const double *refs) override;
    yarp::dev::ReturnValue relativeMoveRaw(int j, double delta) override;
    yarp::dev::ReturnValue relativeMoveRaw(const double *deltas) override;
    yarp::dev::ReturnValue checkMotionDoneRaw(bool *flag) override;
    yarp::dev::ReturnValue checkMotionDoneRaw(int j, bool *flag) override;
    yarp::dev::ReturnValue setTrajSpeedRaw(int j, double sp) override;
    yarp::dev::ReturnValue setTrajSpeedsRaw(const double *spds) override;
    yarp::dev::ReturnValue getTrajSpeedRaw(int j, double *ref) override;
    yarp::dev::ReturnValue getTrajSpeedsRaw(double *spds) override;
    yarp::dev::ReturnValue setTrajAccelerationRaw(int j, double acc) override;
    yarp::dev::ReturnValue setTrajAccelerationsRaw(const double *accs) override;
    yarp::dev::ReturnValue getTrajAccelerationRaw(int j, double *acc) override;
    yarp::dev::ReturnValue getTrajAccelerationsRaw(double *accs) override;
    yarp::dev::ReturnValue stopRaw(int j) override;
    yarp::dev::ReturnValue stopRaw() override;
    yarp::dev::ReturnValue positionMoveRaw(const int n_joint, const int *joints, const double *refs) override;
    yarp::dev::ReturnValue relativeMoveRaw(const int n_joint, const int *joints, const double *deltas) override;
    yarp::dev::ReturnValue checkMotionDoneRaw(const int n_joint, const int *joints, bool *flags) override;
    yarp::dev::ReturnValue setTrajSpeedsRaw(const int n_joint, const int *joints, const double *spds) override;
    yarp::dev::ReturnValue setTrajAccelerationsRaw(const int n_joint, const int *joints, const double *accs) override;
    yarp::dev::ReturnValue getTrajSpeedsRaw(const int n_joint, const int *joints, double *spds) override;
    yarp::dev::ReturnValue getTrajAccelerationsRaw(const int n_joint, const int *joints, double *accs) override;
    yarp::dev::ReturnValue stopRaw(const int n_joint, const int *joints) override;
    yarp::dev::ReturnValue getTargetPositionRaw(const int joint, double *ref) override;
    yarp::dev::ReturnValue getTargetPositionsRaw(double *refs) override;
    yarp::dev::ReturnValue getTargetPositionsRaw(const int n_joint, const int *joints, double *refs) override;

    // IJointFault
    yarp::dev::ReturnValue getLastJointFaultRaw(int j, int& fault, std::string& message) override;

    // calibration2raw
    yarp::dev::ReturnValue setCalibrationParametersRaw(int axis, const yarp::dev::CalibrationParameters& params) override;
    yarp::dev::ReturnValue calibrateAxisWithParamsRaw(int axis, unsigned int type, double p1, double p2, double p3) override;
    yarp::dev::ReturnValue calibrationDoneRaw(int j) override;
    /////////////////////////////// END Position Control INTERFACE

    //////////////////////// BEGINControlMode
    yarp::dev::ReturnValue getAvailableControlModesRaw(int j, std::vector<yarp::dev::SelectableControlModeEnum>& avail) override;
    yarp::dev::ReturnValue getControlModeRaw(int j, yarp::dev::ControlModeEnum& mode) override;
    yarp::dev::ReturnValue getControlModesRaw(std::vector<int> joints, std::vector<yarp::dev::ControlModeEnum>& modes) override;
    yarp::dev::ReturnValue getControlModesRaw(std::vector<yarp::dev::ControlModeEnum>& mode) override;
    yarp::dev::ReturnValue setControlModeRaw(int j, yarp::dev::SelectableControlModeEnum mode) override;
    yarp::dev::ReturnValue setControlModesRaw(std::vector<int> joints, std::vector<yarp::dev::SelectableControlModeEnum> modes) override;
    yarp::dev::ReturnValue setControlModesRaw(std::vector<yarp::dev::SelectableControlModeEnum> mode) override;

    //////////////////////// END ControlMode

    //////////////////////// BEGIN EncoderInterface
    yarp::dev::ReturnValue resetEncoderRaw(int j) override;
    yarp::dev::ReturnValue resetEncodersRaw() override;
    yarp::dev::ReturnValue setEncoderRaw(int j, double val) override;
    yarp::dev::ReturnValue setEncodersRaw(const double *vals) override;
    yarp::dev::ReturnValue getEncoderRaw(int j, double *v) override;
    yarp::dev::ReturnValue getEncodersRaw(double *encs) override;
    yarp::dev::ReturnValue getEncoderSpeedRaw(int j, double *sp) override;
    yarp::dev::ReturnValue getEncoderSpeedsRaw(double *spds) override;
    yarp::dev::ReturnValue getEncoderAccelerationRaw(int j, double *spds) override;
    yarp::dev::ReturnValue getEncoderAccelerationsRaw(double *accs) override;
    ///////////////////////// END Encoder Interface

    yarp::dev::ReturnValue getEncodersTimedRaw(double *encs, double *stamps) override;
    yarp::dev::ReturnValue getEncoderTimedRaw(int j, double *encs, double *stamp) override;

    //////////////////////// BEGIN MotorEncoderInterface
    yarp::dev::ReturnValue getNumberOfMotorEncodersRaw(int * num) override;
    yarp::dev::ReturnValue resetMotorEncoderRaw(int m) override;
    yarp::dev::ReturnValue resetMotorEncodersRaw() override;
    yarp::dev::ReturnValue setMotorEncoderRaw(int m, const double val) override;
    yarp::dev::ReturnValue setMotorEncodersRaw(const double *vals) override;
    yarp::dev::ReturnValue getMotorEncoderRaw(int m, double *v) override;
    yarp::dev::ReturnValue getMotorEncodersRaw(double *encs) override;
    yarp::dev::ReturnValue getMotorEncoderSpeedRaw(int m, double *sp) override;
    yarp::dev::ReturnValue getMotorEncoderSpeedsRaw(double *spds) override;
    yarp::dev::ReturnValue getMotorEncoderAccelerationRaw(int m, double *spds) override;
    yarp::dev::ReturnValue getMotorEncoderAccelerationsRaw(double *accs) override;
    yarp::dev::ReturnValue getMotorEncodersTimedRaw(double *encs, double *stamps) override;
    yarp::dev::ReturnValue getMotorEncoderTimedRaw(int m, double *encs, double *stamp) override;
    yarp::dev::ReturnValue getMotorEncoderCountsPerRevolutionRaw(int m, double *v) override;
    yarp::dev::ReturnValue setMotorEncoderCountsPerRevolutionRaw(int m, const double cpr) override;
    ///////////////////////// END MotorEncoder Interface

    //////////////////////// BEGIN IAxisInfo Interface
    yarp::dev::ReturnValue getAxisNameRaw(int axis, std::string& name) override;
    yarp::dev::ReturnValue getJointTypeRaw(int axis, yarp::dev::JointTypeEnum& type) override;
    ///////////////////////// END IAxisInfo Interface

    //Internal use, not exposed by YARP (yet)
    virtual bool getRotorEncoderResolutionRaw(int m, double &rotres);
    virtual bool getJointEncoderResolutionRaw(int m, double &jntres);
    virtual bool getJointEncoderTypeRaw(int j, int &type);
    virtual bool getRotorEncoderTypeRaw(int j, int &type);
    virtual bool getKinematicMJRaw(int j, double &rotres);
    virtual bool getHasTempSensorsRaw(int j, int& ret);
    virtual bool getHasHallSensorRaw(int j, int& ret);
    virtual bool getHasRotorEncoderRaw(int j, int& ret);
    virtual bool getHasRotorEncoderIndexRaw(int j, int& ret);
    virtual bool getMotorPolesRaw(int j, int& poles);
    virtual bool getRotorIndexOffsetRaw(int j, double& rotorOffset);
    virtual bool getTorqueControlFilterType(int j, int& type);

    ////// Amplifier interface
    yarp::dev::ReturnValue enableAmpRaw(int j) override;
    yarp::dev::ReturnValue disableAmpRaw(int j) override;
    yarp::dev::ReturnValue getCurrentsRaw(double *vals) override;
    yarp::dev::ReturnValue getCurrentRaw(int j, double *val) override;
    yarp::dev::ReturnValue getNominalCurrentRaw(int m, double *val) override;
    yarp::dev::ReturnValue setNominalCurrentRaw(int m, const double val) override;
    yarp::dev::ReturnValue setMaxCurrentRaw(int j, double val) override;
    yarp::dev::ReturnValue getMaxCurrentRaw(int j, double *val) override;
    yarp::dev::ReturnValue getPeakCurrentRaw(int m, double *val) override;
    yarp::dev::ReturnValue setPeakCurrentRaw(int m, const double val) override;
    yarp::dev::ReturnValue getAmpStatusRaw(int *st) override;
    yarp::dev::ReturnValue getAmpStatusRaw(int j, int *st) override;
    yarp::dev::ReturnValue getPWMRaw(int j, double* val) override;
    yarp::dev::ReturnValue getPWMLimitRaw(int j, double* val) override;
    yarp::dev::ReturnValue setPWMLimitRaw(int j, const double val) override;
    yarp::dev::ReturnValue getPowerSupplyVoltageRaw(int j, double* val) override;
    /////////////// END AMPLIFIER INTERFACE

    // Limits
    yarp::dev::ReturnValue setPosLimitsRaw(int axis, double min, double max) override;
    yarp::dev::ReturnValue getPosLimitsRaw(int axis, double *min, double *max) override;
    yarp::dev::ReturnValue setVelLimitsRaw(int axis, double min, double max) override;
    yarp::dev::ReturnValue getVelLimitsRaw(int axis, double *min, double *max) override;

    // Torque control
    yarp::dev::ReturnValue getTorqueRaw(int j, double *t) override;
    yarp::dev::ReturnValue getTorquesRaw(double *t) override;
    yarp::dev::ReturnValue getTorqueRangeRaw(int j, double *min, double *max) override;
    yarp::dev::ReturnValue getTorqueRangesRaw(double *min, double *max) override;
    yarp::dev::ReturnValue setRefTorquesRaw(const double *t) override;
    yarp::dev::ReturnValue setRefTorqueRaw(int j, double t) override;
    yarp::dev::ReturnValue setRefTorquesRaw(const int n_joint, const int *joints, const double *t) override;
    yarp::dev::ReturnValue getRefTorquesRaw(double *t) override;
    yarp::dev::ReturnValue getRefTorqueRaw(int j, double *t) override;
    yarp::dev::ReturnValue getMotorTorqueParamsRaw(int j, yarp::dev::MotorTorqueParameters *params) override;
    yarp::dev::ReturnValue setMotorTorqueParamsRaw(int j, const yarp::dev::MotorTorqueParameters params) override;

    // IVelocityControl interface
    yarp::dev::ReturnValue velocityMoveRaw(int j, double sp) override;
    yarp::dev::ReturnValue velocityMoveRaw(const double *sp) override;
    yarp::dev::ReturnValue velocityMoveRaw(const int n_joint, const int *joints, const double *spds) override;
    yarp::dev::ReturnValue getTargetVelocityRaw(const int joint, double *ref) override;
    yarp::dev::ReturnValue getTargetVelocitiesRaw(double *refs) override;
    yarp::dev::ReturnValue getTargetVelocitiesRaw(const int n_joint, const int *joints, double *refs) override;

    // Impedance interface
    yarp::dev::ReturnValue getImpedanceRaw(int j, double *stiffness, double *damping) override;
    yarp::dev::ReturnValue setImpedanceRaw(int j, double stiffness, double damping) override;
    yarp::dev::ReturnValue setImpedanceOffsetRaw(int j, double offset) override;
    yarp::dev::ReturnValue getImpedanceOffsetRaw(int j, double *offset) override;
    yarp::dev::ReturnValue getCurrentImpedanceLimitRaw(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp) override;

    // PositionDirect Interface
    yarp::dev::ReturnValue setPositionRaw(int j, double ref) override;
    yarp::dev::ReturnValue setPositionsRaw(const int n_joint, const int *joints, const double *refs) override;
    yarp::dev::ReturnValue setPositionsRaw(const double *refs) override;
    yarp::dev::ReturnValue getRefPositionRaw(const int joint, double *ref) override;
    yarp::dev::ReturnValue getRefPositionsRaw(double *refs) override;
    yarp::dev::ReturnValue getRefPositionsRaw(const int n_joint, const int *joints, double *refs) override;

    // InteractionMode interface
    yarp::dev::ReturnValue getInteractionModeRaw(int j, yarp::dev::InteractionModeEnum* _mode) override;
    yarp::dev::ReturnValue getInteractionModesRaw(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes) override;
    yarp::dev::ReturnValue getInteractionModesRaw(yarp::dev::InteractionModeEnum* modes) override;
    yarp::dev::ReturnValue setInteractionModeRaw(int j, yarp::dev::InteractionModeEnum _mode) override;
    yarp::dev::ReturnValue setInteractionModesRaw(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes) override;
    yarp::dev::ReturnValue setInteractionModesRaw(yarp::dev::InteractionModeEnum* modes) override;

    // IMotor interface
    yarp::dev::ReturnValue getNumberOfMotorsRaw(int * num) override;
    yarp::dev::ReturnValue getTemperatureRaw(int m, double* val) override;
    yarp::dev::ReturnValue getTemperaturesRaw(double *vals) override;
    yarp::dev::ReturnValue getTemperatureLimitRaw(int m, double *temp) override;
    yarp::dev::ReturnValue setTemperatureLimitRaw(int m, const double temp) override;
    yarp::dev::ReturnValue getGearboxRatioRaw(int m, double* gearbox) override;
    yarp::dev::ReturnValue setGearboxRatioRaw(int m, const double val) override;

    // PWM interface
    yarp::dev::ReturnValue setRefDutyCycleRaw(int j, double v) override;
    yarp::dev::ReturnValue setRefDutyCyclesRaw(const double *v) override;
    yarp::dev::ReturnValue getRefDutyCycleRaw(int j, double *v) override;
    yarp::dev::ReturnValue getRefDutyCyclesRaw(double *v) override;
    yarp::dev::ReturnValue getDutyCycleRaw(int j, double *v) override;
    yarp::dev::ReturnValue getDutyCyclesRaw(double *v) override;

    //IJointBrake
    yarp::dev::ReturnValue isJointBrakedRaw(int j, bool& braked) const override;
    yarp::dev::ReturnValue setManualBrakeActiveRaw(int j, bool active) override;
    yarp::dev::ReturnValue setAutoBrakeEnabledRaw(int j, bool enabled) override;
    yarp::dev::ReturnValue getAutoBrakeEnabledRaw(int j, bool& enabled) const override;

    // IVelocityDirect
    yarp::dev::ReturnValue getAxes(size_t& axes) override;
    yarp::dev::ReturnValue setRefVelocityRaw(int jnt, double vel) override;
    yarp::dev::ReturnValue setRefVelocityRaw(const std::vector<double>& vels) override;
    yarp::dev::ReturnValue setRefVelocityRaw(const std::vector<int>& jnts, const std::vector<double>& vels) override;
    yarp::dev::ReturnValue getRefVelocityRaw(const int jnt, double& vel) override;
    yarp::dev::ReturnValue getRefVelocityRaw(std::vector<double>& vels) override;
    yarp::dev::ReturnValue getRefVelocityRaw(const std::vector<int>& jnts, std::vector<double>& vels) override;

    // Current interface
    //yarp::dev::ReturnValue getAxes(int *ax) override;
    //yarp::dev::ReturnValue getCurrentRaw(int j, double *t) override;
    //yarp::dev::ReturnValue getCurrentsRaw(double *t) override;
    yarp::dev::ReturnValue getCurrentRangeRaw(int j, double *min, double *max) override;
    yarp::dev::ReturnValue getCurrentRangesRaw(double *min, double *max) override;
    yarp::dev::ReturnValue setRefCurrentsRaw(const double *t) override;
    yarp::dev::ReturnValue setRefCurrentRaw(int j, double t) override;
    yarp::dev::ReturnValue setRefCurrentsRaw(const int n_joint, const int *joints, const double *t) override;
    yarp::dev::ReturnValue getRefCurrentsRaw(double *t) override;
    yarp::dev::ReturnValue getRefCurrentRaw(int j, double *t) override;

    yarp::dev::VAS_status getVirtualAnalogSensorStatusRaw(int ch) override;
    int getVirtualAnalogSensorChannelsRaw() override;
    bool updateVirtualAnalogSensorMeasureRaw(yarp::sig::Vector &measure) override;
    bool updateVirtualAnalogSensorMeasureRaw(int ch, double &measure) override;

    void run() override;
private:
    void cleanup();
    bool dealloc();
    void setInfoPPids(int j);
    void setInfoVPids(int j);
    void setInfoPDPids(int j);
    void setInfoVDPids(int j);
    void setInfoTPids(int j);
    void setInfoCPids(int j);
    void setInfoMPids(int j);

    //bool parsePositionPidsGroup(yarp::os::Bottle& pidsGroup, yarp::dev::Pid myPid[]);
    //bool parseTorquePidsGroup(yarp::os::Bottle& pidsGroup, yarp::dev::Pid myPid[], double kbemf[], double ktau[], int filterType[], double viscousPos[], double viscousNeg[], double coulombPos[], double coulombNeg[], double velocityThres[]);
    //bool parseImpedanceGroup_NewFormat(yarp::os::Bottle& pidsGroup, ImpedanceParameters vals[]);
    //bool extractGroup(yarp::os::Bottle &input, yarp::os::Bottle &out, const std::string &key1, const std::string &txt, int size);
};

#endif  // YARP_DEVICE_FAKE_MOTIONCONTROL
