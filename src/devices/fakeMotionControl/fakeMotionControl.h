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

#include <mutex>


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
 * \brief `fakeMotionControl`: Documentation to be added
 *
 * The aim of this device is to mimic the expected behavior of a
 * real motion control device to help testing the high level software.
 *
 * This device is implementing last version of interfaces and it is compatible
 * with ControlBoardWrapper2 device.
 *
 * WIP - it is very basic now, not all interfaces are implemented yet.
 */
class FakeMotionControl :
        public yarp::dev::DeviceDriver,
//        public yarp::dev::DeviceResponder,
        public yarp::os::PeriodicThread,
        public yarp::dev::IPidControlRaw,
        public yarp::dev::IControlCalibrationRaw,
        public yarp::dev::IAmplifierControlRaw,
        public yarp::dev::IEncodersTimedRaw,
        public yarp::dev::IMotorEncodersRaw,
        public yarp::dev::IMotorRaw,
        public yarp::dev::IPositionControlRaw,
        public yarp::dev::IVelocityControlRaw,
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
        public yarp::dev::ImplementControlCalibration,
        public yarp::dev::ImplementAmplifierControl,
        public yarp::dev::ImplementPidControl,
        public yarp::dev::ImplementEncodersTimed,
        public yarp::dev::ImplementPositionControl,
        public yarp::dev::ImplementVelocityControl,
        public yarp::dev::ImplementControlMode,
        public yarp::dev::ImplementImpedanceControl,
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

    std::mutex _mutex;
    int  _njoints;
    int *_axisMap;                              /** axis remapping lookup-table */
    double *_angleToEncoder;                    /** angle to iCubDegrees conversion factors */
    double  *_encodersStamp;                    /** keep information about acquisition time for encoders read */
    double *_ampsToSensor;
    double *_dutycycleToPWM;
    float *_DEPRECATED_encoderconversionfactor;            /** iCubDegrees to encoder conversion factors */
    float *_DEPRECATED_encoderconversionoffset;            /** iCubDegrees offset */
//     uint8_t *_jointEncoderType;                 /** joint encoder type*/
    int    *_jointEncoderRes;                   /** joint encoder resolution */
    int    *_rotorEncoderRes;                   /** rotor encoder resolution */
//     uint8_t *_rotorEncoderType;                  /** rotor encoder type*/
    double *_gearbox;                           /** the gearbox ratio */
    bool   *_hasHallSensor;                     /** */
    bool   *_hasTempSensor;                     /** */
    bool   *_hasRotorEncoder;                   /** */
    bool   *_hasRotorEncoderIndex;              /** */
    int    *_rotorIndexOffset;                  /** */
    int    *_motorPoles;                        /** */
    double *_rotorlimits_max;                   /** */
    double *_rotorlimits_min;                   /** */
    yarp::dev::Pid *_ppids;                                /** initial position gains */
    yarp::dev::Pid *_tpids;                                /** initial torque gains */
    yarp::dev::Pid *_cpids;                                /** initial current gains */
    yarp::dev::Pid *_vpids;                                /** initial velocity gains */
    bool *_ppids_ena;
    bool *_tpids_ena;
    bool *_cpids_ena;
    bool *_vpids_ena;
    double *_ppids_lim;
    double *_tpids_lim;
    double *_cpids_lim;
    double *_vpids_lim;
    double *_ppids_ref;
    double *_tpids_ref;
    double *_cpids_ref;
    double *_vpids_ref;

    std::string *_axisName;                          /** axis name */
    yarp::dev::JointTypeEnum *_jointType;                          /** axis type */
//     ImpedanceLimits     *_impedance_limits;     /** impedance limits */
    double *_limitsMin;                         /** joint limits, max*/
    double *_limitsMax;                         /** joint limits, min*/
    double *_kinematic_mj;                      /** the kinematic coupling matrix from joints space to motor space */
    //double *_currentLimits;                     /** current limits */
//     MotorCurrentLimits *_currentLimits;
    double *_maxJntCmdVelocity;                 /** max joint commanded velocity */
    double *_maxMotorVelocity;                  /** max motor velocity */
    int *_velocityShifts;                       /** velocity shifts */
    int *_velocityTimeout;                      /** velocity shifts */
    double *_kbemf;                             /** back-emf compensation parameter */
    double *_ktau;                              /** motor torque constant */
    int *_kbemf_scale;                          /** back-emf compensation parameter */
    int *_ktau_scale;                           /** motor torque constant */
    int * _filterType;                          /** the filter type (int value) used by the force control algorithm */
    int *_torqueSensorId;                       /** Id of associated Joint Torque Sensor */
    int *_torqueSensorChan;                     /** Channel of associated Joint Torque Sensor */
    double *_maxTorque;                         /** Max torque of a joint */
    double *_newtonsToSensor;                   /** Newtons to force sensor units conversion factors */
    bool  *checking_motiondone;                 /* flag telling if I'm already waiting for motion done */
    double *_last_position_move_time;           /** time stamp for last received position move command*/
    double *_motorPwmLimits;                    /** motors PWM limits*/
    double *_torques;                           /** joint torques */

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
    int     *_controlModes;
    int     *_interactMode;
    bool    *_enabledAmp;           // Middle step toward a full enabled motor controller. Amp (pwm) plus Pid enable command must be sent in order to get the joint into an active state.
    bool    *_enabledPid;           // Depends on enabledAmp. When both are set, the joint exits the idle mode and goes into position mode. If one of them is disabled, it falls to idle.
    bool    *_calibrated;           // Flag to know if the calibrate function has been called for the joint
    double  *_posCtrl_references;   // used for position control.
    double  *_posDir_references;    // used for position Direct control.
    double  *_ref_speeds;           // used for position control.
    double  *_command_speeds;       // used for velocity control.
    double  *_ref_accs;             // for velocity control, in position min jerk eq is used.
    double  *_ref_torques;          // for torque control.
    double  *_ref_currents;
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
    bool setPidRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j, const yarp::dev::Pid &pid) override;
    bool setPidsRaw(const yarp::dev::PidControlTypeEnum& pidtype,const yarp::dev::Pid *pids) override;
    bool setPidReferenceRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j, double ref) override;
    bool setPidReferencesRaw(const yarp::dev::PidControlTypeEnum& pidtype,const double *refs) override;
    bool setPidErrorLimitRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j, double limit) override;
    bool setPidErrorLimitsRaw(const yarp::dev::PidControlTypeEnum& pidtype,const double *limits) override;
    bool getPidErrorRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j, double *err) override;
    bool getPidErrorsRaw(const yarp::dev::PidControlTypeEnum& pidtype, double *errs) override;
    bool getPidOutputRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j, double *out) override;
    bool getPidOutputsRaw(const yarp::dev::PidControlTypeEnum& pidtype,double *outs) override;
    bool getPidRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j, yarp::dev::Pid *pid) override;
    bool getPidsRaw(const yarp::dev::PidControlTypeEnum& pidtype, yarp::dev::Pid *pids) override;
    bool getPidReferenceRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j, double *ref) override;
    bool getPidReferencesRaw(const yarp::dev::PidControlTypeEnum& pidtype,double *refs) override;
    bool getPidErrorLimitRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j, double *limit) override;
    bool getPidErrorLimitsRaw(const yarp::dev::PidControlTypeEnum& pidtype,double *limits) override;
    bool resetPidRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j) override;
    bool disablePidRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j) override;
    bool enablePidRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j) override;
    bool setPidOffsetRaw(const yarp::dev::PidControlTypeEnum& pidtype,int j, double v) override;
    bool isPidEnabledRaw(const yarp::dev::PidControlTypeEnum& pidtype, int j, bool* enabled) override;

    // POSITION CONTROL INTERFACE RAW
    bool getAxes(int *ax) override;
    bool positionMoveRaw(int j, double ref) override;
    bool positionMoveRaw(const double *refs) override;
    bool relativeMoveRaw(int j, double delta) override;
    bool relativeMoveRaw(const double *deltas) override;
    bool checkMotionDoneRaw(bool *flag) override;
    bool checkMotionDoneRaw(int j, bool *flag) override;
    bool setRefSpeedRaw(int j, double sp) override;
    bool setRefSpeedsRaw(const double *spds) override;
    bool setRefAccelerationRaw(int j, double acc) override;
    bool setRefAccelerationsRaw(const double *accs) override;
    bool getRefSpeedRaw(int j, double *ref) override;
    bool getRefSpeedsRaw(double *spds) override;
    bool getRefAccelerationRaw(int j, double *acc) override;
    bool getRefAccelerationsRaw(double *accs) override;
    bool stopRaw(int j) override;
    bool stopRaw() override;

    // Position Control2 Interface
    bool positionMoveRaw(const int n_joint, const int *joints, const double *refs) override;
    bool relativeMoveRaw(const int n_joint, const int *joints, const double *deltas) override;
    bool checkMotionDoneRaw(const int n_joint, const int *joints, bool *flags) override;
    bool setRefSpeedsRaw(const int n_joint, const int *joints, const double *spds) override;
    bool setRefAccelerationsRaw(const int n_joint, const int *joints, const double *accs) override;
    bool getRefSpeedsRaw(const int n_joint, const int *joints, double *spds) override;
    bool getRefAccelerationsRaw(const int n_joint, const int *joints, double *accs) override;
    bool stopRaw(const int n_joint, const int *joints) override;
    bool getTargetPositionRaw(const int joint, double *ref) override;
    bool getTargetPositionsRaw(double *refs) override;
    bool getTargetPositionsRaw(const int n_joint, const int *joints, double *refs) override;

    //  Velocity control interface raw
    bool velocityMoveRaw(int j, double sp) override;
    bool velocityMoveRaw(const double *sp) override;


    // calibration2raw
    bool setCalibrationParametersRaw(int axis, const yarp::dev::CalibrationParameters& params) override;
    bool calibrateAxisWithParamsRaw(int axis, unsigned int type, double p1, double p2, double p3) override;
    bool calibrationDoneRaw(int j) override;


    /////////////////////////////// END Position Control INTERFACE

    // ControlMode
    bool getControlModeRaw(int j, int *v) override;
    bool getControlModesRaw(int *v) override;

    // ControlMode 2
    bool getControlModesRaw(const int n_joint, const int *joints, int *modes) override;
    bool setControlModeRaw(const int j, const int mode) override;
    bool setControlModesRaw(const int n_joint, const int *joints, int *modes) override;
    bool setControlModesRaw(int *modes) override;

    //////////////////////// BEGIN EncoderInterface
    bool resetEncoderRaw(int j) override;
    bool resetEncodersRaw() override;
    bool setEncoderRaw(int j, double val) override;
    bool setEncodersRaw(const double *vals) override;
    bool getEncoderRaw(int j, double *v) override;
    bool getEncodersRaw(double *encs) override;
    bool getEncoderSpeedRaw(int j, double *sp) override;
    bool getEncoderSpeedsRaw(double *spds) override;
    bool getEncoderAccelerationRaw(int j, double *spds) override;
    bool getEncoderAccelerationsRaw(double *accs) override;
    ///////////////////////// END Encoder Interface

    bool getEncodersTimedRaw(double *encs, double *stamps) override;
    bool getEncoderTimedRaw(int j, double *encs, double *stamp) override;

    //////////////////////// BEGIN MotorEncoderInterface
    bool getNumberOfMotorEncodersRaw(int * num) override;
    bool resetMotorEncoderRaw(int m) override;
    bool resetMotorEncodersRaw() override;
    bool setMotorEncoderRaw(int m, const double val) override;
    bool setMotorEncodersRaw(const double *vals) override;
    bool getMotorEncoderRaw(int m, double *v) override;
    bool getMotorEncodersRaw(double *encs) override;
    bool getMotorEncoderSpeedRaw(int m, double *sp) override;
    bool getMotorEncoderSpeedsRaw(double *spds) override;
    bool getMotorEncoderAccelerationRaw(int m, double *spds) override;
    bool getMotorEncoderAccelerationsRaw(double *accs) override;
    bool getMotorEncodersTimedRaw(double *encs, double *stamps) override;
    bool getMotorEncoderTimedRaw(int m, double *encs, double *stamp) override;\
    bool getMotorEncoderCountsPerRevolutionRaw(int m, double *v) override;
    bool setMotorEncoderCountsPerRevolutionRaw(int m, const double cpr) override;
    ///////////////////////// END MotorEncoder Interface

    //////////////////////// BEGIN IAxisInfo Interface
    bool getAxisNameRaw(int axis, std::string& name) override;
    bool getJointTypeRaw(int axis, yarp::dev::JointTypeEnum& type) override;
    ///////////////////////// END IAxisInfo Interface

    //Internal use, not exposed by YARP (yet)
    bool getGearboxRatioRaw(int m, double *gearbox) override;
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
    bool enableAmpRaw(int j) override;
    bool disableAmpRaw(int j) override;
    bool getCurrentsRaw(double *vals) override;
    bool getCurrentRaw(int j, double *val) override;
    bool getNominalCurrentRaw(int m, double *val) override;
    bool setNominalCurrentRaw(int m, const double val) override;
    bool setMaxCurrentRaw(int j, double val) override;
    bool getMaxCurrentRaw(int j, double *val) override;
    bool getPeakCurrentRaw(int m, double *val) override;
    bool setPeakCurrentRaw(int m, const double val) override;
    bool getAmpStatusRaw(int *st) override;
    bool getAmpStatusRaw(int j, int *st) override;
    bool getPWMRaw(int j, double* val) override;
    bool getPWMLimitRaw(int j, double* val) override;
    bool setPWMLimitRaw(int j, const double val) override;
    bool getPowerSupplyVoltageRaw(int j, double* val) override;
    /////////////// END AMPLIFIER INTERFACE

    // Limits
    bool setLimitsRaw(int axis, double min, double max) override;
    bool getLimitsRaw(int axis, double *min, double *max) override;
    // Limits 2
    bool setVelLimitsRaw(int axis, double min, double max) override;
    bool getVelLimitsRaw(int axis, double *min, double *max) override;

    // Torque control
    bool getTorqueRaw(int j, double *t) override;
    bool getTorquesRaw(double *t) override;
    bool getTorqueRangeRaw(int j, double *min, double *max) override;
    bool getTorqueRangesRaw(double *min, double *max) override;
    bool setRefTorquesRaw(const double *t) override;
    bool setRefTorqueRaw(int j, double t) override;
    bool setRefTorquesRaw(const int n_joint, const int *joints, const double *t) override;
    bool getRefTorquesRaw(double *t) override;
    bool getRefTorqueRaw(int j, double *t) override;
    bool getMotorTorqueParamsRaw(int j, yarp::dev::MotorTorqueParameters *params) override;
    bool setMotorTorqueParamsRaw(int j, const yarp::dev::MotorTorqueParameters params) override;
//     int32_t getRefSpeedInTbl(uint8_t boardNum, int j, eOmeas_position_t pos) override;

    // IVelocityControl interface
    bool velocityMoveRaw(const int n_joint, const int *joints, const double *spds) override;
    bool getRefVelocityRaw(const int joint, double *ref) override;
    bool getRefVelocitiesRaw(double *refs) override;
    bool getRefVelocitiesRaw(const int n_joint, const int *joints, double *refs) override;

    // Impedance interface
    bool getImpedanceRaw(int j, double *stiffness, double *damping) override;
    bool setImpedanceRaw(int j, double stiffness, double damping) override;
    bool setImpedanceOffsetRaw(int j, double offset) override;
    bool getImpedanceOffsetRaw(int j, double *offset) override;
    bool getCurrentImpedanceLimitRaw(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp) override;

    // PositionDirect Interface
    bool setPositionRaw(int j, double ref) override;
    bool setPositionsRaw(const int n_joint, const int *joints, const double *refs) override;
    bool setPositionsRaw(const double *refs) override;
    bool getRefPositionRaw(const int joint, double *ref) override;
    bool getRefPositionsRaw(double *refs) override;
    bool getRefPositionsRaw(const int n_joint, const int *joints, double *refs) override;

    // InteractionMode interface
    bool getInteractionModeRaw(int j, yarp::dev::InteractionModeEnum* _mode) override;
    bool getInteractionModesRaw(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes) override;
    bool getInteractionModesRaw(yarp::dev::InteractionModeEnum* modes) override;
    bool setInteractionModeRaw(int j, yarp::dev::InteractionModeEnum _mode) override;
    bool setInteractionModesRaw(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes) override;
    bool setInteractionModesRaw(yarp::dev::InteractionModeEnum* modes) override;

    // IMotor interface
    bool getNumberOfMotorsRaw(int * num) override;
    bool getTemperatureRaw(int m, double* val) override;
    bool getTemperaturesRaw(double *vals) override;
    bool getTemperatureLimitRaw(int m, double *temp) override;
    bool setTemperatureLimitRaw(int m, const double temp) override;

    // PWM interface
    bool setRefDutyCycleRaw(int j, double v) override;
    bool setRefDutyCyclesRaw(const double *v) override;
    bool getRefDutyCycleRaw(int j, double *v) override;
    bool getRefDutyCyclesRaw(double *v) override;
    bool getDutyCycleRaw(int j, double *v) override;
    bool getDutyCyclesRaw(double *v) override;

    // Current interface
    //bool getAxes(int *ax) override;
    //bool getCurrentRaw(int j, double *t) override;
    //bool getCurrentsRaw(double *t) override;
    bool getCurrentRangeRaw(int j, double *min, double *max) override;
    bool getCurrentRangesRaw(double *min, double *max) override;
    bool setRefCurrentsRaw(const double *t) override;
    bool setRefCurrentRaw(int j, double t) override;
    bool setRefCurrentsRaw(const int n_joint, const int *joints, const double *t) override;
    bool getRefCurrentsRaw(double *t) override;
    bool getRefCurrentRaw(int j, double *t) override;

    yarp::dev::VAS_status getVirtualAnalogSensorStatusRaw(int ch) override;
    int getVirtualAnalogSensorChannelsRaw() override;
    bool updateVirtualAnalogSensorMeasureRaw(yarp::sig::Vector &measure) override;
    bool updateVirtualAnalogSensorMeasureRaw(int ch, double &measure) override;

    void run() override;
private:
    void cleanup();
    bool dealloc();
    bool parsePositionPidsGroup(yarp::os::Bottle& pidsGroup, yarp::dev::Pid myPid[]);
    bool parseTorquePidsGroup(yarp::os::Bottle& pidsGroup, yarp::dev::Pid myPid[], double kbemf[], double ktau[], int filterType[]);
    bool parseImpedanceGroup_NewFormat(yarp::os::Bottle& pidsGroup, ImpedanceParameters vals[]);

    bool extractGroup(yarp::os::Bottle &input, yarp::os::Bottle &out, const std::string &key1, const std::string &txt, int size);
};

#endif  // YARP_DEVICE_FAKE_MOTIONCONTROL
