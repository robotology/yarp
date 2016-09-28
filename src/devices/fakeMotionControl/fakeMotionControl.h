/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Alberto Cardellino
 * email:   alberto.cardellino@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
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
#include <yarp/os/Semaphore.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardInterfacesImpl.inl>

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
                                        public yarp::os::Thread,
                                        public IPidControlRaw,
                                        public IControlCalibration2Raw,
                                        public IAmplifierControlRaw,
                                        public IEncodersTimedRaw,
                                        public IMotorEncodersRaw,
                                        public IMotorRaw,
                                        public IPositionControl2Raw,
                                        public IVelocityControl2Raw,
                                        public IControlMode2Raw,
                                        public IControlLimits2Raw,
                                        public IPositionDirectRaw,
                                        public ITorqueControlRaw,
                                        public IImpedanceControlRaw,
                                        public IInteractionModeRaw,
                                        public IOpenLoopControlRaw,
                                        public IAxisInfoRaw,
                                        public ImplementControlCalibration2<FakeMotionControl, IControlCalibration2>,
                                        public ImplementAmplifierControl<FakeMotionControl, IAmplifierControl>,
                                        public ImplementPidControl<FakeMotionControl, IPidControl>,
                                        public ImplementEncodersTimed,
                                        public ImplementPositionControl2,
                                        public ImplementVelocityControl<FakeMotionControl, IVelocityControl>,
                                        public ImplementVelocityControl2,
                                        public ImplementControlMode2,
                                        public ImplementImpedanceControl,
                                        public ImplementMotorEncoders,
                                        public ImplementTorqueControl,
                                        public ImplementControlLimits2,
                                        public ImplementPositionDirect,
                                        public ImplementOpenLoopControl,
                                        public ImplementInteractionMode,
                                        public ImplementMotor,
                                        public ImplementAxisInfo
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

    yarp::os::Semaphore     _mutex;
    int  _njoints;
    int *_axisMap;                              /** axis remapping lookup-table */
    double *_angleToEncoder;                    /** angle to iCubDegrees conversion factors */
    double  *_encodersStamp;                    /** keep information about acquisition time for encoders read */
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
    Pid *_pids;                                 /** initial gains */
    Pid *_tpids;                                /** initial torque gains */
    Pid *_cpids;                                /** initial current gains */

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
    int * _filterType;                          /** the filter type (int value) used by the force control algorithm */
    int *_torqueSensorId;                       /** Id of associated Joint Torque Sensor */
    int *_torqueSensorChan;                     /** Channel of associated Joint Torque Sensor */
    double *_maxTorque;                         /** Max torque of a joint */
    double *_newtonsToSensor;                   /** Newtons to force sensor units conversion factors */
    bool  *checking_motiondone;                 /* flag telling if I'm already waiting for motion done */
    double *_last_position_move_time;           /** time stamp for last received position move command*/
    double *_motorPwmLimits;                    /** motors PWM limits*/

    // TODO doubled!!! optimize using just one of the 2!!!
//     ImpedanceParameters *_impedance_params;     /** impedance parameters */
//     eOmc_impedance_t *_cacheImpedance;          /* cache impedance value to split up the 2 sets */

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
    yarp::sig::Vector       current, nominalCurrent, maxCurrent, peakCurrent;
    yarp::sig::Vector       pwm, pwmLimit, supplyVoltage;
    yarp::sig::Vector pos, dpos, vel, speed, acc, loc, amp;
    double lifetime;
    bool opened;

    // debugging
    VerboseLevel verbose;
public:

    FakeMotionControl();
    ~FakeMotionControl();

  // Device Driver
    virtual bool open(yarp::os::Searchable &par);
    virtual bool close();
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

    bool init(void);

#ifndef YARP_NO_DEPRECATED // since YARP 2.3.65
    // Hide -Woverloaded-virtual warnings
    using yarp::dev::IPositionControlRaw::setPositionModeRaw;
    using yarp::dev::IVelocityControlRaw::setVelocityModeRaw;
    using yarp::dev::ITorqueControlRaw::setTorqueModeRaw;
    using yarp::dev::IOpenLoopControlRaw::setOpenLoopModeRaw;
#endif

    /////////   PID INTERFACE   /////////
    virtual bool setPidRaw(int j, const Pid &pid);
    virtual bool setPidsRaw(const Pid *pids);
    virtual bool setReferenceRaw(int j, double ref);
    virtual bool setReferencesRaw(const double *refs);
    virtual bool setErrorLimitRaw(int j, double limit);
    virtual bool setErrorLimitsRaw(const double *limits);
    virtual bool getErrorRaw(int j, double *err);
    virtual bool getErrorsRaw(double *errs);
//    virtual bool getOutputRaw(int j, double *out);    // uses iOpenLoop interface
//    virtual bool getOutputsRaw(double *outs);         // uses iOpenLoop interface
    virtual bool getPidRaw(int j, Pid *pid);
    virtual bool getPidsRaw(Pid *pids);
    virtual bool getReferenceRaw(int j, double *ref);
    virtual bool getReferencesRaw(double *refs);
    virtual bool getErrorLimitRaw(int j, double *limit);
    virtual bool getErrorLimitsRaw(double *limits);
    virtual bool resetPidRaw(int j);
    virtual bool disablePidRaw(int j);
    virtual bool enablePidRaw(int j);
    virtual bool setOffsetRaw(int j, double v);

    // POSITION CONTROL INTERFACE RAW
    virtual bool getAxes(int *ax);
    virtual bool positionMoveRaw(int j, double ref);
    virtual bool positionMoveRaw(const double *refs);
    virtual bool relativeMoveRaw(int j, double delta);
    virtual bool relativeMoveRaw(const double *deltas);
    virtual bool checkMotionDoneRaw(bool *flag);
    virtual bool checkMotionDoneRaw(int j, bool *flag);
    virtual bool setRefSpeedRaw(int j, double sp);
    virtual bool setRefSpeedsRaw(const double *spds);
    virtual bool setRefAccelerationRaw(int j, double acc);
    virtual bool setRefAccelerationsRaw(const double *accs);
    virtual bool getRefSpeedRaw(int j, double *ref);
    virtual bool getRefSpeedsRaw(double *spds);
    virtual bool getRefAccelerationRaw(int j, double *acc);
    virtual bool getRefAccelerationsRaw(double *accs);
    virtual bool stopRaw(int j);
    virtual bool stopRaw();

    // Position Control2 Interface
    virtual bool positionMoveRaw(const int n_joint, const int *joints, const double *refs);
    virtual bool relativeMoveRaw(const int n_joint, const int *joints, const double *deltas);
    virtual bool checkMotionDoneRaw(const int n_joint, const int *joints, bool *flags);
    virtual bool setRefSpeedsRaw(const int n_joint, const int *joints, const double *spds);
    virtual bool setRefAccelerationsRaw(const int n_joint, const int *joints, const double *accs);
    virtual bool getRefSpeedsRaw(const int n_joint, const int *joints, double *spds);
    virtual bool getRefAccelerationsRaw(const int n_joint, const int *joints, double *accs);
    virtual bool stopRaw(const int n_joint, const int *joints);
    virtual bool getTargetPositionRaw(const int joint, double *ref);
    virtual bool getTargetPositionsRaw(double *refs);
    virtual bool getTargetPositionsRaw(const int n_joint, const int *joints, double *refs);

    //  Velocity control interface raw
    virtual bool velocityMoveRaw(int j, double sp);
    virtual bool velocityMoveRaw(const double *sp);


    // calibration2raw
    virtual bool setCalibrationParametersRaw(int axis, const CalibrationParameters& params);
    virtual bool calibrate2Raw(int axis, unsigned int type, double p1, double p2, double p3);
    virtual bool doneRaw(int j);


    /////////////////////////////// END Position Control INTERFACE

    // ControlMode
    virtual bool setPositionModeRaw(int j);
    virtual bool setVelocityModeRaw(int j);
    virtual bool setTorqueModeRaw(int j);
    virtual bool setImpedancePositionModeRaw(int j);
    virtual bool setImpedanceVelocityModeRaw(int j);
    virtual bool setOpenLoopModeRaw(int j);
    virtual bool getControlModeRaw(int j, int *v);
    virtual bool getControlModesRaw(int *v);

    // ControlMode 2
    virtual bool getControlModesRaw(const int n_joint, const int *joints, int *modes);
    virtual bool setControlModeRaw(const int j, const int mode);
    virtual bool setControlModesRaw(const int n_joint, const int *joints, int *modes);
    virtual bool setControlModesRaw(int *modes);

    //////////////////////// BEGIN EncoderInterface
    virtual bool resetEncoderRaw(int j);
    virtual bool resetEncodersRaw();
    virtual bool setEncoderRaw(int j, double val);
    virtual bool setEncodersRaw(const double *vals);
    virtual bool getEncoderRaw(int j, double *v);
    virtual bool getEncodersRaw(double *encs);
    virtual bool getEncoderSpeedRaw(int j, double *sp);
    virtual bool getEncoderSpeedsRaw(double *spds);
    virtual bool getEncoderAccelerationRaw(int j, double *spds);
    virtual bool getEncoderAccelerationsRaw(double *accs);
    ///////////////////////// END Encoder Interface

    virtual bool getEncodersTimedRaw(double *encs, double *stamps);
    virtual bool getEncoderTimedRaw(int j, double *encs, double *stamp);

    //////////////////////// BEGIN MotorEncoderInterface
    virtual bool getNumberOfMotorEncodersRaw(int * num);
    virtual bool resetMotorEncoderRaw(int m);
    virtual bool resetMotorEncodersRaw();
    virtual bool setMotorEncoderRaw(int m, const double val);
    virtual bool setMotorEncodersRaw(const double *vals);
    virtual bool getMotorEncoderRaw(int m, double *v);
    virtual bool getMotorEncodersRaw(double *encs);
    virtual bool getMotorEncoderSpeedRaw(int m, double *sp);
    virtual bool getMotorEncoderSpeedsRaw(double *spds);
    virtual bool getMotorEncoderAccelerationRaw(int m, double *spds);
    virtual bool getMotorEncoderAccelerationsRaw(double *accs);
    virtual bool getMotorEncodersTimedRaw(double *encs, double *stamps);
    virtual bool getMotorEncoderTimedRaw(int m, double *encs, double *stamp);\
    virtual bool getMotorEncoderCountsPerRevolutionRaw(int m, double *v);
    virtual bool setMotorEncoderCountsPerRevolutionRaw(int m, const double cpr);
    ///////////////////////// END MotorEncoder Interface

    //////////////////////// BEGIN IAxisInfo Interface
    virtual bool getAxisNameRaw(int axis, yarp::os::ConstString& name);
    virtual bool getJointTypeRaw(int axis, yarp::dev::JointTypeEnum& type);
    ///////////////////////// END IAxisInfo Interface

    //Internal use, not exposed by Yarp (yet)
    virtual bool getGearboxRatioRaw(int m, double *gearbox);
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
    virtual bool getCurrentPidRaw(int j, Pid *pid);
    virtual bool getTorqueControlFilterType(int j, int& type);

    ////// Amplifier interface
    virtual bool enableAmpRaw(int j);
    virtual bool disableAmpRaw(int j);
    virtual bool getCurrentsRaw(double *vals);
    virtual bool getCurrentRaw(int j, double *val);
    virtual bool getNominalCurrentRaw(int m, double *val);
    virtual bool setMaxCurrentRaw(int j, double val);
    virtual bool getMaxCurrentRaw(int j, double *val);
    virtual bool getPeakCurrentRaw(int m, double *val);
    virtual bool setPeakCurrentRaw(int m, const double val);
    virtual bool getAmpStatusRaw(int *st);
    virtual bool getAmpStatusRaw(int j, int *st);
    virtual bool getPWMRaw(int j, double* val);
    virtual bool getPWMLimitRaw(int j, double* val);
    virtual bool setPWMLimitRaw(int j, const double val);
    virtual bool getPowerSupplyVoltageRaw(int j, double* val);
    /////////////// END AMPLIFIER INTERFACE

    // Limits
    virtual bool setLimitsRaw(int axis, double min, double max);
    virtual bool getLimitsRaw(int axis, double *min, double *max);
    // Limits 2
    virtual bool setVelLimitsRaw(int axis, double min, double max);
    virtual bool getVelLimitsRaw(int axis, double *min, double *max);

    // Torque control
    virtual bool getTorqueRaw(int j, double *t);
    virtual bool getTorquesRaw(double *t);
    virtual bool getBemfParamRaw(int j, double *bemf);
    virtual bool setBemfParamRaw(int j, double bemf);
    virtual bool getTorqueRangeRaw(int j, double *min, double *max);
    virtual bool getTorqueRangesRaw(double *min, double *max);
    virtual bool setRefTorquesRaw(const double *t);
    virtual bool setRefTorqueRaw(int j, double t);
    virtual bool setRefTorquesRaw(const int n_joint, const int *joints, const double *t);
    virtual bool getRefTorquesRaw(double *t);
    virtual bool getRefTorqueRaw(int j, double *t);
    virtual bool setTorquePidRaw(int j, const Pid &pid);
    virtual bool setTorquePidsRaw(const Pid *pids);
    virtual bool setTorqueErrorLimitRaw(int j, double limit);
    virtual bool setTorqueErrorLimitsRaw(const double *limits);
    virtual bool getTorqueErrorRaw(int j, double *err);
    virtual bool getTorqueErrorsRaw(double *errs);
    virtual bool getTorquePidOutputRaw(int j, double *out);
    virtual bool getTorquePidOutputsRaw(double *outs);
    virtual bool getTorquePidRaw(int j, Pid *pid);
    virtual bool getTorquePidsRaw(Pid *pids);
    virtual bool getTorqueErrorLimitRaw(int j, double *limit);
    virtual bool getTorqueErrorLimitsRaw(double *limits);
    virtual bool resetTorquePidRaw(int j);
    virtual bool disableTorquePidRaw(int j);
    virtual bool enableTorquePidRaw(int j);
    virtual bool setTorqueOffsetRaw(int j, double v);
    virtual bool getMotorTorqueParamsRaw(int j, MotorTorqueParameters *params);
    virtual bool setMotorTorqueParamsRaw(int j, const MotorTorqueParameters params);
//     int32_t getRefSpeedInTbl(uint8_t boardNum, int j, eOmeas_position_t pos);

    // IVelocityControl2
    virtual bool velocityMoveRaw(const int n_joint, const int *joints, const double *spds);
    virtual bool setVelPidRaw(int j, const Pid &pid);
    virtual bool setVelPidsRaw(const Pid *pids);
    virtual bool getVelPidRaw(int j, Pid *pid);
    virtual bool getVelPidsRaw(Pid *pids);
    virtual bool getRefVelocityRaw(const int joint, double *ref);
    virtual bool getRefVelocitiesRaw(double *refs);
    virtual bool getRefVelocitiesRaw(const int n_joint, const int *joints, double *refs);

    // Impedance interface
    virtual bool getImpedanceRaw(int j, double *stiffness, double *damping);
    virtual bool setImpedanceRaw(int j, double stiffness, double damping);
    virtual bool setImpedanceOffsetRaw(int j, double offset);
    virtual bool getImpedanceOffsetRaw(int j, double *offset);
    virtual bool getCurrentImpedanceLimitRaw(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp);

    // PositionDirect Interface
    virtual bool setPositionRaw(int j, double ref);
    virtual bool setPositionsRaw(const int n_joint, const int *joints, double *refs);
    virtual bool setPositionsRaw(const double *refs);
    virtual bool getRefPositionRaw(const int joint, double *ref);
    virtual bool getRefPositionsRaw(double *refs);
    virtual bool getRefPositionsRaw(const int n_joint, const int *joints, double *refs);

    // InteractionMode interface
    virtual bool getInteractionModeRaw(int j, yarp::dev::InteractionModeEnum* _mode);
    virtual bool getInteractionModesRaw(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes);
    virtual bool getInteractionModesRaw(yarp::dev::InteractionModeEnum* modes);
    virtual bool setInteractionModeRaw(int j, yarp::dev::InteractionModeEnum _mode);
    virtual bool setInteractionModesRaw(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes);
    virtual bool setInteractionModesRaw(yarp::dev::InteractionModeEnum* modes);

    // IMotor interface
    virtual bool getNumberOfMotorsRaw(int * num);
    virtual bool getTemperatureRaw(int m, double* val);
    virtual bool getTemperaturesRaw(double *vals);
    virtual bool getTemperatureLimitRaw(int m, double *temp);
    virtual bool setTemperatureLimitRaw(int m, const double temp);

    // OPENLOOP interface
    virtual bool setRefOutputRaw(int j, double v);
    virtual bool setRefOutputsRaw(const double *v);
    virtual bool getRefOutputRaw(int j, double *out);
    virtual bool getRefOutputsRaw(double *outs);
    virtual bool getOutputRaw(int j, double *out);
    virtual bool getOutputsRaw(double *outs);
    void run();
private:
    void cleanup(void);
    bool dealloc();
    bool parsePositionPidsGroup(yarp::os::Bottle& pidsGroup, Pid myPid[]);
    bool parseTorquePidsGroup(yarp::os::Bottle& pidsGroup, Pid myPid[], double kbemf[], double ktau[], int filterType[]);
    bool parseImpedanceGroup_NewFormat(yarp::os::Bottle& pidsGroup, ImpedanceParameters vals[]);

    bool extractGroup(yarp::os::Bottle &input, yarp::os::Bottle &out, const std::string &key1, const std::string &txt, int size);
};

#endif  // YARP_DEVICE_FAKE_MOTIONCONTROL
