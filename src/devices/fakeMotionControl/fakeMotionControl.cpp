/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Alberto Cardellino
 * email:   alberto.cardellino@iit.it
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <iostream>
#include <sstream>
#include <cstring>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/NetType.h>
#include <yarp/dev/Drivers.h>

#include "fakeMotionControl.h"

using namespace std;
using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::os::impl;


// macros
#define NEW_JSTATUS_STRUCT 1

void FakeMotionControl::run() {
    if (lifetime>=0) {
        Time::delay(lifetime);   // fake device shall run on mutable clock
        std::exit(0);
    }
}

static inline bool NOT_YET_IMPLEMENTED(const char *txt)
{
    yError() << txt << " is not yet implemented for FakeMotionControl";
    return true;
}

static inline bool DEPRECATED(const char *txt)
{
    yError() << txt << " has been deprecated for FakeMotionControl";
    return true;
}


// replace with to_string as soon as C++11 is required by YARP
/**
 * @brief convert an arbitary type to string.
 *
 */
template<typename T>
std::string toString(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

yarp::dev::DriverCreator *createFakeMotionControl()
{
    return new yarp::dev::DriverCreatorOf<yarp::dev::FakeMotionControl>
            ("fakeMotionControl", "controlboardwrapper2", "yarp::dev::FakeMotionControl");
}

//generic function that check is key1 is present in input bottle and that the result has size elements
// return true/false
bool FakeMotionControl::extractGroup(Bottle &input, Bottle &out, const std::string &key1, const std::string &txt, int size)
{
    size++;
    Bottle &tmp=input.findGroup(key1.c_str(), txt.c_str());

    if (tmp.isNull())
    {
        yError() << key1.c_str() << " parameter not found";
        return false;
    }

    if(tmp.size()!=size)
    {
        yError() << key1.c_str() << " incorrect number of entries";
        return false;
    }

    out=tmp;
    return true;
}

void FakeMotionControl::resizeBuffers()
{
    pos.resize(_njoints);
    dpos.resize(_njoints);
    vel.resize(_njoints);
    speed.resize(_njoints);
    acc.resize(_njoints);
    loc.resize(_njoints);
    amp.resize(_njoints);

    current.resize(_njoints);
    nominalCurrent.resize(_njoints);
    maxCurrent.resize(_njoints);
    peakCurrent.resize(_njoints);
    pwm.resize(_njoints);
    refpwm.resize(_njoints);
    pwmLimit.resize(_njoints);
    supplyVoltage.resize(_njoints);

    pos.zero();
    dpos.zero();
    vel.zero();
    speed.zero();
    acc.zero();
    loc.zero();
    amp.zero();

    current.zero();
    nominalCurrent.zero();
    maxCurrent.zero();
    peakCurrent.zero();

    pwm.zero();
    refpwm.zero();
    pwmLimit.zero();
    supplyVoltage.zero();
}

bool FakeMotionControl::alloc(int nj)
{
    _axisMap = allocAndCheck<int>(nj);
    _controlModes = allocAndCheck<int>(nj);
    _interactMode = allocAndCheck<int>(nj);
    _angleToEncoder = allocAndCheck<double>(nj);
    _dutycycleToPWM = allocAndCheck<double>(nj);
    _ampsToSensor = allocAndCheck<double>(nj);
    _encodersStamp = allocAndCheck<double>(nj);
    _DEPRECATED_encoderconversionoffset = allocAndCheck<float>(nj);
    _DEPRECATED_encoderconversionfactor = allocAndCheck<float>(nj);
//     _jointEncoderType = allocAndCheck<uint8_t>(nj);
//     _rotorEncoderType = allocAndCheck<uint8_t>(nj);
    _jointEncoderRes = allocAndCheck<int>(nj);
    _rotorEncoderRes = allocAndCheck<int>(nj);
    _gearbox = allocAndCheck<double>(nj);
    _torqueSensorId= allocAndCheck<int>(nj);
    _torqueSensorChan= allocAndCheck<int>(nj);
    _maxTorque=allocAndCheck<double>(nj);
    _maxJntCmdVelocity = allocAndCheck<double>(nj);
    _maxMotorVelocity = allocAndCheck<double>(nj);
    _newtonsToSensor=allocAndCheck<double>(nj);
    _hasHallSensor = allocAndCheck<bool>(nj);
    _hasTempSensor = allocAndCheck<bool>(nj);
    _hasRotorEncoder = allocAndCheck<bool>(nj);
    _hasRotorEncoderIndex = allocAndCheck<bool>(nj);
    _rotorIndexOffset = allocAndCheck<int>(nj);
    _motorPoles = allocAndCheck<int>(nj);
    _rotorlimits_max = allocAndCheck<double>(nj);
    _rotorlimits_min = allocAndCheck<double>(nj);

    _ppids=allocAndCheck<Pid>(nj);
    _tpids=allocAndCheck<Pid>(nj);
    _cpids = allocAndCheck<Pid>(nj);
    _vpids = allocAndCheck<Pid>(nj);
    _ppids_ena=allocAndCheck<bool>(nj);
    _tpids_ena=allocAndCheck<bool>(nj);
    _cpids_ena = allocAndCheck<bool>(nj);
    _vpids_ena = allocAndCheck<bool>(nj);
    _ppids_lim=allocAndCheck<double>(nj);
    _tpids_lim=allocAndCheck<double>(nj);
    _cpids_lim = allocAndCheck<double>(nj);
    _vpids_lim = allocAndCheck<double>(nj);
    _ppids_ref=allocAndCheck<double>(nj);
    _tpids_ref=allocAndCheck<double>(nj);
    _cpids_ref = allocAndCheck<double>(nj);
    _vpids_ref = allocAndCheck<double>(nj);

//     _impedance_params=allocAndCheck<ImpedanceParameters>(nj);
//     _impedance_limits=allocAndCheck<ImpedanceLimits>(nj);
    _axisName = new string[nj];
    _jointType = new JointTypeEnum[nj];

    _limitsMax=allocAndCheck<double>(nj);
    _limitsMin=allocAndCheck<double>(nj);
    _kinematic_mj=allocAndCheck<double>(16);
//     _currentLimits=allocAndCheck<MotorCurrentLimits>(nj);
    _motorPwmLimits=allocAndCheck<double>(nj);
    checking_motiondone=allocAndCheck<bool>(nj);

    _velocityShifts=allocAndCheck<int>(nj);
    _velocityTimeout=allocAndCheck<int>(nj);
    _kbemf=allocAndCheck<double>(nj);
    _ktau=allocAndCheck<double>(nj);
    _filterType=allocAndCheck<int>(nj);
    _last_position_move_time=allocAndCheck<double>(nj);

    // Reserve space for data stored locally. values are initialized to 0
    _posCtrl_references = allocAndCheck<double>(nj);
    _posDir_references = allocAndCheck<double>(nj);
    _command_speeds = allocAndCheck<double>(nj);
    _ref_speeds = allocAndCheck<double>(nj);
    _ref_accs = allocAndCheck<double>(nj);
    _ref_torques = allocAndCheck<double>(nj);
    _ref_currents = allocAndCheck<double>(nj);
    _enabledAmp = allocAndCheck<bool>(nj);
    _enabledPid = allocAndCheck<bool>(nj);
    _calibrated = allocAndCheck<bool>(nj);
//     _cacheImpedance = allocAndCheck<eOmc_impedance_t>(nj);

    resizeBuffers();

    return true;
}

bool FakeMotionControl::dealloc()
{
    checkAndDestroy(_axisMap);
    checkAndDestroy(_controlModes);
    checkAndDestroy(_interactMode);
    checkAndDestroy(_angleToEncoder);
    checkAndDestroy(_ampsToSensor);
    checkAndDestroy(_dutycycleToPWM);
    checkAndDestroy(_encodersStamp);
    checkAndDestroy(_DEPRECATED_encoderconversionoffset);
    checkAndDestroy(_DEPRECATED_encoderconversionfactor);
    checkAndDestroy(_jointEncoderRes);
    checkAndDestroy(_rotorEncoderRes);
//     checkAndDestroy(_jointEncoderType);
//     checkAndDestroy(_rotorEncoderType);
    checkAndDestroy(_gearbox);
    checkAndDestroy(_torqueSensorId);
    checkAndDestroy(_torqueSensorChan);
    checkAndDestroy(_maxTorque);
    checkAndDestroy(_maxJntCmdVelocity);
    checkAndDestroy(_maxMotorVelocity);
    checkAndDestroy(_newtonsToSensor);
    checkAndDestroy(_ppids);
    checkAndDestroy(_tpids);
    checkAndDestroy(_cpids);
    checkAndDestroy(_vpids);
    checkAndDestroy(_ppids_ena);
    checkAndDestroy(_tpids_ena);
    checkAndDestroy(_cpids_ena);
    checkAndDestroy(_vpids_ena);
    checkAndDestroy(_ppids_lim);
    checkAndDestroy(_tpids_lim);
    checkAndDestroy(_cpids_lim);
    checkAndDestroy(_vpids_lim);
    checkAndDestroy(_ppids_ref);
    checkAndDestroy(_tpids_ref);
    checkAndDestroy(_cpids_ref);
    checkAndDestroy(_vpids_ref);
//     checkAndDestroy(_impedance_params);
//     checkAndDestroy(_impedance_limits);
    checkAndDestroy(_limitsMax);
    checkAndDestroy(_limitsMin);
    checkAndDestroy(_kinematic_mj);
//     checkAndDestroy(_currentLimits);
    checkAndDestroy(_motorPwmLimits);
    checkAndDestroy(checking_motiondone);
    checkAndDestroy(_velocityShifts);
    checkAndDestroy(_velocityTimeout);
    checkAndDestroy(_kbemf);
    checkAndDestroy(_ktau);
    checkAndDestroy(_filterType);
    checkAndDestroy(_posCtrl_references);
    checkAndDestroy(_posDir_references);
    checkAndDestroy(_command_speeds);
    checkAndDestroy(_ref_speeds);
    checkAndDestroy(_ref_accs);
    checkAndDestroy(_ref_torques);
    checkAndDestroy(_ref_currents);
    checkAndDestroy(_enabledAmp);
    checkAndDestroy(_enabledPid);
    checkAndDestroy(_calibrated);
    checkAndDestroy(_hasHallSensor);
    checkAndDestroy(_hasTempSensor);
    checkAndDestroy(_hasRotorEncoder);
    checkAndDestroy(_hasRotorEncoderIndex);
    checkAndDestroy(_rotorIndexOffset);
    checkAndDestroy(_motorPoles);
    checkAndDestroy(_axisName);
    checkAndDestroy(_jointType);
    checkAndDestroy(_rotorlimits_max);
    checkAndDestroy(_rotorlimits_min);
    checkAndDestroy(_last_position_move_time);

    return true;
}

FakeMotionControl::FakeMotionControl() :
    ImplementControlCalibration2<FakeMotionControl, IControlCalibration2>(this),
    ImplementAmplifierControl<FakeMotionControl, IAmplifierControl>(this),
    ImplementPidControl(this),
    ImplementEncodersTimed(this),
    ImplementPositionControl2(this),
    ImplementVelocityControl<FakeMotionControl, IVelocityControl>(this),
    ImplementVelocityControl2(this),
    ImplementControlMode2(this),
    ImplementImpedanceControl(this),
    ImplementMotorEncoders(this),
    ImplementTorqueControl(this),
    ImplementControlLimits2(this),
    ImplementPositionDirect(this),
    ImplementInteractionMode(this),
    ImplementCurrentControl(this),
    ImplementPWMControl(this),
    ImplementMotor(this),
    ImplementAxisInfo(this),
    _mutex(1)
//     SAFETY_THRESHOLD(2.0)
{
    verbose = VERY_VERBOSE;
    _njoints = 2;
    opened = false;

    resizeBuffers();

    _controlModes = NULL;
    _interactMode = NULL;

    lifetime = -1;
    init();

    _gearbox       = 0;
//     opened        = 0;
    _ppids         = NULL;
    _tpids        = NULL;
    _cpids        = NULL;
    _vpids        = NULL;
    _ppids_ena         = NULL;
    _tpids_ena        = NULL;
    _cpids_ena        = NULL;
    _vpids_ena        = NULL;
    _ppids_lim         = NULL;
    _tpids_lim        = NULL;
    _cpids_lim        = NULL;
    _vpids_lim        = NULL;
    _ppids_ref         = NULL;
    _tpids_ref        = NULL;
    _cpids_ref        = NULL;
    _vpids_ref        = NULL;
    _njoints      = 0;
    _axisMap      = NULL;
    _encodersStamp = NULL;
    _DEPRECATED_encoderconversionfactor = NULL;
    _DEPRECATED_encoderconversionoffset = NULL;
    _angleToEncoder = NULL;
    _dutycycleToPWM = NULL;
    _ampsToSensor = NULL;
    _hasHallSensor = NULL;
    _hasTempSensor = NULL;
    _hasRotorEncoder = NULL;
    _hasRotorEncoderIndex = NULL;
    _rotorIndexOffset = NULL;
    _motorPoles       = NULL;
//     _impedance_params = NULL;
//     _impedance_limits = NULL;
    _rotorlimits_max  = NULL;
    _rotorlimits_min  = NULL;

    _axisName         = NULL;
    _jointType         = NULL;
    _limitsMin        = NULL;
    _limitsMax        = NULL;
//     _currentLimits    = NULL;
    _motorPwmLimits   = NULL;
    _velocityShifts   = NULL;
    _velocityTimeout  = NULL;
    _torqueSensorId   = NULL;
    _torqueSensorChan = NULL;
    _maxTorque        = NULL;
    _maxJntCmdVelocity= NULL;
    _maxMotorVelocity = NULL;
    _newtonsToSensor  = NULL;
    _jointEncoderRes  = NULL;
//     _jointEncoderType = NULL;
    _rotorEncoderRes  = NULL;
//     _rotorEncoderType = NULL;
    _ref_accs         = NULL;
    _command_speeds   = NULL;
    _posCtrl_references    = NULL;
    _posDir_references    = NULL;
    _ref_speeds       = NULL;
    _ref_torques      = NULL;
    _ref_currents     = NULL;
    _kinematic_mj     = NULL;
    _kbemf            = NULL;
    _ktau             = NULL;
    _filterType       = NULL;
    _positionControlUnits = P_MACHINE_UNITS;
    _torqueControlUnits = T_MACHINE_UNITS;
    _torqueControlEnabled = false;

    checking_motiondone = NULL;

    // Check status of joints
    _enabledPid       = NULL;
    _enabledAmp       = NULL;
    _calibrated       = NULL;
    _last_position_move_time = NULL;
    // NV stuff

    useRawEncoderData = false;
    _pwmIsLimited     = false;

    ConstString tmp = NetworkBase::getEnvironment("VERBOSE_STICA");
    if (tmp != "")
    {
        verbosewhenok = (bool)NetType::toInt(tmp);
    }
    else
    {
        verbosewhenok = false;
    }

}

FakeMotionControl::~FakeMotionControl()
{
    yTrace();
    dealloc();
}

bool FakeMotionControl::initialised()
{
    return opened;
}


bool FakeMotionControl::open(yarp::os::Searchable &config)
{
    std::string str;

//     if (!config.findGroup("GENERAL").find("MotioncontrolVersion").isInt())
//     {
//         yError() << "Missing MotioncontrolVersion parameter. yarprobotinterface cannot start. Please contact icub-support@iit.it";
//         return false;
//     }
//     else
//     {
//         int mcv = config.findGroup("GENERAL").find("MotioncontrolVersion").asInt();
//         if (mcv != 2)
//         {
//             yError() << "Wrong MotioncontrolVersion parameter. yarprobotinterface cannot start. Please contact icub-support@iit.it";
//             return false;
//         }
//     }

//     if(!config.findGroup("GENERAL").find("verbose").isBool())
//     {
//         yError() << "FakeMotionControl::open() detects that general->verbose bool param is different from accepted values (true / false). Assuming false";
//         str=" ";
//     }
//     else
//     {
//         if(config.findGroup("GENERAL").find("verbose").asBool())
//             str=config.toString().c_str();
//         else
//             str=" ";
//     }
    str=config.toString().c_str();
    yTrace() << str;

    //
    //  Read Configuration params from file
    //
    _njoints = config.findGroup("GENERAL").check("Joints",Value(1),   "Number of degrees of freedom").asInt();

    if(!alloc(_njoints))
    {
        yError() << "Malloc failed";
        return false;
    }

    // Default value
    for(int i=0; i<_njoints; i++)
        _newtonsToSensor[i] = 1;

    if(!fromConfig(config))
    {
        yError() << "Missing parameters in config file";
        return false;
    }

    //  INIT ALL INTERFACES
    yarp::sig::Vector tmpZeros; tmpZeros.resize (_njoints, 0.0);
    yarp::sig::Vector tmpOnes;  tmpOnes.resize  (_njoints, 1.0);

    ImplementControlCalibration2<FakeMotionControl, IControlCalibration2>::initialize(_njoints, _axisMap, _angleToEncoder, NULL);
    ImplementAmplifierControl<FakeMotionControl, IAmplifierControl>::initialize(_njoints, _axisMap, _angleToEncoder, NULL);
    ImplementEncodersTimed::initialize(_njoints, _axisMap, _angleToEncoder, NULL);
    ImplementMotorEncoders::initialize(_njoints, _axisMap, _angleToEncoder, NULL);
    ImplementPositionControl2::initialize(_njoints, _axisMap, _angleToEncoder, NULL);
    ImplementPidControl::initialize(_njoints, _axisMap, _angleToEncoder, NULL, _newtonsToSensor, _ampsToSensor);
    ImplementControlMode2::initialize(_njoints, _axisMap);
    ImplementVelocityControl<FakeMotionControl, IVelocityControl>::initialize(_njoints, _axisMap, _angleToEncoder, NULL);
    ImplementVelocityControl2::initialize(_njoints, _axisMap, _angleToEncoder, NULL);
    ImplementControlLimits2::initialize(_njoints, _axisMap, _angleToEncoder, NULL);
    ImplementImpedanceControl::initialize(_njoints, _axisMap, _angleToEncoder, NULL, _newtonsToSensor);
    ImplementTorqueControl::initialize(_njoints, _axisMap, _angleToEncoder, NULL, _newtonsToSensor);
    ImplementPositionDirect::initialize(_njoints, _axisMap, _angleToEncoder, NULL);
    ImplementInteractionMode::initialize(_njoints, _axisMap, _angleToEncoder, NULL);
    ImplementMotor::initialize(_njoints, _axisMap);
    ImplementAxisInfo::initialize(_njoints, _axisMap);
    ImplementPWMControl::initialize(_njoints, _axisMap, _dutycycleToPWM);
    ImplementCurrentControl::initialize(_njoints, _axisMap, _ampsToSensor);

    if(!init() )
    {
        yError() << "FakeMotionControl::open() has an error in call of FakeMotionControl::init() for board" ;
        return false;
    }
    else
    {
        if(verbosewhenok)
        {
            yDebug() << "FakeMotionControl::init() has successfully initted board ";
        }
    }

    opened = true;
    return true;
}

bool FakeMotionControl::parseImpedanceGroup_NewFormat(Bottle& pidsGroup, ImpedanceParameters vals[])
{
    int j=0;
    Bottle xtmp;

    if (!extractGroup(pidsGroup, xtmp, "stiffness", "stiffness parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        vals[j].stiffness = xtmp.get(j+1).asDouble();
    }

    if (!extractGroup(pidsGroup, xtmp, "damping", "damping parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        vals[j].damping = xtmp.get(j+1).asDouble();
    }

    return true;
}

bool FakeMotionControl::parsePositionPidsGroup(Bottle& pidsGroup, Pid myPid[])
{
    int j=0;
    Bottle xtmp;

    if (!extractGroup(pidsGroup, xtmp, "kp", "Pid kp parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].kp = xtmp.get(j+1).asDouble();
    }

    if (!extractGroup(pidsGroup, xtmp, "kd", "Pid kd parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].kd = xtmp.get(j+1).asDouble();
    }

    if (!extractGroup(pidsGroup, xtmp, "ki", "Pid kp parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].ki = xtmp.get(j+1).asDouble();
    }

    if (!extractGroup(pidsGroup, xtmp, "maxInt", "Pid maxInt parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].max_int = xtmp.get(j+1).asDouble();
    }

    if (!extractGroup(pidsGroup, xtmp, "maxPwm", "Pid maxPwm parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].max_output = xtmp.get(j+1).asDouble();
    }

    if (!extractGroup(pidsGroup, xtmp, "shift", "Pid shift parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].scale = xtmp.get(j+1).asDouble();
    }

    if (!extractGroup(pidsGroup, xtmp, "ko", "Pid ko parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].offset = xtmp.get(j+1).asDouble();
    }

    if (!extractGroup(pidsGroup, xtmp, "stictionUp", "Pid stictionUp", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].stiction_up_val = xtmp.get(j+1).asDouble();
    }

    if (!extractGroup(pidsGroup, xtmp, "stictionDwn", "Pid stictionDwn", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].stiction_down_val = xtmp.get(j+1).asDouble();
    }

    if (!extractGroup(pidsGroup, xtmp, "kff", "Pid kff parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].kff = xtmp.get(j+1).asDouble();
    }

    //conversion from metric to machine units (if applicable)
    if (_positionControlUnits==P_METRIC_UNITS)
    {
        for (j=0; j<_njoints; j++)
        {
            myPid[j].kp = myPid[j].kp / _angleToEncoder[j];  //[PWM/deg]
            myPid[j].ki = myPid[j].ki / _angleToEncoder[j];  //[PWM/deg]
            myPid[j].kd = myPid[j].kd / _angleToEncoder[j];  //[PWM/deg]
        }
    }
    else
    {
        //do nothing
    }

    //optional PWM limit
    if(_pwmIsLimited)
    {   // check for value in the file
        if (!extractGroup(pidsGroup, xtmp, "limPwm", "Limited PWD", _njoints))
        {
            yError() << "The PID parameter limPwm was requested but was not correctly set in the configuration file, please fill it.";
            return false;
        }

        yInfo() << "FakeMotionControl using LIMITED PWM!!";
        for (j=0; j<_njoints; j++)
            myPid[j].max_output = xtmp.get(j+1).asDouble();
    }

    return true;
}

bool FakeMotionControl::parseTorquePidsGroup(Bottle& pidsGroup, Pid myPid[], double kbemf[], double ktau[], int filterType[])
{
    int j=0;
    Bottle xtmp;
    if (!extractGroup(pidsGroup, xtmp, "kp", "Pid kp parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].kp = xtmp.get(j+1).asDouble();
    }

    if (!extractGroup(pidsGroup, xtmp, "kd", "Pid kd parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].kd = xtmp.get(j+1).asDouble();
    }

    if (!extractGroup(pidsGroup, xtmp, "ki", "Pid kp parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].ki = xtmp.get(j+1).asDouble();
    }

    if (!extractGroup(pidsGroup, xtmp, "maxInt", "Pid maxInt parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].max_int = xtmp.get(j+1).asDouble();
    }

    if (!extractGroup(pidsGroup, xtmp, "maxPwm", "Pid maxPwm parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].max_output = xtmp.get(j+1).asDouble();
    }

    if (!extractGroup(pidsGroup, xtmp, "shift", "Pid shift parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].scale = xtmp.get(j+1).asDouble();
    }

    if (!extractGroup(pidsGroup, xtmp, "ko", "Pid ko parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].offset = xtmp.get(j+1).asDouble();
    }

    if (!extractGroup(pidsGroup, xtmp, "stictionUp", "Pid stictionUp", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].stiction_up_val = xtmp.get(j+1).asDouble();
    }

    if (!extractGroup(pidsGroup, xtmp, "stictionDwn", "Pid stictionDwn", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].stiction_down_val = xtmp.get(j+1).asDouble();
    }

    if (!extractGroup(pidsGroup, xtmp, "kff",   "Pid kff parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].kff = xtmp.get(j+1).asDouble();
    }

    if (!extractGroup(pidsGroup, xtmp, "kbemf", "kbemf parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        kbemf[j] = xtmp.get(j+1).asDouble();
    }

    if (!extractGroup(pidsGroup, xtmp, "ktau", "ktau parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        ktau[j] = xtmp.get(j+1).asDouble();
    }

    if (!extractGroup(pidsGroup, xtmp, "filterType", "filterType param", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        filterType[j] = xtmp.get(j+1).asInt();
    }

    //conversion from metric to machine units (if applicable)
//     for (j=0; j<_njoints; j++)
//     {
//         myPid[j].kp = myPid[j].kp / _torqueControlHelper->getNewtonsToSensor(j);  //[PWM/Nm]
//         myPid[j].ki = myPid[j].ki / _torqueControlHelper->getNewtonsToSensor(j);  //[PWM/Nm]
//         myPid[j].kd = myPid[j].kd / _torqueControlHelper->getNewtonsToSensor(j);  //[PWM/Nm]
//         myPid[j].stiction_up_val   = myPid[j].stiction_up_val   * _torqueControlHelper->getNewtonsToSensor(j); //[Nm]
//         myPid[j].stiction_down_val = myPid[j].stiction_down_val * _torqueControlHelper->getNewtonsToSensor(j); //[Nm]
//     }

    //optional PWM limit
    if(_pwmIsLimited)
    {   // check for value in the file
        if (!extractGroup(pidsGroup, xtmp, "limPwm", "Limited PWD", _njoints))
        {
            yError() << "The PID parameter limPwm was requested but was not correctly set in the configuration file, please fill it.";
            return false;
        }

        yInfo() << "FakeMotionControl using LIMITED PWM!!";
        for (j=0; j<_njoints; j++) myPid[j].max_output = xtmp.get(j+1).asDouble();
    }

    return true;
}

bool FakeMotionControl::fromConfig(yarp::os::Searchable &config)
{
    Bottle xtmp;
    int i;
    Bottle general = config.findGroup("GENERAL");

    // read AxisMap values from file
    if(general.check("AxisMap"))
    {
        if(extractGroup(general, xtmp, "AxisMap", "a list of reordered indices for the axes", _njoints))
        {
            for (i = 1; i < xtmp.size(); i++)
                _axisMap[i - 1] = xtmp.get(i).asInt();
        }
        else
            return false;
    }
    else
    {
        yInfo() << "FakeMotionControl: Using default AxisMap";
        for (i = 0; i < _njoints; i++)
            _axisMap[i] = i;
    }

    if(general.check("AxisName"))
    {
        if (extractGroup(general, xtmp, "AxisName", "a list of strings representing the axes names", _njoints))
        {
            //beware: axis name has to be remapped here because they are not set using the toHw() helper function
            for (i = 1; i < xtmp.size(); i++)
            {
                _axisName[_axisMap[i - 1]] = xtmp.get(i).asString();
            }
        }
        else
            return false;
    }
    else
    {
        yInfo() << "FakeMotionControl: Using default AxisName";
        for (i = 0; i < _njoints; i++)
        {
            _axisName[_axisMap[i]] = "joint" + toString(i);
        }
    }
    if(general.check("AxisType"))
    {
        if (extractGroup(general, xtmp, "AxisType", "a list of strings representing the axes type (revolute/prismatic)", _njoints))
        {
            //beware: axis type has to be remapped here because they are not set using the toHw() helper function
            for (i = 1; i < xtmp.size(); i++)
            {
                string typeString = xtmp.get(i).asString();
                if (typeString == "revolute")  _jointType[_axisMap[i - 1]] = VOCAB_JOINTTYPE_REVOLUTE;
                else if (typeString == "prismatic")  _jointType[_axisMap[i - 1]] = VOCAB_JOINTTYPE_PRISMATIC;
                else
                {
                    yError("Unknown AxisType value %s!", typeString.c_str());
                    _jointType[_axisMap[i - 1]] = VOCAB_JOINTTYPE_UNKNOWN;
                    return false;
                }
            }
        }
        else
            return false;
    }
    else
    {
        yInfo() << "FakeMotionControl: Using default AxisType (revolute)";
        for (i = 0; i < _njoints; i++)
        {
            _jointType[_axisMap[i]] = VOCAB_JOINTTYPE_REVOLUTE;
        }
    }

    // current conversions factor
    if (general.check("ampsToSensor"))
    {
        if (extractGroup(general, xtmp, "ampsToSensor", "a list of scales for the ampsToSensor conversion factors", _njoints))
        {
            for (i = 1; i < xtmp.size(); i++)
            {
                if (xtmp.get(i).isDouble())
                {
                    _ampsToSensor[i - 1] = xtmp.get(i).asDouble();
                }
            }
        }
        else
            return false;
    }
    else
    {
        yInfo() << "Using default ampsToSensor";
        for (i = 0; i < _njoints; i++)
        {
            _ampsToSensor[i] = 1.0;
        }
    }

    // pwm conversions factor
    if (general.check("dutycycleToPWM"))
    {
        if (extractGroup(general, xtmp, "dutycycleToPWM", "a list of scales for the dutycycleToPWM conversion factors", _njoints))
        {
            for (i = 1; i < xtmp.size(); i++)
            {
                if (xtmp.get(i).isDouble())
                {
                    _dutycycleToPWM[i - 1] = xtmp.get(i).asDouble();
                }
            }
        }
        else
            return false;
    }
    else
    {
        yInfo() << "Using default ampsToSensor";
        for (i = 0; i < _njoints; i++)
            _dutycycleToPWM[i] = 1.0;
    }

//     double tmp_A2E;
    // Encoder scales
    if(general.check("Encoder"))
    {
        if (extractGroup(general, xtmp, "Encoder", "a list of scales for the encoders", _njoints))
        {
            for (i = 1; i < xtmp.size(); i++)
            {
                _angleToEncoder[i-1] = xtmp.get(i).asDouble();
            }
        }
        else
            return false;
    }
    else
    {
        yInfo() << "Using default Encoder";
        for (i = 0; i < _njoints; i++)
            _angleToEncoder[i] = 1;
    }

    // Joint encoder resolution
    /*if (!extractGroup(general, xtmp, "JointEncoderRes", "the resolution of the joint encoder", _njoints))
    {
        return false;
    }
    else
    {
        int test = xtmp.size();
        for (i = 1; i < xtmp.size(); i++)
            _jointEncoderRes[i - 1] = xtmp.get(i).asInt();
    }

    // Joint encoder type
    if (!extractGroup(general, xtmp, "JointEncoderType", "JointEncoderType", _njoints))
    {
        return false;
    }
    else
    {
        int test = xtmp.size();
        for (i = 1; i < xtmp.size(); i++)
        {
            uint8_t val;
            string s = xtmp.get(i).asString();
            bool b = EncoderType_iCub2eo(&s, &val);
            if (b == false)
            {
                yError("Invalid JointEncoderType: %s!", s.c_str()); return false;
            }
//             _jointEncoderType[i - 1] = val;
        }
    }
*/

    // Motor capabilities
/*    if (!extractGroup(general, xtmp, "HasHallSensor", "HasHallSensor 0/1 ", _njoints))
    {
        return false;
    }
    else
    {
        int test = xtmp.size();
        for (i = 1; i < xtmp.size(); i++)
            _hasHallSensor[i - 1] = xtmp.get(i).asInt();
    }
    if (!extractGroup(general, xtmp, "HasTempSensor", "HasTempSensor 0/1 ", _njoints))
    {
        return false;
    }
    else
    {
        int test = xtmp.size();
        for (i = 1; i < xtmp.size(); i++)
            _hasTempSensor[i - 1] = xtmp.get(i).asInt();
    }
    if (!extractGroup(general, xtmp, "HasRotorEncoder", "HasRotorEncoder 0/1 ", _njoints))
    {
        return false;
    }
    else
    {
        int test = xtmp.size();
        for (i = 1; i < xtmp.size(); i++)
            _hasRotorEncoder[i - 1] = xtmp.get(i).asInt();
    }
    if (!extractGroup(general, xtmp, "HasRotorEncoderIndex", "HasRotorEncoderIndex 0/1 ", _njoints))
    {
        return false;
    }
    else
    {
        int test = xtmp.size();
        for (i = 1; i < xtmp.size(); i++)
            _hasRotorEncoderIndex[i - 1] = xtmp.get(i).asInt();
    }

    // Rotor encoder res
    if (!extractGroup(general, xtmp, "RotorEncoderRes", "a list of scales for the rotor encoders", _njoints))
    {
        return false;
    }
    else
    {
        int test = xtmp.size();
        for (i = 1; i < xtmp.size(); i++)
            _rotorEncoderRes[i - 1] = xtmp.get(i).asInt();
    }

    // joint encoder res
    if (!extractGroup(general, xtmp, "JointEncoderRes", "a list of scales for the joint encoders", _njoints))
    {
        return false;
    }
    else
    {
        int test = xtmp.size();
        for (i = 1; i < xtmp.size(); i++)
            _jointEncoderRes[i - 1] = xtmp.get(i).asInt();
    }
*/
    // Number of motor poles
    /*if (!extractGroup(general, xtmp, "MotorPoles", "MotorPoles", _njoints))
    {
        return false;
    }
    else
    {
        int test = xtmp.size();
        for (i = 1; i < xtmp.size(); i++)
            _motorPoles[i - 1] = xtmp.get(i).asInt();
    }

    // Rotor encoder index
    if (!extractGroup(general, xtmp, "RotorIndexOffset", "RotorIndexOffset", _njoints))
    {
        return false;
    }
    else
    {
        int test = xtmp.size();
        for (i = 1; i < xtmp.size(); i++)
            _rotorIndexOffset[i - 1] = xtmp.get(i).asInt();
    }
*/

    // Rotor encoder type
/*
    if (!extractGroup(general, xtmp, "RotorEncoderType", "RotorEncoderType", _njoints))
    {
        return false;
    }
    else
    {
        int test = xtmp.size();
        for (i = 1; i < xtmp.size(); i++)
        {
            uint8_t val;
            string s = xtmp.get(i).asString();
            bool b = EncoderType_iCub2eo(&s, &val);
            if (b == false)
            {
                yError("Invalid RotorEncoderType: %s", s.c_str()); return false;
            }
            _rotorEncoderType[i - 1] = val;
        }
    }
*/
/*
    // Gearbox
    if (!extractGroup(general, xtmp, "Gearbox", "The gearbox reduction ratio", _njoints))
    {
        return false;
    }
    else
    {
        int test = xtmp.size();
        for (i = 1; i < xtmp.size(); i++)
        {
            _gearbox[i-1] = xtmp.get(i).asDouble();
            if (_gearbox[i-1]==0) {yError() << "Using a gearbox value = 0 may cause problems! Check your configuration files"; return false;}
        }
    }

    // Torque sensors stuff
    if (!extractGroup(general, xtmp, "TorqueId","a list of associated joint torque sensor ids", _njoints))
    {
        fprintf(stderr, "Using default value = 0 (disabled)\n");
        for(i=1; i<_njoints+1; i++)
            _torqueSensorId[i-1] = 0;
    }
    else
    {
        for (i = 1; i < xtmp.size(); i++) _torqueSensorId[i-1] = xtmp.get(i).asInt();
    }


    if (!extractGroup(general, xtmp, "TorqueChan","a list of associated joint torque sensor channels", _njoints))
    {
        yWarning() <<  "FakeMotionControl::fromConfig() detected that TorqueChan is not present: using default value = 0 (disabled)";
        for(i=1; i<_njoints+1; i++)
            _torqueSensorChan[i-1] = 0;
    }
    else
    {
        for (i = 1; i < xtmp.size(); i++) _torqueSensorChan[i-1] = xtmp.get(i).asInt();
    }


    if (!extractGroup(general, xtmp, "TorqueMax","full scale value for a joint torque sensor", _njoints))
    {
        return false;
    }
    else
    {
        for (i = 1; i < xtmp.size(); i++)
        {
            _maxTorque[i-1] = xtmp.get(i).asInt();
            _newtonsToSensor[i-1] = 1000.0f; // conversion from Nm into milliNm
        }
    }
*/

    ////// POSITION PIDS
/*
    {
        Bottle posPidsGroup;
        posPidsGroup=config.findGroup("POSITION_CONTROL", "Position Pid parameters new format");
        if (posPidsGroup.isNull()==false)
        {
           Value &controlUnits=posPidsGroup.find("controlUnits");
           if  (controlUnits.isNull() == false && controlUnits.isString() == true)
           {
                if      (controlUnits.toString()==string("metric_units"))
                {
                    yDebug("POSITION_CONTROL: using metric_units");  _positionControlUnits=P_METRIC_UNITS;
                }
                else if (controlUnits.toString()==string("machine_units"))
                {
                    yDebug("POSITION_CONTROL: using machine_units"); _positionControlUnits=P_MACHINE_UNITS;
                }
                else
                {
                    yError() << "FakeMotionControl::fromConfig(): POSITION_CONTROL section: invalid controlUnits value";
                         return false;
                }
           }
           else
           {
                yError() << "FakeMotionControl::fromConfig(): POSITION_CONTROL section: missing controlUnits parameter. Assuming machine_units. Please fix your configuration file.";
                _positionControlUnits=P_MACHINE_UNITS;
           }

//            Value &controlLaw=posPidsGroup.find("controlLaw");
//            if (controlLaw.isNull() == false && controlLaw.isString() == true)
//            {
//                string s_controlaw = controlLaw.toString();
//                if (s_controlaw==string("joint_pid_v1"))
//                {
//                    if (!parsePositionPidsGroup (posPidsGroup, _pids))
//                    {
//                        yError() << "FakeMotionControl::fromConfig(): POSITION_CONTROL section: error detected in parameters syntax";
//                        return false;
//                    }
//                    else
//                    {
//                         yDebug("POSITION_CONTROL: using control law joint_pid_v1");
//                    }
//                }
//                else if (s_controlaw==string("not_implemented"))
//                {
//                    yDebug() << "found 'not_impelemented' in position control_law. This will terminate yarprobotinterface execution.";
//                    return false;
//                }
//                else if (s_controlaw==string("disabled"))
//                {
//                    yDebug() << "found 'disabled' in position control_law. This will terminate yarprobotinterface execution.";
//                    return false;
//                }
//                else
//                {
//                    yError() << "Unable to use control law " << s_controlaw << " por position control. Quitting.";
//                    return false;
//                }
//            }
        }
        else
        {
            yError() <<"FakeMotionControl::fromConfig(): Error: no POS_PIDS group found in config file, returning";
            return false;
        }
    }
*/


    ////// TORQUE PIDS
/*
    {
        Bottle trqPidsGroup;
        trqPidsGroup=config.findGroup("TORQUE_CONTROL", "Torque control parameters new format");
        if (trqPidsGroup.isNull()==false)
        {
           Value &controlUnits=trqPidsGroup.find("controlUnits");
           if  (controlUnits.isNull() == false && controlUnits.isString() == true)
           {
                if      (controlUnits.toString()==string("metric_units"))  {yDebug("TORQUE_CONTROL: using metric_units"); _torqueControlUnits=T_METRIC_UNITS;}
                else if (controlUnits.toString()==string("machine_units")) {yDebug("TORQUE_CONTROL: using metric_units"); _torqueControlUnits=T_MACHINE_UNITS;}
                else    {yError() << "FakeMotionControl::fromConfig(): TORQUE_CONTROL section: invalid controlUnits value";
                         return false;}
           }
           else
           {
                yError() << "FakeMotionControl::fromConfig(): TORQUE_CONTROL section: missing controlUnits parameter. Assuming machine_units. Please fix your configuration file.";
                _torqueControlUnits=T_MACHINE_UNITS;
           }

            if(_torqueControlUnits==T_MACHINE_UNITS)
            {
                yarp::sig::Vector tmpOnes; tmpOnes.resize(_njoints,1.0);
            }
            else if (_torqueControlUnits==T_METRIC_UNITS)
            {
            }
            else
            {
                yError() << "FakeMotionControl::fromConfig(): TORQUE_CONTROL section: invalid controlUnits value (_torqueControlUnits=" << _torqueControlUnits << ")";
                return false;
            }
        }
        else
        {
            yError() <<"FakeMotionControl::fromConfig(): Error: no TORQUE_CONTROL group found in config file";
            _torqueControlEnabled = false;
            return false; //torque control group is mandatory
        }
    }
*/

    ////// IMPEDANCE LIMITS DEFAULT VALUES (UNDER TESTING)
/*
    for(j=0; j<_njoints; j++)
    {
        // got from canBusMotionControl, ask to Randazzo Marco
        _impedance_limits[j].min_damp=  0.001;
        _impedance_limits[j].max_damp=  9.888;
        _impedance_limits[j].min_stiff= 0.002;
        _impedance_limits[j].max_stiff= 9.889;
        _impedance_limits[j].param_a=   0.011;
        _impedance_limits[j].param_b=   0.012;
        _impedance_limits[j].param_c=   0.013;
    }
*/

    /////// JOINTS_COUPLING
/*
    if (_njoints<=4)
    {
        Bottle &coupling=config.findGroup("JOINTS_COUPLING");
        if (coupling.isNull())
        {
            yWarning() << "FakeMotionControl::fromConfig() detected that Group JOINTS_COUPLING is not found in configuration file";
            //return false;
        }
        // current limit
        if (!extractGroup(coupling, xtmp, "kinematic_mj","the kinematic matrix 4x4 which tranforms from joint space to motor space", 16))
        {
            for(i=1; i<xtmp.size(); i++) _kinematic_mj[i-1]=0.0;
        }
        else
            for(i=1; i<xtmp.size(); i++) _kinematic_mj[i-1]=xtmp.get(i).asDouble();
    }
    else
    {
        //we are skipping JOINTS_COUPLING for EMS boards which control MC4 boards (for now)
    }
*/

    /////// LIMITS
/*
    Bottle &limits=config.findGroup("LIMITS");
    if (limits.isNull())
    {
        yWarning() << "FakeMotionControl::fromConfig() detected that Group LIMITS is not found in configuration file";
        return false;
    }
    // current limit
    if (!extractGroup(limits, xtmp, "OverloadCurrents","a list of current limits", _njoints))
        return false;
    else
        for(i=1; i<xtmp.size(); i++) _currentLimits[i-1].overloadCurrent=xtmp.get(i).asDouble();

    // nominal current
    if (!extractGroup(limits, xtmp, "MotorNominalCurrents","a list of nominal current limits", _njoints))
        return false;
    else
        for(i=1; i<xtmp.size(); i++) _currentLimits[i-1].nominalCurrent =xtmp.get(i).asDouble();

    // nominal current
    if (!extractGroup(limits, xtmp, "MotorPeakCurrents","a list of peak current limits", _njoints))
        return false;
    else
        for(i=1; i<xtmp.size(); i++) _currentLimits[i-1].peakCurrent=xtmp.get(i).asDouble();

    // max limit
    if (!extractGroup(limits, xtmp, "Max","a list of maximum angles (in degrees)", _njoints))
        return false;
    else
        for(i=1; i<xtmp.size(); i++) _limitsMax[i-1]=xtmp.get(i).asDouble();

    // min limit
    if (!extractGroup(limits, xtmp, "Min","a list of minimum angles (in degrees)", _njoints))
        return false;
    else
        for(i=1; i<xtmp.size(); i++) _limitsMin[i-1]=xtmp.get(i).asDouble();

    // Rotor max limit
    if (!extractGroup(limits, xtmp, "RotorMax","a list of maximum rotor angles (in degrees)", _njoints))
        return false;
    else
        for(i=1; i<xtmp.size(); i++) _rotorlimits_max[i-1]=xtmp.get(i).asDouble();

    // joint Velocity command max limit
    if (!extractGroup(limits, xtmp, "JntVelocityMax", "a list of maximum velocities for the joints (in degrees/s)", _njoints))
        return false;
    else
        for (i = 1; i<xtmp.size(); i++) _maxJntCmdVelocity[i - 1] = xtmp.get(i).asDouble();

    // Rotor min limit
    if (!extractGroup(limits, xtmp, "RotorMin","a list of minimum roto angles (in degrees)", _njoints))
        return false;
    else
        for(i=1; i<xtmp.size(); i++) _rotorlimits_min[i-1]=xtmp.get(i).asDouble();

    // Motor pwm limit
    if (!extractGroup(limits, xtmp, "MotorPwmLimit","a list of motor PWM limits", _njoints))
        return false;
    else
    {
        for(i=1; i<xtmp.size(); i++)
        {
            _motorPwmLimits[i-1]=xtmp.get(i).asDouble();
            if(_motorPwmLimits[i-1]<0)
            {
                yError() << "MotorPwmLimit should be a positive value";
                return false;
            }
        }
    }
*/
    return true;
}


bool FakeMotionControl::init()
{
    yTrace();
    for(int i=0; i<_njoints; i++)
    {
        pwm[i]              = 33+i;
        pwmLimit[i]         = (33+i)*10;
        current[i]          = (33+i)*100;
        maxCurrent[i]       = (33+i)*1000;
        peakCurrent[i]      = (33+i)*2;
        nominalCurrent[i]   = (33+i)*20;
        supplyVoltage[i]    = (33+i)*200;
    }
    return true;
}



bool FakeMotionControl::close()
{
    yTrace() << " FakeMotionControl::close()";

    ImplementControlMode2::uninitialize();
    ImplementEncodersTimed::uninitialize();
    ImplementMotorEncoders::uninitialize();
    ImplementPositionControl2::uninitialize();
    ImplementVelocityControl<FakeMotionControl, IVelocityControl>::uninitialize();
    ImplementVelocityControl2::uninitialize();
    ImplementPidControl::uninitialize();
    ImplementControlCalibration2<FakeMotionControl, IControlCalibration2>::uninitialize();
    ImplementAmplifierControl<FakeMotionControl, IAmplifierControl>::uninitialize();
    ImplementImpedanceControl::uninitialize();
    ImplementControlLimits2::uninitialize();
    ImplementTorqueControl::uninitialize();
    ImplementPositionDirect::uninitialize();
    ImplementInteractionMode::uninitialize();
    ImplementAxisInfo::uninitialize();

//     cleanup();

    return true;
}

void FakeMotionControl::cleanup(void)
{

}



///////////// PID INTERFACE

bool FakeMotionControl::setPidRaw(const PidControlTypeEnum& pidtype, int j, const Pid &pid)
{
    switch (pidtype)
    {
        case VOCAB_PIDTYPE_POSITION:
            _ppids[j]=pid;
        break;
        case VOCAB_PIDTYPE_VELOCITY:
            _vpids[j]=pid;
        break;
        case VOCAB_PIDTYPE_CURRENT:
            _cpids[j]=pid;
        break;
        case VOCAB_PIDTYPE_TORQUE:
            _tpids[j]=pid;
        break;
        default:
        break;
    }
    return true;
}

bool FakeMotionControl::setPidsRaw(const PidControlTypeEnum& pidtype, const Pid *pids)
{
    bool ret = true;
    for(int j=0; j< _njoints; j++)
    {
        ret &= setPidRaw(pidtype, j, pids[j]);
    }
    return ret;
}

bool FakeMotionControl::setPidReferenceRaw(const PidControlTypeEnum& pidtype, int j, double ref)
{
    switch (pidtype)
    {
        case VOCAB_PIDTYPE_POSITION:
            _ppids_ref[j]=ref;
        break;
        case VOCAB_PIDTYPE_VELOCITY:
            _vpids_ref[j]=ref;
        break;
        case VOCAB_PIDTYPE_CURRENT:
            _cpids_ref[j]=ref;
        break;
        case VOCAB_PIDTYPE_TORQUE:
            _tpids_ref[j]=ref;
        break;
        default:
        break;
    }
    return true;
}

bool FakeMotionControl::setPidReferencesRaw(const PidControlTypeEnum& pidtype, const double *refs)
{
    bool ret = true;
    for(int j=0, index=0; j< _njoints; j++, index++)
    {
        ret &= setPidReferenceRaw(pidtype, j, refs[index]);
    }
    return ret;
}

bool FakeMotionControl::setPidErrorLimitRaw(const PidControlTypeEnum& pidtype, int j, double limit)
{
    switch (pidtype)
    {
        case VOCAB_PIDTYPE_POSITION:
            _ppids_lim[j]=limit;
        break;
        case VOCAB_PIDTYPE_VELOCITY:
            _vpids_lim[j]=limit;
        break;
        case VOCAB_PIDTYPE_CURRENT:
            _cpids_lim[j]=limit;
        break;
        case VOCAB_PIDTYPE_TORQUE:
            _tpids_lim[j]=limit;
        break;
        default:
        break;
    }
    return true;
}

bool FakeMotionControl::setPidErrorLimitsRaw(const PidControlTypeEnum& pidtype, const double *limits)
{
    bool ret = true;
    for(int j=0, index=0; j< _njoints; j++, index++)
    {
        ret &= setPidErrorLimitRaw(pidtype, j, limits[index]);
    }
    return ret;
}

bool FakeMotionControl::getPidErrorRaw(const PidControlTypeEnum& pidtype, int j, double *err)
{
    switch (pidtype)
    {
        case VOCAB_PIDTYPE_POSITION:
            *err=0.1;
        break;
        case VOCAB_PIDTYPE_VELOCITY:
            *err=0.2;
        break;
        case VOCAB_PIDTYPE_CURRENT:
            *err=0.3;
        break;
        case VOCAB_PIDTYPE_TORQUE:
            *err=0.4;
        break;
        default:
        break;
    }
    return true;
}

bool FakeMotionControl::getPidErrorsRaw(const PidControlTypeEnum& pidtype, double *errs)
{
    bool ret = true;
    for(int j=0; j< _njoints; j++)
    {
        ret &= getPidErrorRaw(pidtype, j, &errs[j]);
    }
    return ret;
}

bool FakeMotionControl::getPidRaw(const PidControlTypeEnum& pidtype, int j, Pid *pid)
{
    switch (pidtype)
    {
        case VOCAB_PIDTYPE_POSITION:
            *pid=_ppids[j];
        break;
        case VOCAB_PIDTYPE_VELOCITY:
            *pid=_vpids[j];
        break;
        case VOCAB_PIDTYPE_CURRENT:
            *pid=_cpids[j];
        break;
        case VOCAB_PIDTYPE_TORQUE:
            *pid=_tpids[j];
        break;
        default:
        break;
    }
    return true;
}

bool FakeMotionControl::getPidsRaw(const PidControlTypeEnum& pidtype, Pid *pids)
{
    bool ret = true;

    // just one joint at time, wait answer before getting to the next.
    // This is because otherwise too many msg will be placed into can queue
    for(int j=0, index=0; j<_njoints; j++, index++)
    {
        ret &=getPidRaw(pidtype, j, &pids[j]);
    }
    return ret;
}

bool FakeMotionControl::getPidReferenceRaw(const PidControlTypeEnum& pidtype, int j, double *ref)
{
    switch (pidtype)
    {
        case VOCAB_PIDTYPE_POSITION:
            *ref=_ppids_ref[j];
        break;
        case VOCAB_PIDTYPE_VELOCITY:
            *ref=_vpids_ref[j];
        break;
        case VOCAB_PIDTYPE_CURRENT:
            *ref=_cpids_ref[j];
        break;
        case VOCAB_PIDTYPE_TORQUE:
            *ref=_tpids_ref[j];
        break;
        default:
        break;
    }
    return true;
}

bool FakeMotionControl::getPidReferencesRaw(const PidControlTypeEnum& pidtype, double *refs)
{
    bool ret = true;

    // just one joint at time, wait answer before getting to the next.
    // This is because otherwise too many msg will be placed into can queue
    for(int j=0; j< _njoints; j++)
    {
        ret &= getPidReferenceRaw(pidtype, j, &refs[j]);
    }
    return ret;
}

bool FakeMotionControl::getPidErrorLimitRaw(const PidControlTypeEnum& pidtype, int j, double *limit)
{
    switch (pidtype)
    {
        case VOCAB_PIDTYPE_POSITION:
            *limit=_ppids_lim[j];
        break;
        case VOCAB_PIDTYPE_VELOCITY:
            *limit=_vpids_lim[j];
        break;
        case VOCAB_PIDTYPE_CURRENT:
            *limit=_cpids_lim[j];
        break;
        case VOCAB_PIDTYPE_TORQUE:
            *limit=_tpids_lim[j];
        break;
        default:
        break;
    }
    return true;
}

bool FakeMotionControl::getPidErrorLimitsRaw(const PidControlTypeEnum& pidtype, double *limits)
{
    bool ret = true;
    for(int j=0, index=0; j<_njoints; j++, index++)
    {
        ret &=getPidErrorLimitRaw(pidtype, j, &limits[j]);
    }
    return ret;
}

bool FakeMotionControl::resetPidRaw(const PidControlTypeEnum& pidtype, int j)
{
    return true;
}

bool FakeMotionControl::disablePidRaw(const PidControlTypeEnum& pidtype, int j)
{
    switch (pidtype)
    {
        case VOCAB_PIDTYPE_POSITION:
            _ppids_ena[j]=false;
        break;
        case VOCAB_PIDTYPE_VELOCITY:
            _vpids_ena[j]=false;
        break;
        case VOCAB_PIDTYPE_CURRENT:
            _cpids_ena[j]=false;
        break;
        case VOCAB_PIDTYPE_TORQUE:
            _tpids_ena[j]=false;
        break;
        default:
        break;
    }
    return true;
}

bool FakeMotionControl::enablePidRaw(const PidControlTypeEnum& pidtype, int j)
{
    switch (pidtype)
    {
        case VOCAB_PIDTYPE_POSITION:
            _ppids_ena[j]=true;
        break;
        case VOCAB_PIDTYPE_VELOCITY:
            _vpids_ena[j]=true;
        break;
        case VOCAB_PIDTYPE_CURRENT:
            _cpids_ena[j]=true;
        break;
        case VOCAB_PIDTYPE_TORQUE:
            _tpids_ena[j]=true;
        break;
        default:
        break;
    }
    return true;
}

bool FakeMotionControl::setPidOffsetRaw(const PidControlTypeEnum& pidtype, int j, double v)
{
    switch (pidtype)
    {
        case VOCAB_PIDTYPE_POSITION:
            _ppids[j].offset=v;
        break;
        case VOCAB_PIDTYPE_VELOCITY:
            _vpids[j].offset=v;
        break;
        case VOCAB_PIDTYPE_CURRENT:
            _cpids[j].offset=v;
        break;
        case VOCAB_PIDTYPE_TORQUE:
            _tpids[j].offset=v;
        break;
        default:
        break;
    }
    return true;
}

bool FakeMotionControl::isPidEnabledRaw(const PidControlTypeEnum& pidtype, int j, bool* enabled)
{
    switch (pidtype)
    {
        case VOCAB_PIDTYPE_POSITION:
            *enabled=_ppids_ena[j];
        break;
        case VOCAB_PIDTYPE_VELOCITY:
            *enabled=_vpids_ena[j];
        break;
        case VOCAB_PIDTYPE_CURRENT:
            *enabled=_cpids_ena[j];
        break;
        case VOCAB_PIDTYPE_TORQUE:
            *enabled=_tpids_ena[j];
        break;
        default:
        break;
    }
    return true;
}

bool FakeMotionControl::getPidOutputRaw(const PidControlTypeEnum& pidtype, int j, double *out)
{
    switch (pidtype)
    {
        case VOCAB_PIDTYPE_POSITION:
            *out=1.1;
        break;
        case VOCAB_PIDTYPE_VELOCITY:
            *out=1.2;
        break;
        case VOCAB_PIDTYPE_CURRENT:
            *out=1.3;
        break;
        case VOCAB_PIDTYPE_TORQUE:
            *out=1.4;
        break;
        default:
        break;
    }
    return true;
}

bool FakeMotionControl::getPidOutputsRaw(const PidControlTypeEnum& pidtype, double *outs)
{
    bool ret = true;
    for(int j=0; j< _njoints; j++)
    {
        ret &= getPidOutputRaw(pidtype, j, &outs[j]);
    }
    return ret;
}

////////////////////////////////////////
//    Velocity control interface raw  //
////////////////////////////////////////

bool FakeMotionControl::velocityMoveRaw(int j, double sp)
{
    int mode=0;
    getControlModeRaw(j, &mode);
    if( (mode != VOCAB_CM_VELOCITY) &&
        (mode != VOCAB_CM_MIXED) &&
        (mode != VOCAB_CM_IMPEDANCE_VEL) &&
        (mode != VOCAB_CM_IDLE))
    {
          yError() << "velocityMoveRaw: skipping command because board "  << " joint " << j << " is not in VOCAB_CM_VELOCITY mode";
    }
    _command_speeds[j] = sp;
    return true;
}

bool FakeMotionControl::velocityMoveRaw(const double *sp)
{
    yTrace();
    bool ret = true;
    for(int i=0; i<_njoints; i++)
        ret &= velocityMoveRaw(i, sp[i]);
    return ret;
}


////////////////////////////////////////
//    Calibration control interface   //
////////////////////////////////////////

bool FakeMotionControl::setCalibrationParametersRaw(int j, const CalibrationParameters& params)
{
    yTrace() << "setCalibrationParametersRaw for joint" << j;
    return true;
}

bool FakeMotionControl::calibrate2Raw(int j, unsigned int type, double p1, double p2, double p3)
{
    yTrace() << "calibrate2Raw for joint" << j;
    return true;
}

bool FakeMotionControl::doneRaw(int axis)
{
    bool result = false;

    return result;
}

////////////////////////////////////////
//     Position control interface     //
////////////////////////////////////////

bool FakeMotionControl::getAxes(int *ax)
{
    *ax=_njoints;

    return true;
}

bool FakeMotionControl::positionMoveRaw(int j, double ref)
{
    if(verbose >= VERY_VERBOSE)
        yTrace() << "j " << j << " ref " << ref;

//     if (yarp::os::Time::now()-_last_position_move_time[j]<MAX_POSITION_MOVE_INTERVAL)
//     {
//         yWarning() << "Performance warning: You are using positionMove commands at high rate (<"<< MAX_POSITION_MOVE_INTERVAL*1000.0 <<" ms). Probably position control mode is not the right control mode to use.";
//     }
//     _last_position_move_time[j] = yarp::os::Time::now();

    int mode = 0;
    getControlModeRaw(j, &mode);
    if( (mode != VOCAB_CM_POSITION) &&
        (mode != VOCAB_CM_MIXED) &&
        (mode != VOCAB_CM_IMPEDANCE_POS) &&
        (mode != VOCAB_CM_IDLE))
    {
        yError() << "positionMoveRaw: skipping command because joint " << j << " is not in VOCAB_CM_POSITION mode";
    }
    _posCtrl_references[j] = ref;
    pos[j] = _posCtrl_references[j];
    return true;
}

bool FakeMotionControl::positionMoveRaw(const double *refs)
{
    if(verbose >= VERY_VERBOSE)
        yTrace();

    bool ret = true;
    for(int j=0, index=0; j< _njoints; j++, index++)
    {
        ret &= positionMoveRaw(j, refs[index]);
    }
    return ret;
}

bool FakeMotionControl::relativeMoveRaw(int j, double delta)
{
    if(verbose >= VERY_VERBOSE)
        yTrace() << "j " << j << " ref " << delta;
//     if (yarp::os::Time::now()-_last_position_move_time[j]<MAX_POSITION_MOVE_INTERVAL)
//     {
//         yWarning() << "Performance warning: You are using positionMove commands at high rate (<"<< MAX_POSITION_MOVE_INTERVAL*1000.0 <<" ms). Probably position control mode is not the right control mode to use.";
//     }
//     _last_position_move_time[j] = yarp::os::Time::now();

    int mode = 0;
    getControlModeRaw(j, &mode);
    if( (mode != VOCAB_CM_POSITION) &&
        (mode != VOCAB_CM_MIXED) &&
        (mode != VOCAB_CM_IMPEDANCE_POS) &&
        (mode != VOCAB_CM_IDLE))
    {
        yError() << "relativeMoveRaw: skipping command because joint " << j << " is not in VOCAB_CM_POSITION mode";
    }
    _posCtrl_references[j] += delta;
    pos[j] = _posCtrl_references[j];
    return false;
}

bool FakeMotionControl::relativeMoveRaw(const double *deltas)
{
    if(verbose >= VERY_VERBOSE)
        yTrace();

    bool ret = true;
    for(int j=0, index=0; j< _njoints; j++, index++)
    {
        ret &= relativeMoveRaw(j, deltas[index]);
    }
    return ret;
}


bool FakeMotionControl::checkMotionDoneRaw(int j, bool *flag)
{
    if(verbose >= VERY_VERBOSE)
        yTrace() << "j ";

    *flag = false;
    return false;
}

bool FakeMotionControl::checkMotionDoneRaw(bool *flag)
{
    if(verbose >= VERY_VERBOSE)
        yTrace();

    bool ret = true;
    bool val, tot_res = true;

    for(int j=0, index=0; j< _njoints; j++, index++)
    {
        ret &= checkMotionDoneRaw(&val);
        tot_res &= val;
    }
    *flag = tot_res;
    return ret;
}

bool FakeMotionControl::setRefSpeedRaw(int j, double sp)
{
    // Velocity is expressed in iDegrees/s
    // save internally the new value of speed; it'll be used in the positionMove
    int index = j ;
    _ref_speeds[index] = sp;
    return true;
}

bool FakeMotionControl::setRefSpeedsRaw(const double *spds)
{
    // Velocity is expressed in iDegrees/s
    // save internally the new value of speed; it'll be used in the positionMove
    for(int j=0, index=0; j< _njoints; j++, index++)
    {
        _ref_speeds[index] = spds[index];
    }
    return true;
}

bool FakeMotionControl::setRefAccelerationRaw(int j, double acc)
{
    // Acceleration is expressed in iDegrees/s^2
    // save internally the new value of the acceleration; it'll be used in the velocityMove command

    if (acc > 1e6)
    {
        _ref_accs[j ] =  1e6;
    }
    else if (acc < -1e6)
    {
        _ref_accs[j ] = -1e6;
    }
    else
    {
        _ref_accs[j ] = acc;
    }

    return true;
}

bool FakeMotionControl::setRefAccelerationsRaw(const double *accs)
{
    // Acceleration is expressed in iDegrees/s^2
    // save internally the new value of the acceleration; it'll be used in the velocityMove command
    for(int j=0, index=0; j< _njoints; j++, index++)
    {
        if (accs[j] > 1e6)
        {
            _ref_accs[index] =  1e6;
        }
        else if (accs[j] < -1e6)
        {
            _ref_accs[index] = -1e6;
        }
        else
        {
            _ref_accs[index] = accs[j];
        }
    }
    return true;
}

bool FakeMotionControl::getRefSpeedRaw(int j, double *spd)
{
    *spd = _ref_speeds[j];
    return true;
}

bool FakeMotionControl::getRefSpeedsRaw(double *spds)
{
    memcpy(spds, _ref_speeds, sizeof(double) * _njoints);
    return true;
}

bool FakeMotionControl::getRefAccelerationRaw(int j, double *acc)
{
    *acc = _ref_accs[j];
    return true;
}

bool FakeMotionControl::getRefAccelerationsRaw(double *accs)
{
    memcpy(accs, _ref_accs, sizeof(double) * _njoints);
    return true;
}

bool FakeMotionControl::stopRaw(int j)
{
    return false;
}

bool FakeMotionControl::stopRaw()
{
    bool ret = true;
    for(int j=0; j< _njoints; j++)
    {
        ret &= stopRaw(j);
    }
    return ret;
}
///////////// END Position Control INTERFACE  //////////////////

////////////////////////////////////////
//     Position control2 interface    //
////////////////////////////////////////

bool FakeMotionControl::positionMoveRaw(const int n_joint, const int *joints, const double *refs)
{
    if(verbose >= VERY_VERBOSE)
        yTrace() << " -> n_joint " << n_joint;

    for(int j=0; j<n_joint; j++)
    {
        printf("j: %d; ref %f;\n", joints[j], refs[j]); fflush(stdout);
    }

    bool ret = true;
    for(int j=0; j<n_joint; j++)
    {
        ret = ret &&positionMoveRaw(joints[j], refs[j]);
    }
    return ret;
}

bool FakeMotionControl::relativeMoveRaw(const int n_joint, const int *joints, const double *deltas)
{
    if(verbose >= VERY_VERBOSE)
        yTrace() << "n_joint " << _njoints;

    bool ret = true;
    for(int j=0; j<n_joint; j++)
    {
        ret = ret &&relativeMoveRaw(joints[j], deltas[j]);
    }
    return ret;
}

bool FakeMotionControl::checkMotionDoneRaw(const int n_joint, const int *joints, bool *flag)
{
    if(verbose >= VERY_VERBOSE)
        yTrace() << "n_joint " << _njoints;

    bool ret = true;
    bool val = true;
    bool tot_val = true;

    for(int j=0; j<n_joint; j++)
    {
        ret = ret && checkMotionDoneRaw(joints[j], &val);
        tot_val &= val;
    }
    *flag = tot_val;
    return ret;
}

bool FakeMotionControl::setRefSpeedsRaw(const int n_joint, const int *joints, const double *spds)
{
    if(verbose >= VERY_VERBOSE)
        yTrace() << "n_joint " << _njoints;

    bool ret = true;
    for(int j=0; j<n_joint; j++)
    {
        ret = ret &&setRefSpeedRaw(joints[j], spds[j]);
    }
    return ret;
}

bool FakeMotionControl::setRefAccelerationsRaw(const int n_joint, const int *joints, const double *accs)
{
    if(verbose >= VERY_VERBOSE)
        yTrace() << "n_joint " << _njoints;

    bool ret = true;
    for(int j=0; j<n_joint; j++)
    {
        ret = ret &&setRefAccelerationRaw(joints[j], accs[j]);
    }
    return ret;
}

bool FakeMotionControl::getRefSpeedsRaw(const int n_joint, const int *joints, double *spds)
{
    if(verbose >= VERY_VERBOSE)
        yTrace() << "n_joint " << _njoints;

    bool ret = true;
    for(int j=0; j<n_joint; j++)
    {
        ret = ret && getRefSpeedRaw(joints[j], &spds[j]);
    }
    return ret;
}

bool FakeMotionControl::getRefAccelerationsRaw(const int n_joint, const int *joints, double *accs)
{
    if(verbose >= VERY_VERBOSE)
        yTrace() << "n_joint " << _njoints;

    bool ret = true;
    for(int j=0; j<n_joint; j++)
    {
        ret = ret && getRefAccelerationRaw(joints[j], &accs[j]);
    }
    return ret;
}

bool FakeMotionControl::stopRaw(const int n_joint, const int *joints)
{
    if(verbose >= VERY_VERBOSE)
        yTrace() << "n_joint " << _njoints;

    bool ret = true;
    for(int j=0; j<n_joint; j++)
    {
        ret = ret &&stopRaw(joints[j]);
    }
    return ret;
}

///////////// END Position Control INTERFACE  //////////////////

// ControlMode

// puo' essere richiesto con get
bool FakeMotionControl::getControlModeRaw(int j, int *v)
{
    if(verbose > VERY_VERY_VERBOSE)
        yTrace() << "j: " << j;

    *v = _controlModes[j];
    return true;
}

// IControl Mode 2
bool FakeMotionControl::getControlModesRaw(int* v)
{
    bool ret = true;
    for(int j=0; j< _njoints; j++)
    {
        ret = ret && getControlModeRaw(j, &v[j]);
    }
    return ret;
}

bool FakeMotionControl::getControlModesRaw(const int n_joint, const int *joints, int *modes)
{
    bool ret = true;
    for(int j=0; j< n_joint; j++)
    {
        ret = ret && getControlModeRaw(joints[j], &modes[j]);
    }
    return ret;
}



// marco.accame: con alberto cardellino abbiamo parlato della correttezza di effettuare la verifica di quanto imposto (in setControlModeRaw() ed affini)
// andando a rileggere il valore nella scheda eth fino a che esso non sia quello atteso. si deve fare oppure no?
// con il control mode il can ora lo fa ma e' giusto? era cosi' anche in passato?
bool FakeMotionControl::setControlModeRaw(const int j, const int _mode)
{
    if(verbose >= VERY_VERBOSE)
        yTrace() << "j: " << j << " mode: " << yarp::os::Vocab::decode(_mode);

    _controlModes[j] = _mode;
    return true;
}


bool FakeMotionControl::setControlModesRaw(const int n_joint, const int *joints, int *modes)
{
    if(verbose >= VERY_VERBOSE)
        yTrace() << "n_joints: " << n_joint;

    bool ret = true;
    for(int i=0; i<n_joint; i++)
    {
        ret &= setControlModeRaw(joints[i], modes[i]);
    }
    return ret;
}

bool FakeMotionControl::setControlModesRaw(int *modes)
{
    if(verbose >= VERY_VERBOSE)
        yTrace();

    bool ret = true;
    for(int i=0; i<_njoints; i++)
    {
        ret &= setControlModeRaw(i, modes[i]);
    }
    return ret;
}


//////////////////////// BEGIN EncoderInterface

bool FakeMotionControl::setEncoderRaw(int j, double val)
{
    return NOT_YET_IMPLEMENTED("setEncoder");
}

bool FakeMotionControl::setEncodersRaw(const double *vals)
{
    return NOT_YET_IMPLEMENTED("setEncoders");
}

bool FakeMotionControl::resetEncoderRaw(int j)
{
    return NOT_YET_IMPLEMENTED("resetEncoder");
}

bool FakeMotionControl::resetEncodersRaw()
{
    return NOT_YET_IMPLEMENTED("resetEncoders");
}

bool FakeMotionControl::getEncoderRaw(int j, double *value)
{
    bool ret = true;

    // To simulate a real controlboard, we assume that the joint
    // encoders is exactly the last set by setPosition(s) or positionMove
    *value = pos[j];

    return ret;
}

bool FakeMotionControl::getEncodersRaw(double *encs)
{
    bool ret = true;
    for(int j=0; j< _njoints; j++)
    {
        bool ok = getEncoderRaw(j, &encs[j]);
        ret = ret && ok;

    }
    return ret;
}

bool FakeMotionControl::getEncoderSpeedRaw(int j, double *sp)
{
    // To avoid returning uninitialized memory, we set the encoder speed to 0
    *sp = 0.0;
    return true;
}

bool FakeMotionControl::getEncoderSpeedsRaw(double *spds)
{
    bool ret = true;
    for(int j=0; j< _njoints; j++)
    {
        ret &= getEncoderSpeedRaw(j, &spds[j]);
    }
    return ret;
}

bool FakeMotionControl::getEncoderAccelerationRaw(int j, double *acc)
{
    // To avoid returning uninitialized memory, we set the encoder acc to 0
    *acc = 0.0;

    return true;
}

bool FakeMotionControl::getEncoderAccelerationsRaw(double *accs)
{
    bool ret = true;
    for(int j=0; j< _njoints; j++)
    {
        ret &= getEncoderAccelerationRaw(j, &accs[j]);
    }
    return ret;
}

///////////////////////// END Encoder Interface

bool FakeMotionControl::getEncodersTimedRaw(double *encs, double *stamps)
{
    bool ret = getEncodersRaw(encs);
    _mutex.wait();
    for(int i=0; i<_njoints; i++)
        stamps[i] = _encodersStamp[i];
    _mutex.post();
    return ret;
}

bool FakeMotionControl::getEncoderTimedRaw(int j, double *encs, double *stamp)
{
    bool ret = getEncoderRaw(j, encs);
    _mutex.wait();
    *stamp = _encodersStamp[j];
    _mutex.post();

    return ret;
}

//////////////////////// BEGIN EncoderInterface

bool FakeMotionControl::getNumberOfMotorEncodersRaw(int* num)
{
    *num=_njoints;
    return true;
}

bool FakeMotionControl::setMotorEncoderRaw(int m, const double val)
{
    return NOT_YET_IMPLEMENTED("setMotorEncoder");
}

bool FakeMotionControl::setMotorEncodersRaw(const double *vals)
{
    return NOT_YET_IMPLEMENTED("setMotorEncoders");
}

bool FakeMotionControl::setMotorEncoderCountsPerRevolutionRaw(int m, const double cpr)
{
    return NOT_YET_IMPLEMENTED("setMotorEncoderCountsPerRevolutionRaw");
}

bool FakeMotionControl::getMotorEncoderCountsPerRevolutionRaw(int m, double *cpr)
{
    return NOT_YET_IMPLEMENTED("getMotorEncoderCountsPerRevolutionRaw");
}

bool FakeMotionControl::resetMotorEncoderRaw(int mj)
{
    return NOT_YET_IMPLEMENTED("resetMotorEncoder");
}

bool FakeMotionControl::resetMotorEncodersRaw()
{
    return NOT_YET_IMPLEMENTED("reseMotortEncoders");
}

bool FakeMotionControl::getMotorEncoderRaw(int m, double *value)
{
    *value = 0.0;
    return true;
}

bool FakeMotionControl::getMotorEncodersRaw(double *encs)
{
    bool ret = true;
    for(int j=0; j< _njoints; j++)
    {
        ret &= getMotorEncoderRaw(j, &encs[j]);

    }
    return ret;
}

bool FakeMotionControl::getMotorEncoderSpeedRaw(int m, double *sp)
{
    *sp = 0.0;
    return true;
}

bool FakeMotionControl::getMotorEncoderSpeedsRaw(double *spds)
{
    bool ret = true;
    for(int j=0; j< _njoints; j++)
    {
        ret &= getMotorEncoderSpeedRaw(j, &spds[j]);
    }
    return ret;
}

bool FakeMotionControl::getMotorEncoderAccelerationRaw(int m, double *acc)
{
    *acc = 0.0;
    return true;
}

bool FakeMotionControl::getMotorEncoderAccelerationsRaw(double *accs)
{
    bool ret = true;
    for(int j=0; j< _njoints; j++)
    {
        ret &= getMotorEncoderAccelerationRaw(j, &accs[j]);
    }
    return ret;
}

bool FakeMotionControl::getMotorEncodersTimedRaw(double *encs, double *stamps)
{
    bool ret = getMotorEncodersRaw(encs);
    _mutex.wait();
    for(int i=0; i<_njoints; i++)
        stamps[i] = _encodersStamp[i];
    _mutex.post();

    return ret;
}

bool FakeMotionControl::getMotorEncoderTimedRaw(int m, double *encs, double *stamp)
{
    bool ret = getMotorEncoderRaw(m, encs);
    _mutex.wait();
    *stamp = _encodersStamp[m];
    _mutex.post();

    return ret;
}
///////////////////////// END Motor Encoder Interface

////// Amplifier interface

bool FakeMotionControl::enableAmpRaw(int j)
{
    return DEPRECATED("enableAmpRaw");
}

bool FakeMotionControl::disableAmpRaw(int j)
{
    return DEPRECATED("disableAmpRaw");
}

bool FakeMotionControl::getCurrentRaw(int j, double *value)
{
    //just for testing purposes, this is not a real implementation
    *value = current[j];
    return true;
}

bool FakeMotionControl::getCurrentsRaw(double *vals)
{
    //just for testing purposes, this is not a real implementation
    bool ret = true;
    for(int j=0; j< _njoints; j++)
    {
        ret &= getCurrentRaw(j, &vals[j]);
    }
    return ret;
}

bool FakeMotionControl::setMaxCurrentRaw(int m, double val)
{
    maxCurrent[m] = val;
    return true;
}

bool FakeMotionControl::getMaxCurrentRaw(int m, double *val)
{
    *val = maxCurrent[m];
    return true;
}

bool FakeMotionControl::getAmpStatusRaw(int j, int *st)
{
    (_enabledAmp[j ]) ? *st = 1 : *st = 0;
    return true;
}

bool FakeMotionControl::getAmpStatusRaw(int *sts)
{
    bool ret = true;
    for(int j=0; j<_njoints; j++)
    {
        sts[j] = _enabledAmp[j];
    }
    return ret;
}

bool FakeMotionControl::getPeakCurrentRaw(int m, double *val)
{
    *val = peakCurrent[m];
    return true;
}

bool FakeMotionControl::setPeakCurrentRaw(int m, const double val)
{
    peakCurrent[m] = val;
    return true;
}

bool FakeMotionControl::getNominalCurrentRaw(int m, double *val)
{
    *val = nominalCurrent[m];
    return true;
}

bool FakeMotionControl::getPWMRaw(int m, double *val)
{
    *val = pwm[m];
    return true;
}

bool FakeMotionControl::getPWMLimitRaw(int m, double* val)
{
    *val = pwmLimit[m];
    return true;
}

bool FakeMotionControl::setPWMLimitRaw(int m, const double val)
{
    pwmLimit[m] = val;
    return true;
}

bool FakeMotionControl::getPowerSupplyVoltageRaw(int m, double* val)
{
    *val = supplyVoltage[m];
    return true;
}


// Limit interface
bool FakeMotionControl::setLimitsRaw(int j, double min, double max)
{
    bool ret = true;
    return ret;
}

bool FakeMotionControl::getLimitsRaw(int j, double *min, double *max)
{
    return false;
}

bool FakeMotionControl::getGearboxRatioRaw(int j, double *gearbox)
{
    return true;
}

bool FakeMotionControl::getTorqueControlFilterType(int j, int& type)
{
    return true;
}

bool FakeMotionControl::getRotorEncoderResolutionRaw(int j, double &rotres)
{
    return true;
}

bool FakeMotionControl::getJointEncoderResolutionRaw(int j, double &jntres)
{
    return true;
}

bool FakeMotionControl::getJointEncoderTypeRaw(int j, int &type)
{
    return true;
}

bool FakeMotionControl::getRotorEncoderTypeRaw(int j, int &type)
{
    return true;
}

bool FakeMotionControl::getKinematicMJRaw(int j, double &rotres)
{
    yError("getKinematicMJRaw not yet  implemented");
    return false;
}

bool FakeMotionControl::getHasTempSensorsRaw(int j, int& ret)
{
    return true;
}

bool FakeMotionControl::getHasHallSensorRaw(int j, int& ret)
{
    return true;
}

bool FakeMotionControl::getHasRotorEncoderRaw(int j, int& ret)
{
    return true;
}

bool FakeMotionControl::getHasRotorEncoderIndexRaw(int j, int& ret)
{
    return true;
}

bool FakeMotionControl::getMotorPolesRaw(int j, int& poles)
{
    return true;
}

bool FakeMotionControl::getRotorIndexOffsetRaw(int j, double& rotorOffset)
{
    return true;
}

bool FakeMotionControl::getAxisNameRaw(int axis, yarp::os::ConstString& name)
{
    if (axis >= 0 && axis < _njoints)
    {
        name = _axisName[axis];
        return true;
    }
    else
    {
        name = "ERROR";
        return false;
    }
}

bool FakeMotionControl::getJointTypeRaw(int axis, yarp::dev::JointTypeEnum& type)
{
    if (axis >= 0 && axis < _njoints)
    {
        type = _jointType[axis];
        return true;
    }
    else
    {
        return false;
    }
}

// IControlLimits2
bool FakeMotionControl::setVelLimitsRaw(int axis, double min, double max)
{
    return NOT_YET_IMPLEMENTED("setVelLimitsRaw");
}

bool FakeMotionControl::getVelLimitsRaw(int axis, double *min, double *max)
{
    *min = 0.0;
    *max = _maxJntCmdVelocity[axis];
    return true;
}


// Torque control
bool FakeMotionControl::getTorqueRaw(int j, double *t)
{
    return true;
}

bool FakeMotionControl::getTorquesRaw(double *t)
{
    return true;
}

bool FakeMotionControl::getTorqueRangeRaw(int j, double *min, double *max)
{
    return NOT_YET_IMPLEMENTED("getTorqueRangeRaw");
}

bool FakeMotionControl::getTorqueRangesRaw(double *min, double *max)
{
    return NOT_YET_IMPLEMENTED("getTorqueRangesRaw");
}

bool FakeMotionControl::setRefTorquesRaw(const double *t)
{
    bool ret = true;
    for(int j=0; j<_njoints && ret; j++)
        ret &= setRefTorqueRaw(j, t[j]);
    return ret;
}

bool FakeMotionControl::setRefTorqueRaw(int j, double t)
{
    return false;
}

bool FakeMotionControl::setRefTorquesRaw(const int n_joint, const int *joints, const double *t)
{
    return false;
}

bool FakeMotionControl::getRefTorquesRaw(double *t)
{
    return false;
}

bool FakeMotionControl::getRefTorqueRaw(int j, double *t)
{
    return false;
}

bool FakeMotionControl::getImpedanceRaw(int j, double *stiffness, double *damping)
{
    return false;
}

bool FakeMotionControl::setImpedanceRaw(int j, double stiffness, double damping)
{
    return false;
}

bool FakeMotionControl::setImpedanceOffsetRaw(int j, double offset)
{
    return false;
}

bool FakeMotionControl::getImpedanceOffsetRaw(int j, double *offset)
{
    return false;
}

bool FakeMotionControl::getCurrentImpedanceLimitRaw(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp)
{
    return false;
}

bool FakeMotionControl::getBemfParamRaw(int j, double *bemf)
{
    return DEPRECATED("getBemfParamRaw");
}

bool FakeMotionControl::setBemfParamRaw(int j, double bemf)
{
    return DEPRECATED("setBemfParamRaw");
}

bool FakeMotionControl::getMotorTorqueParamsRaw(int j, MotorTorqueParameters *params)
{
    return false;
}

bool FakeMotionControl::setMotorTorqueParamsRaw(int j, const MotorTorqueParameters params)
{
    return false;
}

// IVelocityControl2
bool FakeMotionControl::velocityMoveRaw(const int n_joint, const int *joints, const double *spds)
{
    bool ret = true;
    for(int i=0; i<n_joint; i++)
    {
        ret &= velocityMoveRaw(joints[i], spds[i]);
    }
    return ret;
}

// PositionDirect Interface
bool FakeMotionControl::setPositionRaw(int j, double ref)
{
    _posDir_references[j] = ref;
    pos[j] = _posDir_references[j];
    return true;
}

bool FakeMotionControl::setPositionsRaw(const int n_joint, const int *joints, double *refs)
{
    for(int i=0; i< n_joint; i++)
    {
        _posDir_references[joints[i]] = refs[i];
        pos[joints[i]] = _posDir_references[joints[i]];
    }
    return true;
}

bool FakeMotionControl::setPositionsRaw(const double *refs)
{
    for(int i=0; i< _njoints; i++)
    {
        _posDir_references[i] = refs[i];
        pos[i] = _posDir_references[i];
    }
    return true;
}


bool FakeMotionControl::getTargetPositionRaw(int axis, double *ref)
{
    if(verbose >= VERY_VERBOSE)
        yTrace() << "j " << axis << " ref " << _posCtrl_references[axis];

    int mode = 0;
    getControlModeRaw(axis, &mode);
    if( (mode != VOCAB_CM_POSITION) &&
        (mode != VOCAB_CM_MIXED) &&
        (mode != VOCAB_CM_IMPEDANCE_POS))
    {
        yWarning() << "getTargetPosition: Joint " << axis << " is not in POSITION mode, therefore the value returned by " <<
        "this call is for reference only and may not reflect the actual behaviour of the motor/firmware.";
    }
    *ref = _posCtrl_references[axis];
    return true;
}

bool FakeMotionControl::getTargetPositionsRaw(double *refs)
{
    bool ret = true;
    for(int i=0; i<_njoints; i++)
        ret &= getTargetPositionRaw(i, &refs[i]);
    return ret;
}

bool FakeMotionControl::getTargetPositionsRaw(int nj, const int * jnts, double *refs)
{
    bool ret = true;
    for (int i = 0; i<nj; i++)
    {
        ret &= getTargetPositionRaw(jnts[i], &refs[i]);
    }
    return ret;
}

bool FakeMotionControl::getRefVelocityRaw(int axis, double *ref)
{
    *ref = _command_speeds[axis];
    return true;
}

bool FakeMotionControl::getRefVelocitiesRaw(double *refs)
{
    bool ret = true;
    for (int i = 0; i<_njoints; i++)
    {
        ret &= getRefVelocityRaw(i, &refs[i]);
    }
    return ret;
}

bool FakeMotionControl::getRefVelocitiesRaw(int nj, const int * jnts, double *refs)
{
    bool ret = true;
    for (int i = 0; i<nj; i++)
    {
        ret &= getRefVelocityRaw(jnts[i], &refs[i]);
    }
    return ret;
}

bool FakeMotionControl::getRefPositionRaw(int axis, double *ref)
{
    int mode = 0;
    getControlModeRaw(axis, &mode);
    if( (mode != VOCAB_CM_POSITION) &&
        (mode != VOCAB_CM_MIXED) &&
        (mode != VOCAB_CM_IMPEDANCE_POS) &&
        (mode != VOCAB_CM_POSITION_DIRECT) )
    {
        yWarning() << "getRefPosition: Joint " << axis << " is not in POSITION_DIRECT mode, therefore the value returned by \
        this call is for reference only and may not reflect the actual behaviour of the motor/firmware.";
    }

    *ref = _posDir_references[axis];

    return true;
}

bool FakeMotionControl::getRefPositionsRaw(double *refs)
{
    bool ret = true;
    for (int i = 0; i<_njoints; i++)
    {
        ret &= getRefPositionRaw(i, &refs[i]);
    }
    return ret;
}

bool FakeMotionControl::getRefPositionsRaw(int nj, const int * jnts, double *refs)
{
    bool ret = true;
    for (int i = 0; i<nj; i++)
    {
        ret &= getRefPositionRaw(jnts[i], &refs[i]);
    }
    return ret;
}


// InteractionMode
bool FakeMotionControl::getInteractionModeRaw(int j, yarp::dev::InteractionModeEnum* _mode)
{
    return false;
}

bool FakeMotionControl::getInteractionModesRaw(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes)
{
    return false;
}

bool FakeMotionControl::getInteractionModesRaw(yarp::dev::InteractionModeEnum* modes)
{
    bool ret = true;
    for(int j=0; j<_njoints; j++)
        ret = ret && getInteractionModeRaw(j, &modes[j]);
    return ret;
}

// marco.accame: con alberto cardellino abbiamo parlato della correttezza di effettuare la verifica di quanto imposto (in setInteractionModeRaw() ed affini)
// andando a rileggere il valore nella scheda eth fino a che esso non sia quello atteso. si deve fare oppure no?
// con il interaction mode il can ora non lo fa. mentre lo fa per il control mode. perche' diverso?
bool FakeMotionControl::setInteractionModeRaw(int j, yarp::dev::InteractionModeEnum _mode)
{
     return false;
}


bool FakeMotionControl::setInteractionModesRaw(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes)
{
    return false;
}

bool FakeMotionControl::setInteractionModesRaw(yarp::dev::InteractionModeEnum* modes)
{
    return false;
}

bool FakeMotionControl::getNumberOfMotorsRaw(int* num)
{
    *num=_njoints;
    return true;
}

bool FakeMotionControl::getTemperatureRaw(int m, double* val)
{
    return false;
}

bool FakeMotionControl::getTemperaturesRaw(double *vals)
{
    bool ret = true;
    for(int j=0; j< _njoints; j++)
    {
        ret &= getTemperatureRaw(j, &vals[j]);
    }
    return ret;
}

bool FakeMotionControl::getTemperatureLimitRaw(int m, double *temp)
{
    return false;
}

bool FakeMotionControl::setTemperatureLimitRaw(int m, const double temp)
{
    return false;
}

//PWM interface
bool FakeMotionControl::setRefDutyCycleRaw(int j, double v)
{
    refpwm[j] = v;
    pwm[j] = v;
    return true;
}

bool FakeMotionControl::setRefDutyCyclesRaw(const double *v)
{
    for (int i = 0; i < _njoints; i++)
    {
        refpwm[i] = v[i];
        pwm[i] = v[i];
    }
    return true;
}

bool FakeMotionControl::getRefDutyCycleRaw(int j, double *v)
{
    *v = refpwm[j];
    return true;
}

bool FakeMotionControl::getRefDutyCyclesRaw(double *v)
{
    for (int i = 0; i < _njoints; i++)
    {
        v[i] = refpwm[i];
    }
    return true;
}

bool FakeMotionControl::getDutyCycleRaw(int j, double *v)
{
    *v = pwm[j];
    return true;
}

bool FakeMotionControl::getDutyCyclesRaw(double *v)
{
    for (int i = 0; i < _njoints; i++)
    {
        v[i] = pwm[i];
    }
    return true;
}

// Current interface
/*bool FakeMotionControl::getCurrentRaw(int j, double *t)
{
    return NOT_YET_IMPLEMENTED("getCurrentRaw");
}

bool FakeMotionControl::getCurrentsRaw(double *t)
{
    return NOT_YET_IMPLEMENTED("getCurrentsRaw");
}
*/

bool FakeMotionControl::getCurrentRangeRaw(int j, double *min, double *max)
{
    //just for testing purposes, this is not a real implementation
    *min = _ref_currents[j] / 100;
    *max = _ref_currents[j] * 100;
    return true;
}

bool FakeMotionControl::getCurrentRangesRaw(double *min, double *max)
{
    //just for testing purposes, this is not a real implementation
    for (int i = 0; i < _njoints; i++)
    {
        min[i] = _ref_currents[i] / 100;
        max[i] = _ref_currents[i] * 100;
    }
    return true;
}

bool FakeMotionControl::setRefCurrentsRaw(const double *t)
{
    for (int i = 0; i < _njoints; i++)
    {
        _ref_currents[i] = t[i];
        current[i] = t[i] / 2;
    }
    return true;
}

bool FakeMotionControl::setRefCurrentRaw(int j, double t)
{
    _ref_currents[j] = t;
    current[j] = t / 2;
    return true;
}

bool FakeMotionControl::setRefCurrentsRaw(const int n_joint, const int *joints, const double *t)
{
    bool ret = true;
    for (int j = 0; j<n_joint; j++)
    {
        ret = ret &&setRefCurrentRaw(joints[j], t[j]);
    }
    return ret;
}

bool FakeMotionControl::getRefCurrentsRaw(double *t)
{
    for (int i = 0; i < _njoints; i++)
    {
        t[i] = _ref_currents[i];
    }
    return true;
}

bool FakeMotionControl::getRefCurrentRaw(int j, double *t)
{
    *t = _ref_currents[j];
    return true;
}

// bool FakeMotionControl::checkRemoteControlModeStatus(int joint, int target_mode)
// {
//     return false;
// }


// eof
