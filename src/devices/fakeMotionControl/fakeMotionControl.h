/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

/*
 * The aim of this device is to mimic the expected behaviour of a
 * real motion control device to help testing the high level software.
 *
 * This device is implementing last version of interfaces and it is compatible
 * with ControlBoardWrapper2 device.
 *
 * WIP - it is very basic now, not all interfaces are implemented yet.
 */

#ifndef YARP_DEVICE_FAKE_MOTIONCONTROL
#define YARP_DEVICE_FAKE_MOTIONCONTROL

#include <yarp/os/Time.h>
#include <yarp/os/Bottle.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Mutex.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardInterfacesImpl-inl.h>
#include <yarp/dev/IVirtualAnalogSensor.h>
#include <yarp/dev/IVirtualAnalogSensorImpl.h>

namespace yarp {
    namespace dev {
      class FakeMotionControl;
    }
}

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

class yarp::dev::FakeMotionControl :    public DeviceDriver,
//                                         public DeviceResponder,
                                        public yarp::os::PeriodicThread,
                                        public IPidControlRaw,
                                        public IControlCalibrationRaw,
                                        public IAmplifierControlRaw,
                                        public IEncodersTimedRaw,
                                        public IMotorEncodersRaw,
                                        public IMotorRaw,
                                        public IPositionControlRaw,
                                        public IVelocityControlRaw,
                                        public IControlModeRaw,
                                        public IControlLimitsRaw,
                                        public IPositionDirectRaw,
                                        public ITorqueControlRaw,
                                        public ICurrentControlRaw,
                                        public IPWMControlRaw,
                                        public IImpedanceControlRaw,
                                        public IInteractionModeRaw,
                                        public IAxisInfoRaw,
                                        public IVirtualAnalogSensorRaw, //*
                                        public ImplementControlCalibration<FakeMotionControl, IControlCalibration>,
                                        public ImplementAmplifierControl<FakeMotionControl, IAmplifierControl>,
                                        public ImplementPidControl,
                                        public ImplementEncodersTimed,
                                        public ImplementPositionControl,
                                        public ImplementVelocityControl,
                                        public ImplementControlMode,
                                        public ImplementImpedanceControl,
                                        public ImplementMotorEncoders,
                                        public ImplementTorqueControl,
                                        public ImplementControlLimits,
                                        public ImplementPositionDirect,
                                        public ImplementInteractionMode,
                                        public ImplementCurrentControl,
                                        public ImplementPWMControl,
                                        public ImplementMotor,
                                        public ImplementAxisInfo,
                                        public ImplementVirtualAnalogSensor //*
{
private:
    enum VerboseLevel
    {
        MUTE                = 0,    // only errors that prevent device from working
        QUIET               = 1,    // adds errors that can cause misfunctioning
        DEFAULT             = 2,    // adds warnings // DEFAULT // show noisy messages about back-compatible changes
        CHATTY              = 3,    // adds info messages
        VERBOSE             = 4,    // adds debug messages
        VERY_VERBOSE        = 5,    // adds trace of events (shows thread running and catch if they get stuck)
        VERY_VERY_VERBOSE   = 6     // adds messages printed every cycle, so too much verbose for usage, only for deep debugging
    };

    yarp::os::Mutex _mutex;
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
    Pid *_ppids;                                /** initial position gains */
    Pid *_tpids;                                /** initial torque gains */
    Pid *_cpids;                                /** initial current gains */
    Pid *_vpids;                                /** initial velocity gains */
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
    JointTypeEnum *_jointType;                          /** axis type */
//     ImpedanceLimits     *_impedance_limits;     /** impedancel imits */
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
    virtual bool open(yarp::os::Searchable &par) override;
    virtual bool close() override;
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

    virtual bool threadInit() override;
    virtual void threadRelease() override;

    /////////   PID INTERFACE   /////////
    virtual bool setPidRaw(const PidControlTypeEnum& pidtype,int j, const Pid &pid) override;
    virtual bool setPidsRaw(const PidControlTypeEnum& pidtype,const Pid *pids) override;
    virtual bool setPidReferenceRaw(const PidControlTypeEnum& pidtype,int j, double ref) override;
    virtual bool setPidReferencesRaw(const PidControlTypeEnum& pidtype,const double *refs) override;
    virtual bool setPidErrorLimitRaw(const PidControlTypeEnum& pidtype,int j, double limit) override;
    virtual bool setPidErrorLimitsRaw(const PidControlTypeEnum& pidtype,const double *limits) override;
    virtual bool getPidErrorRaw(const PidControlTypeEnum& pidtype,int j, double *err) override;
    virtual bool getPidErrorsRaw(const PidControlTypeEnum& pidtype, double *errs) override;
    virtual bool getPidOutputRaw(const PidControlTypeEnum& pidtype,int j, double *out) override;
    virtual bool getPidOutputsRaw(const PidControlTypeEnum& pidtype,double *outs) override;
    virtual bool getPidRaw(const PidControlTypeEnum& pidtype,int j, Pid *pid) override;
    virtual bool getPidsRaw(const PidControlTypeEnum& pidtype,Pid *pids) override;
    virtual bool getPidReferenceRaw(const PidControlTypeEnum& pidtype,int j, double *ref) override;
    virtual bool getPidReferencesRaw(const PidControlTypeEnum& pidtype,double *refs) override;
    virtual bool getPidErrorLimitRaw(const PidControlTypeEnum& pidtype,int j, double *limit) override;
    virtual bool getPidErrorLimitsRaw(const PidControlTypeEnum& pidtype,double *limits) override;
    virtual bool resetPidRaw(const PidControlTypeEnum& pidtype,int j) override;
    virtual bool disablePidRaw(const PidControlTypeEnum& pidtype,int j) override;
    virtual bool enablePidRaw(const PidControlTypeEnum& pidtype,int j) override;
    virtual bool setPidOffsetRaw(const PidControlTypeEnum& pidtype,int j, double v) override;
    virtual bool isPidEnabledRaw(const PidControlTypeEnum& pidtype, int j, bool* enabled) override;

    // POSITION CONTROL INTERFACE RAW
    virtual bool getAxes(int *ax) override;
    virtual bool positionMoveRaw(int j, double ref) override;
    virtual bool positionMoveRaw(const double *refs) override;
    virtual bool relativeMoveRaw(int j, double delta) override;
    virtual bool relativeMoveRaw(const double *deltas) override;
    virtual bool checkMotionDoneRaw(bool *flag) override;
    virtual bool checkMotionDoneRaw(int j, bool *flag) override;
    virtual bool setRefSpeedRaw(int j, double sp) override;
    virtual bool setRefSpeedsRaw(const double *spds) override;
    virtual bool setRefAccelerationRaw(int j, double acc) override;
    virtual bool setRefAccelerationsRaw(const double *accs) override;
    virtual bool getRefSpeedRaw(int j, double *ref) override;
    virtual bool getRefSpeedsRaw(double *spds) override;
    virtual bool getRefAccelerationRaw(int j, double *acc) override;
    virtual bool getRefAccelerationsRaw(double *accs) override;
    virtual bool stopRaw(int j) override;
    virtual bool stopRaw() override;

    // Position Control2 Interface
    virtual bool positionMoveRaw(const int n_joint, const int *joints, const double *refs) override;
    virtual bool relativeMoveRaw(const int n_joint, const int *joints, const double *deltas) override;
    virtual bool checkMotionDoneRaw(const int n_joint, const int *joints, bool *flags) override;
    virtual bool setRefSpeedsRaw(const int n_joint, const int *joints, const double *spds) override;
    virtual bool setRefAccelerationsRaw(const int n_joint, const int *joints, const double *accs) override;
    virtual bool getRefSpeedsRaw(const int n_joint, const int *joints, double *spds) override;
    virtual bool getRefAccelerationsRaw(const int n_joint, const int *joints, double *accs) override;
    virtual bool stopRaw(const int n_joint, const int *joints) override;
    virtual bool getTargetPositionRaw(const int joint, double *ref) override;
    virtual bool getTargetPositionsRaw(double *refs) override;
    virtual bool getTargetPositionsRaw(const int n_joint, const int *joints, double *refs) override;

    //  Velocity control interface raw
    virtual bool velocityMoveRaw(int j, double sp) override;
    virtual bool velocityMoveRaw(const double *sp) override;


    // calibration2raw
    virtual bool setCalibrationParametersRaw(int axis, const CalibrationParameters& params) override;
    using yarp::dev::IControlCalibrationRaw::calibrateRaw;
    virtual bool calibrateRaw(int axis, unsigned int type, double p1, double p2, double p3) override;
    virtual bool doneRaw(int j) override;


    /////////////////////////////// END Position Control INTERFACE

    // ControlMode
    virtual bool getControlModeRaw(int j, int *v) override;
    virtual bool getControlModesRaw(int *v) override;

    // ControlMode 2
    virtual bool getControlModesRaw(const int n_joint, const int *joints, int *modes) override;
    virtual bool setControlModeRaw(const int j, const int mode) override;
    virtual bool setControlModesRaw(const int n_joint, const int *joints, int *modes) override;
    virtual bool setControlModesRaw(int *modes) override;

    //////////////////////// BEGIN EncoderInterface
    virtual bool resetEncoderRaw(int j) override;
    virtual bool resetEncodersRaw() override;
    virtual bool setEncoderRaw(int j, double val) override;
    virtual bool setEncodersRaw(const double *vals) override;
    virtual bool getEncoderRaw(int j, double *v) override;
    virtual bool getEncodersRaw(double *encs) override;
    virtual bool getEncoderSpeedRaw(int j, double *sp) override;
    virtual bool getEncoderSpeedsRaw(double *spds) override;
    virtual bool getEncoderAccelerationRaw(int j, double *spds) override;
    virtual bool getEncoderAccelerationsRaw(double *accs) override;
    ///////////////////////// END Encoder Interface

    virtual bool getEncodersTimedRaw(double *encs, double *stamps) override;
    virtual bool getEncoderTimedRaw(int j, double *encs, double *stamp) override;

    //////////////////////// BEGIN MotorEncoderInterface
    virtual bool getNumberOfMotorEncodersRaw(int * num) override;
    virtual bool resetMotorEncoderRaw(int m) override;
    virtual bool resetMotorEncodersRaw() override;
    virtual bool setMotorEncoderRaw(int m, const double val) override;
    virtual bool setMotorEncodersRaw(const double *vals) override;
    virtual bool getMotorEncoderRaw(int m, double *v) override;
    virtual bool getMotorEncodersRaw(double *encs) override;
    virtual bool getMotorEncoderSpeedRaw(int m, double *sp) override;
    virtual bool getMotorEncoderSpeedsRaw(double *spds) override;
    virtual bool getMotorEncoderAccelerationRaw(int m, double *spds) override;
    virtual bool getMotorEncoderAccelerationsRaw(double *accs) override;
    virtual bool getMotorEncodersTimedRaw(double *encs, double *stamps) override;
    virtual bool getMotorEncoderTimedRaw(int m, double *encs, double *stamp) override;\
    virtual bool getMotorEncoderCountsPerRevolutionRaw(int m, double *v) override;
    virtual bool setMotorEncoderCountsPerRevolutionRaw(int m, const double cpr) override;
    ///////////////////////// END MotorEncoder Interface

    //////////////////////// BEGIN IAxisInfo Interface
    virtual bool getAxisNameRaw(int axis, std::string& name) override;
    virtual bool getJointTypeRaw(int axis, yarp::dev::JointTypeEnum& type) override;
    ///////////////////////// END IAxisInfo Interface

    //Internal use, not exposed by Yarp (yet)
    virtual bool getGearboxRatioRaw(int m, double *gearbox) override;
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
    virtual bool enableAmpRaw(int j) override;
    virtual bool disableAmpRaw(int j) override;
    virtual bool getCurrentsRaw(double *vals) override;
    virtual bool getCurrentRaw(int j, double *val) override;
    virtual bool getNominalCurrentRaw(int m, double *val) override;
    virtual bool setNominalCurrentRaw(int m, const double val) override;
    virtual bool setMaxCurrentRaw(int j, double val) override;
    virtual bool getMaxCurrentRaw(int j, double *val) override;
    virtual bool getPeakCurrentRaw(int m, double *val) override;
    virtual bool setPeakCurrentRaw(int m, const double val) override;
    virtual bool getAmpStatusRaw(int *st) override;
    virtual bool getAmpStatusRaw(int j, int *st) override;
    virtual bool getPWMRaw(int j, double* val) override;
    virtual bool getPWMLimitRaw(int j, double* val) override;
    virtual bool setPWMLimitRaw(int j, const double val) override;
    virtual bool getPowerSupplyVoltageRaw(int j, double* val) override;
    /////////////// END AMPLIFIER INTERFACE

    // Limits
    virtual bool setLimitsRaw(int axis, double min, double max) override;
    virtual bool getLimitsRaw(int axis, double *min, double *max) override;
    // Limits 2
    virtual bool setVelLimitsRaw(int axis, double min, double max) override;
    virtual bool getVelLimitsRaw(int axis, double *min, double *max) override;

    // Torque control
    virtual bool getTorqueRaw(int j, double *t) override;
    virtual bool getTorquesRaw(double *t) override;
    virtual bool getTorqueRangeRaw(int j, double *min, double *max) override;
    virtual bool getTorqueRangesRaw(double *min, double *max) override;
    virtual bool setRefTorquesRaw(const double *t) override;
    virtual bool setRefTorqueRaw(int j, double t) override;
    virtual bool setRefTorquesRaw(const int n_joint, const int *joints, const double *t) override;
    virtual bool getRefTorquesRaw(double *t) override;
    virtual bool getRefTorqueRaw(int j, double *t) override;
    virtual bool getMotorTorqueParamsRaw(int j, MotorTorqueParameters *params) override;
    virtual bool setMotorTorqueParamsRaw(int j, const MotorTorqueParameters params) override;
//     int32_t getRefSpeedInTbl(uint8_t boardNum, int j, eOmeas_position_t pos) override;

    // IVelocityControl2
    virtual bool velocityMoveRaw(const int n_joint, const int *joints, const double *spds) override;
    virtual bool getRefVelocityRaw(const int joint, double *ref) override;
    virtual bool getRefVelocitiesRaw(double *refs) override;
    virtual bool getRefVelocitiesRaw(const int n_joint, const int *joints, double *refs) override;

    // Impedance interface
    virtual bool getImpedanceRaw(int j, double *stiffness, double *damping) override;
    virtual bool setImpedanceRaw(int j, double stiffness, double damping) override;
    virtual bool setImpedanceOffsetRaw(int j, double offset) override;
    virtual bool getImpedanceOffsetRaw(int j, double *offset) override;
    virtual bool getCurrentImpedanceLimitRaw(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp) override;

    // PositionDirect Interface
    virtual bool setPositionRaw(int j, double ref) override;
    virtual bool setPositionsRaw(const int n_joint, const int *joints, const double *refs) override;
    virtual bool setPositionsRaw(const double *refs) override;
    virtual bool getRefPositionRaw(const int joint, double *ref) override;
    virtual bool getRefPositionsRaw(double *refs) override;
    virtual bool getRefPositionsRaw(const int n_joint, const int *joints, double *refs) override;

    // InteractionMode interface
    virtual bool getInteractionModeRaw(int j, yarp::dev::InteractionModeEnum* _mode) override;
    virtual bool getInteractionModesRaw(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes) override;
    virtual bool getInteractionModesRaw(yarp::dev::InteractionModeEnum* modes) override;
    virtual bool setInteractionModeRaw(int j, yarp::dev::InteractionModeEnum _mode) override;
    virtual bool setInteractionModesRaw(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes) override;
    virtual bool setInteractionModesRaw(yarp::dev::InteractionModeEnum* modes) override;

    // IMotor interface
    virtual bool getNumberOfMotorsRaw(int * num) override;
    virtual bool getTemperatureRaw(int m, double* val) override;
    virtual bool getTemperaturesRaw(double *vals) override;
    virtual bool getTemperatureLimitRaw(int m, double *temp) override;
    virtual bool setTemperatureLimitRaw(int m, const double temp) override;

    // PWM interface
    virtual bool setRefDutyCycleRaw(int j, double v) override;
    virtual bool setRefDutyCyclesRaw(const double *v) override;
    virtual bool getRefDutyCycleRaw(int j, double *v) override;
    virtual bool getRefDutyCyclesRaw(double *v) override;
    virtual bool getDutyCycleRaw(int j, double *v) override;
    virtual bool getDutyCyclesRaw(double *v) override;

    // Current interface
    //virtual bool getAxes(int *ax) override;
    //virtual bool getCurrentRaw(int j, double *t) override;
    //virtual bool getCurrentsRaw(double *t) override;
    virtual bool getCurrentRangeRaw(int j, double *min, double *max) override;
    virtual bool getCurrentRangesRaw(double *min, double *max) override;
    virtual bool setRefCurrentsRaw(const double *t) override;
    virtual bool setRefCurrentRaw(int j, double t) override;
    virtual bool setRefCurrentsRaw(const int n_joint, const int *joints, const double *t) override;
    virtual bool getRefCurrentsRaw(double *t) override;
    virtual bool getRefCurrentRaw(int j, double *t) override;

    yarp::dev::VAS_status getVirtualAnalogSensorStatusRaw(int ch) override;
    int getVirtualAnalogSensorChannelsRaw() override;
    bool updateVirtualAnalogSensorMeasureRaw(yarp::sig::Vector &measure) override;
    bool updateVirtualAnalogSensorMeasureRaw(int ch, double &measure) override;

    void run() override;
private:
    void cleanup(void);
    bool dealloc();
    bool parsePositionPidsGroup(yarp::os::Bottle& pidsGroup, Pid myPid[]);
    bool parseTorquePidsGroup(yarp::os::Bottle& pidsGroup, Pid myPid[], double kbemf[], double ktau[], int filterType[]);
    bool parseImpedanceGroup_NewFormat(yarp::os::Bottle& pidsGroup, ImpedanceParameters vals[]);

    bool extractGroup(yarp::os::Bottle &input, yarp::os::Bottle &out, const std::string &key1, const std::string &txt, int size);
};

#endif  // YARP_DEVICE_FAKE_MOTIONCONTROL
