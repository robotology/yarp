/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FakeMotionControl.h"

#include <yarp/conf/environment.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/NetType.h>
#include <yarp/dev/Drivers.h>

#include <sstream>
#include <cstring>

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::os::impl;

// macros
#define NEW_JSTATUS_STRUCT 1
#define VELOCITY_WATCHDOG 0.1
#define OPENLOOP_WATCHDOG 0.1
#define PWM_CONSTANT      0.1

namespace {
YARP_LOG_COMPONENT(FAKEMOTIONCONTROL, "yarp.device.fakeMotionControl")
}

void FakeMotionControl::run()
{
    std::lock_guard lock(_mutex);

    _cycleTimestamp = yarp::os::Time::now();

    for (int i=0;i <_njoints ;i++)
    {
        if (_controlModes[i] == VOCAB_CM_VELOCITY)
        {
            //increment joint position
            if (this->_command_speeds[i]!=0)
            {
                double elapsed = yarp::os::Time::now()-prev_time;
                double increment = _command_speeds[i]*elapsed;
                pos[i]+=increment;
            }

            //velocity watchdog
            if (velocity_watchdog_enabled && yarp::os::Time::now()-last_velocity_command[i]>=VELOCITY_WATCHDOG)
            {
                this->_command_speeds[i]=0.0;
            }
        }
        else if (_controlModes[i] == VOCAB_CM_PWM)
        {
            //increment joint position
            if (this->refpwm[i]!=0)
            {
                double elapsed = yarp::os::Time::now()-prev_time;
                double increment = refpwm[i]*elapsed*PWM_CONSTANT;
                pos[i]+=increment;
            }

            //pwm watchdog
            if (openloop_watchdog_enabled && yarp::os::Time::now()-last_pwm_command[i]>=OPENLOOP_WATCHDOG)
            {
                this->refpwm[i]=0.0;
            }
        }
        else if (_controlModes[i] == VOCAB_CM_POSITION_DIRECT)
        {
             pos[i] = _posDir_references[i];
        }
        else if (_controlModes[i] == VOCAB_CM_POSITION)
        {
             pos[i] = _posCtrl_references[i];
             //do something with _ref_speeds[i];
        }
        else if (_controlModes[i] == VOCAB_CM_IDLE)
        {
            //do nothing
        }
        else if (_controlModes[i] == VOCAB_CM_CURRENT)
        {
            //do nothing
        }
        else if (_controlModes[i] == VOCAB_CM_VELOCITY_DIRECT)
        {
            //not yet implemented
        }
        else if (_controlModes[i] == VOCAB_CM_MIXED)
        {
            //not yet implemented
        }
        else if (_controlModes[i] == VOCAB_CM_TORQUE)
        {
            //not yet implemented
        }
        else if (_controlModes[i] == VOCAB_CM_HW_FAULT)
        {
            //not yet implemented
        }
        else
        {
            //unsupported control mode
            yCWarning(FAKEMOTIONCONTROL) << "Unsupported control mode, joint " << i << " " << yarp::os::Vocab32::decode(_controlModes[i]);
        }
    }
    prev_time = yarp::os::Time::now();
}

static inline bool NOT_YET_IMPLEMENTED(const char *txt)
{
    yCDebug(FAKEMOTIONCONTROL) << txt << "is not yet implemented for FakeMotionControl";
    return true;
}

static inline bool DEPRECATED(const char *txt)
{
    yCError(FAKEMOTIONCONTROL) << txt << "has been deprecated for FakeMotionControl";
    return true;
}


// replace with to_string as soon as C++11 is required by YARP
/**
 * @brief convert an arbitrary type to string.
 *
 */
template<typename T>
std::string toString(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

//generic function that check is key1 is present in input bottle and that the result has size elements
// return true/false
/*
bool FakeMotionControl::extractGroup(Bottle &input, Bottle &out, const std::string &key1, const std::string &txt, int size)
{
    size++;
    Bottle &tmp=input.findGroup(key1, txt);

    if (tmp.isNull())
    {
        yCError(FAKEMOTIONCONTROL) << key1.c_str() << "parameter not found";
        return false;
    }

    if(tmp.size()!=(size_t) size)
    {
        yCError(FAKEMOTIONCONTROL) << key1.c_str() << "incorrect number of entries";
        return false;
    }

    out=tmp;
    return true;
}
*/

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
    last_velocity_command.resize(_njoints);
    last_pwm_command.resize(_njoints);

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


void FakeMotionControl::setInfoPPids(int j)
{
    _ppids[j].at(0).pidExtraInfo.description.pid_description = "Position PID 0";
    _ppids[j].at(0).pidExtraInfo.description.input_data_description = "Encoder Value";
    _ppids[j].at(0).pidExtraInfo.description.output_data_description = "PWM";

    _ppids[j].at(1).pidExtraInfo.description.pid_description = "Position PID 1";
    _ppids[j].at(1).pidExtraInfo.description.input_data_description = "Encoder Value";
    _ppids[j].at(1).pidExtraInfo.description.output_data_description = "Velocity";

    _ppids[j].at(2).pidExtraInfo.description.pid_description = "Position PID 2";
    _ppids[j].at(2).pidExtraInfo.description.input_data_description = "Encoder Value";
    _ppids[j].at(2).pidExtraInfo.description.output_data_description = "Current";
}

void FakeMotionControl::setInfoPDPids(int j)
{
    _pdpids[j].at(0).pidExtraInfo.description.pid_description = "Position Direct PID 0";
    _pdpids[j].at(0).pidExtraInfo.description.input_data_description = "Encoder Value";
    _pdpids[j].at(0).pidExtraInfo.description.output_data_description = "PWM";

    _pdpids[j].at(1).pidExtraInfo.description.pid_description = "Position Direct PID 1";
    _pdpids[j].at(1).pidExtraInfo.description.input_data_description = "Encoder Value";
    _pdpids[j].at(1).pidExtraInfo.description.output_data_description = "Velocity";

    _pdpids[j].at(2).pidExtraInfo.description.pid_description = "Position Direct PID 2";
    _pdpids[j].at(2).pidExtraInfo.description.input_data_description = "Encoder Value";
    _pdpids[j].at(2).pidExtraInfo.description.output_data_description = "Current";
}

void FakeMotionControl::setInfoTPids(int j)
{
    _tpids[j].at(0).pidExtraInfo.description.pid_description = "Torque PID 0";
    _tpids[j].at(0).pidExtraInfo.description.input_data_description = "Torque";
    _tpids[j].at(0).pidExtraInfo.description.output_data_description = "PWM";

    _tpids[j].at(1).pidExtraInfo.description.pid_description = "Torque PID 1";
    _tpids[j].at(1).pidExtraInfo.description.input_data_description = "Torque";
    _tpids[j].at(1).pidExtraInfo.description.output_data_description = "Velocity";

    _tpids[j].at(2).pidExtraInfo.description.pid_description = "Torque PID 2";
    _tpids[j].at(2).pidExtraInfo.description.input_data_description = "Torque";
    _tpids[j].at(2).pidExtraInfo.description.output_data_description = "Current";
}

void FakeMotionControl::setInfoVPids(int j)
{
    _vpids[j].at(0).pidExtraInfo.description.pid_description = "Velocity PID 0";
    _vpids[j].at(0).pidExtraInfo.description.input_data_description = "Velocity Value";
    _vpids[j].at(0).pidExtraInfo.description.output_data_description = "PWM";

    _vpids[j].at(1).pidExtraInfo.description.pid_description = "Velocity PID 1";
    _vpids[j].at(1).pidExtraInfo.description.input_data_description = "Velocity Value";
    _vpids[j].at(1).pidExtraInfo.description.output_data_description = "PWM";

    _vpids[j].at(2).pidExtraInfo.description.pid_description = "Velocity PID 2";
    _vpids[j].at(2).pidExtraInfo.description.input_data_description = "Velocity Value";
    _vpids[j].at(2).pidExtraInfo.description.output_data_description = "Current";
}

void FakeMotionControl::setInfoVDPids(int j)
{
    _vdpids[j].at(0).pidExtraInfo.description.pid_description = "Velocity Direct PID 0";
    _vdpids[j].at(0).pidExtraInfo.description.input_data_description = "Velocity Value";
    _vdpids[j].at(0).pidExtraInfo.description.output_data_description = "PWM";

    _vdpids[j].at(1).pidExtraInfo.description.pid_description = "Velocity Direct PID 1";
    _vdpids[j].at(1).pidExtraInfo.description.input_data_description = "Velocity Value";
    _vdpids[j].at(1).pidExtraInfo.description.output_data_description = "PWM";

    _vdpids[j].at(2).pidExtraInfo.description.pid_description = "Velocity Direct PID 2";
    _vdpids[j].at(2).pidExtraInfo.description.input_data_description = "Velocity Value";
    _vdpids[j].at(2).pidExtraInfo.description.output_data_description = "Current";
}

void FakeMotionControl::setInfoCPids(int j)
{
    _cpids[j].at(0).pidExtraInfo.description.pid_description = "Current PID 0";
    _cpids[j].at(0).pidExtraInfo.description.input_data_description = "Current Value";
    _cpids[j].at(0).pidExtraInfo.description.output_data_description = "PWM";

    _cpids[j].at(1).pidExtraInfo.description.pid_description = "Current PID 1";
    _cpids[j].at(1).pidExtraInfo.description.input_data_description = "Current Value";
    _cpids[j].at(1).pidExtraInfo.description.output_data_description = "PWM";

    _cpids[j].at(2).pidExtraInfo.description.pid_description = "Current PID 2";
    _cpids[j].at(2).pidExtraInfo.description.input_data_description = "Current Value";
    _cpids[j].at(2).pidExtraInfo.description.output_data_description = "PWM";
}

void FakeMotionControl::setInfoMPids(int j)
{
    _mpids[j].at(0).pidExtraInfo.description.pid_description = "Mixed PID 0";
    _mpids[j].at(0).pidExtraInfo.description.input_data_description = "Encoder Value";
    _mpids[j].at(0).pidExtraInfo.description.output_data_description = "PWM";

    _mpids[j].at(1).pidExtraInfo.description.pid_description = "Mixed PID 1";
    _mpids[j].at(1).pidExtraInfo.description.input_data_description = "Encoder Value";
    _mpids[j].at(1).pidExtraInfo.description.output_data_description = "PWM";

    _mpids[j].at(2).pidExtraInfo.description.pid_description = "Current PID 2";
    _mpids[j].at(2).pidExtraInfo.description.input_data_description = "Encoder Value";
    _mpids[j].at(2).pidExtraInfo.description.output_data_description = "PWM";
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
    _jointEncoderRes = allocAndCheck<int>(nj);
    _rotorEncoderRes = allocAndCheck<int>(nj);
    _gearbox = allocAndCheck<double>(nj);
    _torqueSensorId= allocAndCheck<int>(nj);
    _torqueSensorChan= allocAndCheck<int>(nj);
    _maxTorque=allocAndCheck<double>(nj);
    _torques = allocAndCheck<double>(nj);
    _minJntCmdVelocity = allocAndCheck<double>(nj);
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
    _hwfault_code = allocAndCheck<int>(nj);
    _hwfault_message = allocAndCheck<std::string>(nj);
    _braked = allocAndCheck<bool>(nj);
    _autobraked = allocAndCheck<bool>(nj);

    _ppids = allocAndCheck<std::vector<PidWithExtraInfo>>(nj);    for (int i = 0; i < nj; ++i) {_ppids[i].resize(npids); setInfoPPids(i);}
    _tpids = allocAndCheck<std::vector<PidWithExtraInfo>>(nj);    for (int i = 0; i < nj; ++i) {_tpids[i].resize(npids); setInfoTPids(i);}
    _cpids = allocAndCheck<std::vector<PidWithExtraInfo>>(nj);    for (int i = 0; i < nj; ++i) {_cpids[i].resize(npids); setInfoCPids(i);}
    _vpids = allocAndCheck<std::vector<PidWithExtraInfo>>(nj);    for (int i = 0; i < nj; ++i) {_vpids[i].resize(npids); setInfoVPids(i);}
    _mpids = allocAndCheck<std::vector<PidWithExtraInfo>>(nj);    for (int i = 0; i < nj; ++i) {_mpids[i].resize(npids); setInfoMPids(i);}
    _pdpids = allocAndCheck<std::vector<PidWithExtraInfo>>(nj);    for (int i = 0; i < nj; ++i) {_pdpids[i].resize(npids); setInfoPDPids(i);}
    _vdpids = allocAndCheck<std::vector<PidWithExtraInfo>>(nj);    for (int i = 0; i < nj; ++i) {_vdpids[i].resize(npids); setInfoVDPids(i);}

    _ppids_ena = allocAndCheck<std::vector<bool>>(nj);    for (int i = 0; i < nj; ++i) _ppids_ena[i].resize(npids);
    _tpids_ena = allocAndCheck<std::vector<bool>>(nj);    for (int i = 0; i < nj; ++i) _tpids_ena[i].resize(npids);
    _cpids_ena = allocAndCheck<std::vector<bool>>(nj);    for (int i = 0; i < nj; ++i) _cpids_ena[i].resize(npids);
    _vpids_ena = allocAndCheck<std::vector<bool>>(nj);    for (int i = 0; i < nj; ++i) _vpids_ena[i].resize(npids);
    _mpids_ena = allocAndCheck<std::vector<bool>>(nj);    for (int i = 0; i < nj; ++i) _mpids_ena[i].resize(npids);
    _pdpids_ena = allocAndCheck<std::vector<bool>>(nj);    for (int i = 0; i < nj; ++i) _pdpids_ena[i].resize(npids);
    _vdpids_ena = allocAndCheck<std::vector<bool>>(nj);    for (int i = 0; i < nj; ++i) _vdpids_ena[i].resize(npids);

    _ppids_lim = allocAndCheck<std::vector<double>>(nj);    for (int i = 0; i < nj; ++i) _ppids_lim[i].resize(npids);
    _tpids_lim = allocAndCheck<std::vector<double>>(nj);    for (int i = 0; i < nj; ++i) _tpids_lim[i].resize(npids);
    _cpids_lim = allocAndCheck<std::vector<double>>(nj);    for (int i = 0; i < nj; ++i) _cpids_lim[i].resize(npids);
    _vpids_lim = allocAndCheck<std::vector<double>>(nj);    for (int i = 0; i < nj; ++i) _vpids_lim[i].resize(npids);
    _mpids_lim = allocAndCheck<std::vector<double>>(nj);    for (int i = 0; i < nj; ++i) _mpids_lim[i].resize(npids);
    _pdpids_lim = allocAndCheck<std::vector<double>>(nj);    for (int i = 0; i < nj; ++i) _pdpids_lim[i].resize(npids);
    _vdpids_lim = allocAndCheck<std::vector<double>>(nj);    for (int i = 0; i < nj; ++i) _vdpids_lim[i].resize(npids);

    _ppids_ref = allocAndCheck<std::vector<double>>(nj);    for (int i = 0; i < nj; ++i) _ppids_ref[i].resize(npids);
    _tpids_ref = allocAndCheck<std::vector<double>>(nj);    for (int i = 0; i < nj; ++i) _tpids_ref[i].resize(npids);
    _cpids_ref = allocAndCheck<std::vector<double>>(nj);    for (int i = 0; i < nj; ++i) _cpids_ref[i].resize(npids);
    _vpids_ref = allocAndCheck<std::vector<double>>(nj);    for (int i = 0; i < nj; ++i) _vpids_ref[i].resize(npids);
    _mpids_ref = allocAndCheck<std::vector<double>>(nj);    for (int i = 0; i < nj; ++i) _mpids_ref[i].resize(npids);
    _pdpids_ref = allocAndCheck<std::vector<double>>(nj);    for (int i = 0; i < nj; ++i) _pdpids_ref[i].resize(npids);
    _vdpids_ref = allocAndCheck<std::vector<double>>(nj);    for (int i = 0; i < nj; ++i) _vdpids_ref[i].resize(npids);

    _ppids_ffd = allocAndCheck<std::vector<double>>(nj);    for (int i = 0; i < nj; ++i) _ppids_ffd[i].resize(npids);
    _tpids_ffd = allocAndCheck<std::vector<double>>(nj);    for (int i = 0; i < nj; ++i) _tpids_ffd[i].resize(npids);
    _cpids_ffd = allocAndCheck<std::vector<double>>(nj);    for (int i = 0; i < nj; ++i) _cpids_ffd[i].resize(npids);
    _vpids_ffd = allocAndCheck<std::vector<double>>(nj);    for (int i = 0; i < nj; ++i) _vpids_ffd[i].resize(npids);
    _mpids_ffd = allocAndCheck<std::vector<double>>(nj);    for (int i = 0; i < nj; ++i) _mpids_ffd[i].resize(npids);
    _pdpids_ffd = allocAndCheck<std::vector<double>>(nj);    for (int i = 0; i < nj; ++i) _pdpids_ffd[i].resize(npids);
    _vdpids_ffd = allocAndCheck<std::vector<double>>(nj);    for (int i = 0; i < nj; ++i) _vdpids_ffd[i].resize(npids);

//     _impedance_params=allocAndCheck<ImpedanceParameters>(nj);
//     _impedance_limits=allocAndCheck<ImpedanceLimits>(nj);
    _axisName = new std::string[nj];
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
    _kbemf_scale = allocAndCheck<int>(nj);
    _ktau_scale = allocAndCheck<int>(nj);
    _filterType=allocAndCheck<int>(nj);
    _last_position_move_time=allocAndCheck<double>(nj);
    _viscousPos=allocAndCheck<double>(nj);
    _viscousNeg=allocAndCheck<double>(nj);
    _coulombPos=allocAndCheck<double>(nj);
    _coulombNeg=allocAndCheck<double>(nj);
    _velocityThres=allocAndCheck<double>(nj);

    // Reserve space for data stored locally. values are initialized to 0
    _posCtrl_references = allocAndCheck<double>(nj);
    _posDir_references = allocAndCheck<double>(nj);
    _command_speeds = allocAndCheck<double>(nj);
    _dir_vel_commands = allocAndCheck<double>(nj);
    _ref_speeds = allocAndCheck<double>(nj);
    _ref_accs = allocAndCheck<double>(nj);
    _ref_torques = allocAndCheck<double>(nj);
    _ref_currents = allocAndCheck<double>(nj);
    _enabledAmp = allocAndCheck<bool>(nj);
    _enabledPid = allocAndCheck<bool>(nj);
    _calibrated = allocAndCheck<bool>(nj);
//     _cacheImpedance = allocAndCheck<eOmc_impedance_t>(nj);

    _stiffness = allocAndCheck<double>(nj);
    _damping = allocAndCheck<double>(nj);
    _force_offset = allocAndCheck<double>(nj);

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
    checkAndDestroy(_braked);
    checkAndDestroy(_autobraked);
    checkAndDestroy(_maxTorque);
    checkAndDestroy(_minJntCmdVelocity);
    checkAndDestroy(_maxJntCmdVelocity);
    checkAndDestroy(_maxMotorVelocity);
    checkAndDestroy(_newtonsToSensor);

    checkAndDestroy(_ppids);
    checkAndDestroy(_tpids);
    checkAndDestroy(_cpids);
    checkAndDestroy(_vpids);
    checkAndDestroy(_mpids);
    checkAndDestroy(_pdpids);
    checkAndDestroy(_vdpids);

    checkAndDestroy(_ppids_ena);
    checkAndDestroy(_tpids_ena);
    checkAndDestroy(_cpids_ena);
    checkAndDestroy(_vpids_ena);
    checkAndDestroy(_mpids_ena);
    checkAndDestroy(_pdpids_ena);
    checkAndDestroy(_vdpids_ena);

    checkAndDestroy(_ppids_lim);
    checkAndDestroy(_tpids_lim);
    checkAndDestroy(_cpids_lim);
    checkAndDestroy(_vpids_lim);
    checkAndDestroy(_mpids_lim);
    checkAndDestroy(_pdpids_lim);
    checkAndDestroy(_vdpids_lim);

    checkAndDestroy(_ppids_ref);
    checkAndDestroy(_tpids_ref);
    checkAndDestroy(_cpids_ref);
    checkAndDestroy(_vpids_ref);
    checkAndDestroy(_mpids_ref);
    checkAndDestroy(_pdpids_ref);
    checkAndDestroy(_vdpids_ref);

    checkAndDestroy(_ppids_ffd);
    checkAndDestroy(_tpids_ffd);
    checkAndDestroy(_cpids_ffd);
    checkAndDestroy(_vpids_ffd);
    checkAndDestroy(_mpids_ffd);
    checkAndDestroy(_pdpids_ffd);
    checkAndDestroy(_vdpids_ffd);

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
    checkAndDestroy(_kbemf_scale);
    checkAndDestroy(_ktau_scale);
    checkAndDestroy(_filterType);
    checkAndDestroy(_viscousPos);
    checkAndDestroy(_viscousNeg);
    checkAndDestroy(_coulombPos);
    checkAndDestroy(_coulombNeg);
    checkAndDestroy(_velocityThres);
    checkAndDestroy(_posCtrl_references);
    checkAndDestroy(_posDir_references);
    checkAndDestroy(_command_speeds);
    checkAndDestroy(_dir_vel_commands);
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
    checkAndDestroy(_torques);
    checkAndDestroy(_hwfault_code);
    checkAndDestroy(_hwfault_message);
    checkAndDestroy(_stiffness);
    checkAndDestroy(_damping);
    checkAndDestroy(_force_offset);
    return true;
}

FakeMotionControl::FakeMotionControl() :
    PeriodicThread(0.01),
    ImplementControlCalibration(this),
    ImplementAmplifierControl(this),
    ImplementPidControl(this),
    ImplementEncodersTimed(this),
    ImplementPositionControl(this),
    ImplementVelocityControl(this),
    ImplementVelocityDirect(this),
    ImplementControlMode(this),
    ImplementImpedanceControl(this),
    ImplementMotorEncoders(this),
    ImplementTorqueControl(this),
    ImplementControlLimits(this),
    ImplementPositionDirect(this),
    ImplementInteractionMode(this),
    ImplementCurrentControl(this),
    ImplementPWMControl(this),
    ImplementMotor(this),
    ImplementAxisInfo(this),
    ImplementVirtualAnalogSensor(this),
    ImplementJointFault(this),
    ImplementJointBrake(this),
    _mutex(),
    _njoints       (0),
    _axisMap       (nullptr),
    _angleToEncoder(nullptr),
    _encodersStamp (nullptr),
    _ampsToSensor  (nullptr),
    _dutycycleToPWM(nullptr),
    _DEPRECATED_encoderconversionfactor (nullptr),
    _DEPRECATED_encoderconversionoffset (nullptr),
    _jointEncoderRes        (nullptr),
    _rotorEncoderRes        (nullptr),
    _gearbox                (nullptr),
    _hasHallSensor          (nullptr),
    _hasTempSensor          (nullptr),
    _hasRotorEncoder        (nullptr),
    _hasRotorEncoderIndex   (nullptr),
    _rotorIndexOffset       (nullptr),
    _motorPoles             (nullptr),
    _rotorlimits_max        (nullptr),
    _rotorlimits_min        (nullptr),
    _axisName               (nullptr),
    _jointType              (nullptr),
    _limitsMin              (nullptr),
    _limitsMax              (nullptr),
    _kinematic_mj           (nullptr),
    _maxMotorVelocity       (nullptr),
    _velocityShifts         (nullptr),
    _velocityTimeout        (nullptr),
    _kbemf                  (nullptr),
    _ktau                   (nullptr),
    _kbemf_scale            (nullptr),
    _ktau_scale             (nullptr),
    _viscousPos             (nullptr),
    _viscousNeg             (nullptr),
    _coulombPos             (nullptr),
    _coulombNeg             (nullptr),
    _velocityThres          (nullptr),
    _filterType             (nullptr),
    _torqueSensorId         (nullptr),
    _torqueSensorChan       (nullptr),
    _maxTorque              (nullptr),
    _newtonsToSensor        (nullptr),
    checking_motiondone     (nullptr),
    _last_position_move_time(nullptr),
    _motorPwmLimits         (nullptr),
    _torques                (nullptr),
    useRawEncoderData       (false),
    _pwmIsLimited           (false),
    _torqueControlEnabled   (false),
    _torqueControlUnits     (T_MACHINE_UNITS),
    _positionControlUnits   (P_MACHINE_UNITS),
    prev_time               (0.0),
    opened                  (false),
    verbose                 (VERY_VERBOSE)
{
    resizeBuffers();
    std::string tmp = yarp::conf::environment::get_string("VERBOSE_STICA");
    verbosewhenok = (tmp != "") ? (bool)yarp::conf::numeric::from_string<int>(tmp) :
                                  false;
}

FakeMotionControl::~FakeMotionControl()
{
    yCTrace(FAKEMOTIONCONTROL);
    dealloc();
}

bool FakeMotionControl::initialised()
{
    return opened;
}

bool FakeMotionControl::threadInit()
{
    yCTrace(FAKEMOTIONCONTROL);
    for(int i=0; i<_njoints; i++)
    {
        pwm[i]              = 33+i;
        pwmLimit[i]         = (33+i)*10;
        current[i]          = (33+i)*100;
        maxCurrent[i]       = (33+i)*1000;
        peakCurrent[i]      = (33+i)*2;
        nominalCurrent[i]   = (33+i)*20;
        supplyVoltage[i]    = (33+i)*200;
        last_velocity_command[i] = -1;
        last_pwm_command[i] = -1;
        _controlModes[i]    = VOCAB_CM_POSITION;
        _minJntCmdVelocity[i]=-50.0;
        _maxJntCmdVelocity[i]=50.0;
    }
    prev_time = yarp::os::Time::now();
    return true;
}

void FakeMotionControl::threadRelease()
{
}

bool FakeMotionControl::open(yarp::os::Searchable &config)
{
    if (!this->parseParams(config)) {return false;}

    std::string str;

    //
    //  Read Configuration params from file
    //
    _njoints = m_GENERAL_Joints;

    if(!alloc(_njoints))
    {
        yCError(FAKEMOTIONCONTROL) << "Malloc failed";
        return false;
    }

    // Default value
    for (int i = 0; i < _njoints; i++) {
        _newtonsToSensor[i] = 1;
    }

    if(!fromConfig(config))
    {
        yCError(FAKEMOTIONCONTROL) << "Missing parameters in config file";
        return false;
    }

    //  INIT ALL INTERFACES
    yarp::sig::Vector tmpZeros; tmpZeros.resize (_njoints, 0.0);
    yarp::sig::Vector tmpOnes;  tmpOnes.resize  (_njoints, 1.0);
    yarp::sig::Vector bemfToRaw; bemfToRaw.resize(_njoints, 1.0);
    yarp::sig::Vector ktauToRaw; ktauToRaw.resize(_njoints, 1.0);
    for (size_t i = 0; i < _njoints; i++) { bemfToRaw [i] = _newtonsToSensor[i] / _angleToEncoder[i];}
    for (size_t i = 0; i < _njoints; i++) { ktauToRaw[i]  = _dutycycleToPWM[i]  / _newtonsToSensor[i]; }
    ControlBoardHelper cb(_njoints, _axisMap, _angleToEncoder, nullptr, _newtonsToSensor, _ampsToSensor, _dutycycleToPWM);
    ControlBoardHelper cb_copy_test(cb);
    ImplementControlCalibration::initialize(_njoints, _axisMap, _angleToEncoder, nullptr);
    ImplementAmplifierControl::initialize(_njoints, _axisMap, _angleToEncoder, nullptr);
    ImplementEncodersTimed::initialize(_njoints, _axisMap, _angleToEncoder, nullptr);
    ImplementMotorEncoders::initialize(_njoints, _axisMap, _angleToEncoder, nullptr);
    ImplementPositionControl::initialize(_njoints, _axisMap, _angleToEncoder, nullptr);
    ImplementPidControl::initialize(_njoints, _axisMap, _angleToEncoder, nullptr, _newtonsToSensor, _ampsToSensor, _dutycycleToPWM);
    ImplementPidControl::setConversionUnits(PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_1, PidFeedbackUnitsEnum::METRIC, PidOutputUnitsEnum::DUTYCYCLE_PWM_PERCENT);
    ImplementPidControl::setConversionUnits(PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_2, PidFeedbackUnitsEnum::METRIC, PidOutputUnitsEnum::DUTYCYCLE_PWM_PERCENT);
    ImplementPidControl::setConversionUnits(PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_3, PidFeedbackUnitsEnum::METRIC, PidOutputUnitsEnum::DUTYCYCLE_PWM_PERCENT);
    ImplementPidControl::setConversionUnits(PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_1, PidFeedbackUnitsEnum::METRIC, PidOutputUnitsEnum::DUTYCYCLE_PWM_PERCENT);
    ImplementPidControl::setConversionUnits(PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_2, PidFeedbackUnitsEnum::METRIC, PidOutputUnitsEnum::DUTYCYCLE_PWM_PERCENT);
    ImplementPidControl::setConversionUnits(PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_3, PidFeedbackUnitsEnum::METRIC, PidOutputUnitsEnum::DUTYCYCLE_PWM_PERCENT);
    ImplementPidControl::setConversionUnits(PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_1, PidFeedbackUnitsEnum::METRIC, PidOutputUnitsEnum::DUTYCYCLE_PWM_PERCENT);
    ImplementPidControl::setConversionUnits(PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_2, PidFeedbackUnitsEnum::METRIC, PidOutputUnitsEnum::DUTYCYCLE_PWM_PERCENT);
    ImplementPidControl::setConversionUnits(PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_3, PidFeedbackUnitsEnum::METRIC, PidOutputUnitsEnum::DUTYCYCLE_PWM_PERCENT);
    ImplementPidControl::setConversionUnits(PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_1, PidFeedbackUnitsEnum::METRIC, PidOutputUnitsEnum::DUTYCYCLE_PWM_PERCENT);
    ImplementPidControl::setConversionUnits(PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_2, PidFeedbackUnitsEnum::METRIC, PidOutputUnitsEnum::DUTYCYCLE_PWM_PERCENT);
    ImplementPidControl::setConversionUnits(PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_3, PidFeedbackUnitsEnum::METRIC, PidOutputUnitsEnum::DUTYCYCLE_PWM_PERCENT);
    ImplementPidControl::setConversionUnits(PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_1, PidFeedbackUnitsEnum::METRIC, PidOutputUnitsEnum::DUTYCYCLE_PWM_PERCENT);
    ImplementPidControl::setConversionUnits(PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_2, PidFeedbackUnitsEnum::METRIC, PidOutputUnitsEnum::DUTYCYCLE_PWM_PERCENT);
    ImplementPidControl::setConversionUnits(PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_3, PidFeedbackUnitsEnum::METRIC, PidOutputUnitsEnum::DUTYCYCLE_PWM_PERCENT);
    ImplementPidControl::setConversionUnits(PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_1, PidFeedbackUnitsEnum::METRIC, PidOutputUnitsEnum::DUTYCYCLE_PWM_PERCENT);
    ImplementPidControl::setConversionUnits(PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_2, PidFeedbackUnitsEnum::METRIC, PidOutputUnitsEnum::DUTYCYCLE_PWM_PERCENT);
    ImplementPidControl::setConversionUnits(PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_3, PidFeedbackUnitsEnum::METRIC, PidOutputUnitsEnum::DUTYCYCLE_PWM_PERCENT);
    ImplementPidControl::setConversionUnits(PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_1, PidFeedbackUnitsEnum::METRIC, PidOutputUnitsEnum::DUTYCYCLE_PWM_PERCENT);
    ImplementPidControl::setConversionUnits(PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_2, PidFeedbackUnitsEnum::METRIC, PidOutputUnitsEnum::DUTYCYCLE_PWM_PERCENT);
    ImplementPidControl::setConversionUnits(PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_3, PidFeedbackUnitsEnum::METRIC, PidOutputUnitsEnum::DUTYCYCLE_PWM_PERCENT);
    ImplementControlMode::initialize(_njoints, _axisMap);
    ImplementVelocityControl::initialize(_njoints, _axisMap, _angleToEncoder, nullptr);
    ImplementVelocityDirect::initialize(_njoints, _axisMap, _angleToEncoder, nullptr);
    ImplementControlLimits::initialize(_njoints, _axisMap, _angleToEncoder, nullptr);
    ImplementImpedanceControl::initialize(_njoints, _axisMap, _angleToEncoder, nullptr, _newtonsToSensor);
    ImplementTorqueControl::initialize(_njoints, _axisMap, _angleToEncoder, nullptr, _newtonsToSensor, _ampsToSensor, _dutycycleToPWM, bemfToRaw.data(), ktauToRaw.data());
    ImplementPositionDirect::initialize(_njoints, _axisMap, _angleToEncoder, nullptr);
    ImplementInteractionMode::initialize(_njoints, _axisMap, _angleToEncoder, nullptr);
    ImplementMotor::initialize(_njoints, _axisMap);
    ImplementAxisInfo::initialize(_njoints, _axisMap);
    ImplementPWMControl::initialize(_njoints, _axisMap, _dutycycleToPWM);
    ImplementCurrentControl::initialize(_njoints, _axisMap, _ampsToSensor);
    ImplementVirtualAnalogSensor::initialize(_njoints, _axisMap, _newtonsToSensor);
    ImplementJointFault::initialize(_njoints, _axisMap);
    ImplementJointBrake::initialize(_njoints, _axisMap);

    //start the rateThread
    bool init = this->start();
    if(!init)
    {
        yCError(FAKEMOTIONCONTROL) << "open() has an error in call of FakeMotionControl::init() for board" ;
        return false;
    }
    else
    {
        if(verbosewhenok)
        {
            yCDebug(FAKEMOTIONCONTROL) << "init() has successfully initted board ";
        }
    }
    opened = true;

    return true;
}

bool FakeMotionControl::fromConfig(yarp::os::Searchable &config)
{
    size_t i;

    // AxisMap
    if (!m_GENERAL_AxisMap.empty())
    {
        for (i = 0; i < m_GENERAL_AxisMap.size(); i++) {
            _axisMap[i] = m_GENERAL_AxisMap[i];
        }
    }
    else
    {
        for (i = 0; i < _njoints; i++) {
            _axisMap[i] = i;
        }
    }
    for (i = 0; i < _njoints; i++) {yDebug() << "_axisMap: " << _axisMap[i] << " "; }

    // AxisName
    if (!m_GENERAL_AxisName.empty())
    {
        for (i = 0; i < m_GENERAL_AxisName.size(); i++) {
            _axisName[_axisMap[i]] = m_GENERAL_AxisName[i];
        }
    }
    else
    {
        for (i = 0; i < _njoints; i++) {
            _axisName[_axisMap[i]] = "joint" + toString(i);
        }
    }
    for (i = 0; i < _njoints; i++) { yDebug() << "_axisName: " << _axisName[_axisMap[i]] << " "; }

    // Axis Type
    if (!m_GENERAL_AxisType.empty())
    {
        //beware: axis type has to be remapped here because they are not set using the toHw() helper function
        for (i = 0; i < m_GENERAL_AxisType.size(); i++)
        {
            std::string typeString = m_GENERAL_AxisType[i];
            if (typeString == "revolute") {
                _jointType[_axisMap[i]] = VOCAB_JOINTTYPE_REVOLUTE;
            }
            else if (typeString == "prismatic") {
                _jointType[_axisMap[i]] = VOCAB_JOINTTYPE_PRISMATIC;
            }
            else {
                yCError(FAKEMOTIONCONTROL, "Unknown AxisType value %s!", typeString.c_str());
                _jointType[_axisMap[i]] = VOCAB_JOINTTYPE_UNKNOWN;
                return false;
            }
        }
    }
    else
    {
        yCInfo(FAKEMOTIONCONTROL) << "Using default AxisType (revolute)";
        for (i = 0; i < _njoints; i++)  {
            _jointType[_axisMap[i]] = VOCAB_JOINTTYPE_REVOLUTE;
        }
    }

    // current conversions factor
    if (!m_GENERAL_ampsToSensor.empty())
    {
        for (i = 0; i < m_GENERAL_ampsToSensor.size(); i++) {
            _ampsToSensor[i] = m_GENERAL_ampsToSensor[i];
        }
    }
    else
    {
        yCInfo(FAKEMOTIONCONTROL) << "Using default ampsToSensor=1";
        for (i = 0; i < _njoints; i++) {
            _ampsToSensor[i] = 1;
        }
    }


    // pwm conversions factor
    if (!m_GENERAL_fullscalePWM.empty())
    {
        for (i = 0; i < m_GENERAL_fullscalePWM.size(); i++) {
            _dutycycleToPWM[i] = m_GENERAL_fullscalePWM[i]/100;
        }
    }
    else
    {
        yCInfo(FAKEMOTIONCONTROL) << "Using default fullscalePWM=1";
        for (i = 0; i < _njoints; i++) {
            _dutycycleToPWM[i] = 1;
        }
    }

//     double tmp_A2E;
    // Encoder scales
    if(!m_GENERAL_Encoder.empty())
    {
        for (i = 0; i < m_GENERAL_Encoder.size(); i++) {
            _angleToEncoder[i] = m_GENERAL_Encoder[i]; }
    }
    else
    {
        yCInfo(FAKEMOTIONCONTROL) << "Using default Encoder=1";
        for (i = 0; i < _njoints; i++) {
            _angleToEncoder[i] = 1; }
    }

    /////// LIMITS
    if (!m_LIMITS_Max.empty())
    {
        for (i = 0; i < m_LIMITS_Max.size(); i++) {
            _limitsMax[i] = m_LIMITS_Max[i];
        }
    }
    else
    {
        yCInfo(FAKEMOTIONCONTROL) << "Using default m_LIMITS_Max=100";
        for (i = 0; i < _njoints; i++) {
            _limitsMax[i] = 100;
        }
    }

    if (!m_LIMITS_Min.empty())
    {
        for (i = 0; i < m_LIMITS_Min.size(); i++) {
            _limitsMin[i] = m_LIMITS_Min[i];
        }
    }
    else
    {
        yCInfo(FAKEMOTIONCONTROL) << "Using default m_LIMITS_Min=0";
        for (i = 0; i < _njoints; i++) {
            _limitsMin[i] = 0;
        }
    }

    return true;
}


bool FakeMotionControl::close()
{
    std::lock_guard lock(_mutex);

    this->yarp::os::PeriodicThread::stop();

    yCTrace(FAKEMOTIONCONTROL) << " close()";

    ImplementControlMode::uninitialize();
    ImplementEncodersTimed::uninitialize();
    ImplementMotorEncoders::uninitialize();
    ImplementPositionControl::uninitialize();
    ImplementVelocityControl::uninitialize();
    ImplementVelocityDirect::uninitialize();
    ImplementPidControl::uninitialize();
    ImplementControlCalibration::uninitialize();
    ImplementAmplifierControl::uninitialize();
    ImplementImpedanceControl::uninitialize();
    ImplementControlLimits::uninitialize();
    ImplementTorqueControl::uninitialize();
    ImplementPositionDirect::uninitialize();
    ImplementInteractionMode::uninitialize();
    ImplementAxisInfo::uninitialize();
    ImplementVirtualAnalogSensor::uninitialize();
    ImplementJointFault::uninitialize();
    ImplementJointBrake::uninitialize();

//     cleanup();

    return true;
}

void FakeMotionControl::cleanup()
{

}



///////////// PID INTERFACE

ReturnValue FakeMotionControl::setPidRaw(const PidControlTypeEnum& pidtype, int jnt, const Pid &pid)
{
    yCDebug(FAKEMOTIONCONTROL) << "setPidRaw" << (yarp::conf::vocab32_t)(pidtype) << jnt << pid.kp;
    size_t index = PidControlTypeEnum2Index(pidtype);
    switch (pidtype)
    {
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_3:
           _ppids[jnt].at(index)=pid;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_3:
           _pdpids[jnt].at(index)=pid;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_3:
            _vpids[jnt].at(index)=pid;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_3:
            _vdpids[jnt].at(index)=pid;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_3:
            _mpids[jnt].at(index)=pid;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_3:
            _cpids[jnt].at(index)=pid;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_3:
            _tpids[jnt].at(index)=pid;
        break;
        default:
            yCError(FAKEMOTIONCONTROL) << "Invalid pid type";
            return ReturnValue::return_code::return_value_error_method_failed;
        break;
    }
    return ReturnValue_ok;
}

ReturnValue FakeMotionControl::setPidsRaw(const PidControlTypeEnum& pidtype, const Pid *pids)
{
    yarp::dev::ReturnValue ret = ReturnValue_ok;
    for(int j=0; j< _njoints; j++)
    {
        ret &= setPidRaw(pidtype, j, pids[j]);
    }
    return ret;
}

ReturnValue FakeMotionControl::setPidReferenceRaw(const PidControlTypeEnum& pidtype, int j, double ref)
{
    size_t index = PidControlTypeEnum2Index(pidtype);
    switch (pidtype)
    {
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_3:
            _ppids_ref[j].at(index)=ref;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_3:
            _pdpids_ref[j].at(index)=ref;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_3:
            _vpids_ref[j].at(index)=ref;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_3:
            _vdpids_ref[j].at(index)=ref;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_3:
            _mpids_ref[j].at(index)=ref;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_3:
            _cpids_ref[j].at(index)=ref;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_3:
            _tpids_ref[j].at(index)=ref;
        break;
        default:
            yCError(FAKEMOTIONCONTROL) << "Invalid pid type";
            return ReturnValue::return_code::return_value_error_method_failed;
        break;
    }
    return ReturnValue_ok;
}

ReturnValue FakeMotionControl::setPidReferencesRaw(const PidControlTypeEnum& pidtype, const double *refs)
{
    ReturnValue ret = ReturnValue_ok;
    for(int j=0, index=0; j< _njoints; j++, index++)
    {
        ret &= setPidReferenceRaw(pidtype, j, refs[index]);
    }
    return ret;
}

ReturnValue FakeMotionControl::setPidErrorLimitRaw(const PidControlTypeEnum& pidtype, int j, double limit)
{
    size_t index = PidControlTypeEnum2Index(pidtype);
    switch (pidtype)
    {
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_3:
            _ppids_lim[j].at(index)=limit;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_3:
            _pdpids_lim[j].at(index)=limit;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_3:
            _vpids_lim[j].at(index)=limit;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_3:
            _vdpids_lim[j].at(index)=limit;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_3:
            _mpids_lim[j].at(index)=limit;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_3:
            _cpids_lim[j].at(index)=limit;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_3:
            _tpids_lim[j].at(index)=limit;
        break;
        default:
            yCError(FAKEMOTIONCONTROL) << "Invalid pid type";
            return ReturnValue::return_code::return_value_error_method_failed;
        break;
    }
    return ReturnValue_ok;
}

ReturnValue FakeMotionControl::setPidErrorLimitsRaw(const PidControlTypeEnum& pidtype, const double *limits)
{
    ReturnValue ret = ReturnValue_ok;
    for(int j=0, index=0; j< _njoints; j++, index++)
    {
        ret &= setPidErrorLimitRaw(pidtype, j, limits[index]);
    }
    return ret;
}

ReturnValue FakeMotionControl::getPidErrorRaw(const PidControlTypeEnum& pidtype, int j, double *err)
{
    switch (pidtype)
    {
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_3:
            *err=0.1;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_3:
            *err=0.1;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_3:
            *err=0.2;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_3:
            *err=0.2;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_3:
            *err=0.2;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_3:
            *err=0.3;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_3:
            *err=0.4;
        break;
        default:
            yCError(FAKEMOTIONCONTROL) << "Invalid pid type";
            return ReturnValue::return_code::return_value_error_method_failed;
        break;
    }
    return ReturnValue_ok;
}

ReturnValue FakeMotionControl::getPidErrorsRaw(const PidControlTypeEnum& pidtype, double *errs)
{
    ReturnValue ret = ReturnValue_ok;
    for(int j=0; j< _njoints; j++)
    {
        ret &= getPidErrorRaw(pidtype, j, &errs[j]);
    }
    return ret;
}

ReturnValue FakeMotionControl::getPidRaw(const PidControlTypeEnum& pidtype, int jnt, Pid *pid)
{
    size_t index = PidControlTypeEnum2Index(pidtype);
    switch (pidtype)
    {
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_3:
            *pid=_ppids[jnt].at(index).pid;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_3:
            *pid=_pdpids[jnt].at(index).pid;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_3:
            *pid=_vpids[jnt].at(index).pid;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_3:
            *pid=_vdpids[jnt].at(index).pid;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_3:
            *pid=_mpids[jnt].at(index).pid;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_3:
            *pid=_cpids[jnt].at(index).pid;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_3:
            *pid=_tpids[jnt].at(index).pid;
        break;
        default:
            yCError(FAKEMOTIONCONTROL) << "Invalid pid type";
            return ReturnValue::return_code::return_value_error_method_failed;
        break;
    }
    yCDebug(FAKEMOTIONCONTROL) << "getPidRaw" << (yarp::conf::vocab32_t)(pidtype) << jnt << pid->kp;
    return ReturnValue_ok;
}

ReturnValue FakeMotionControl::getPidsRaw(const PidControlTypeEnum& pidtype, Pid *pids)
{
    ReturnValue ret = ReturnValue_ok;

    for(int j=0, index=0; j<_njoints; j++, index++)
    {
        ret &=getPidRaw(pidtype, j, &pids[j]);
    }
    return ret;
}

ReturnValue FakeMotionControl::getPidReferenceRaw(const PidControlTypeEnum& pidtype, int j, double *ref)
{
    size_t index = PidControlTypeEnum2Index(pidtype);
    switch (pidtype)
    {
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_3:
            *ref=_ppids_ref[j].at(index);
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_3:
            *ref=_pdpids_ref[j].at(index);
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_3:
            *ref=_vpids_ref[j].at(index);
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_3:
            *ref=_vdpids_ref[j].at(index);
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_3:
            *ref=_mpids_ref[j].at(index);
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_3:
            *ref=_cpids_ref[j].at(index);
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_3:
            *ref=_tpids_ref[j].at(index);
        break;
        default:
            yCError(FAKEMOTIONCONTROL) << "Invalid pid type";
            return ReturnValue::return_code::return_value_error_method_failed;
        break;
    }
    return ReturnValue_ok;
}

ReturnValue FakeMotionControl::getPidReferencesRaw(const PidControlTypeEnum& pidtype, double *refs)
{
    ReturnValue ret = ReturnValue_ok;

    // just one joint at time, wait answer before getting to the next.
    // This is because otherwise too many msg will be placed into can queue
    for(int j=0; j< _njoints; j++)
    {
        ret &= getPidReferenceRaw(pidtype, j, &refs[j]);
    }
    return ret;
}

ReturnValue FakeMotionControl::getPidErrorLimitRaw(const PidControlTypeEnum& pidtype, int j, double *limit)
{
    size_t index = PidControlTypeEnum2Index(pidtype);
    switch (pidtype)
    {
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_3:
            *limit=_ppids_lim[j].at(index);
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_3:
            *limit=_pdpids_lim[j].at(index);
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_3:
            *limit=_vpids_lim[j].at(index);
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_3:
            *limit=_vdpids_lim[j].at(index);
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_3:
            *limit=_mpids_lim[j].at(index);
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_3:
            *limit=_cpids_lim[j].at(index);
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_3:
            *limit=_tpids_lim[j].at(index);
        break;
        default:
            yCError(FAKEMOTIONCONTROL) << "Invalid pid type";
            return ReturnValue::return_code::return_value_error_method_failed;
        break;
    }
    return ReturnValue_ok;
}

ReturnValue FakeMotionControl::getPidErrorLimitsRaw(const PidControlTypeEnum& pidtype, double *limits)
{
    ReturnValue ret = ReturnValue_ok;
    for(int j=0, index=0; j<_njoints; j++, index++)
    {
        ret &=getPidErrorLimitRaw(pidtype, j, &limits[j]);
    }
    return ret;
}

ReturnValue FakeMotionControl::resetPidRaw(const PidControlTypeEnum& pidtype, int j)
{
    return ReturnValue_ok;
}

ReturnValue FakeMotionControl::disablePidRaw(const PidControlTypeEnum& pidtype, int j)
{
    size_t index = PidControlTypeEnum2Index(pidtype);
    switch (pidtype)
    {
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_3:
            _ppids_ena[j].at(index)=false;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_3:
            _pdpids_ena[j].at(index)=false;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_3:
            _vpids_ena[j].at(index)=false;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_3:
            _vdpids_ena[j].at(index)=false;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_3:
            _mpids_ena[j].at(index)=false;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_3:
            _cpids_ena[j].at(index)=false;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_3:
            _tpids_ena[j].at(index)=false;
        break;
        default:
            yCError(FAKEMOTIONCONTROL) << "Invalid pid type";
            return ReturnValue::return_code::return_value_error_method_failed;
        break;
    }
    return ReturnValue_ok;
}

ReturnValue FakeMotionControl::enablePidRaw(const PidControlTypeEnum& pidtype, int j)
{
    size_t index = PidControlTypeEnum2Index(pidtype);
    switch (pidtype)
    {
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_3:
            _ppids_ena[j].at(index)=true;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_3:
            _pdpids_ena[j].at(index)=true;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_3:
            _vpids_ena[j].at(index)=true;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_3:
            _vdpids_ena[j].at(index)=true;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_3:
            _mpids_ena[j].at(index)=true;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_3:
            _cpids_ena[j].at(index)=true;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_3:
            _tpids_ena[j].at(index)=true;
        break;
        default:
            yCError(FAKEMOTIONCONTROL) << "Invalid pid type";
            return ReturnValue::return_code::return_value_error_method_failed;
        break;
    }
    return ReturnValue_ok;
}

ReturnValue FakeMotionControl::setPidOffsetRaw(const PidControlTypeEnum& pidtype, int jnt, double v)
{
    yCDebug(FAKEMOTIONCONTROL) << "setPidOffsetRaw" << (yarp::conf::vocab32_t)(pidtype) << jnt << v;
    size_t index = PidControlTypeEnum2Index(pidtype);
    switch (pidtype)
    {
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_3:
            _ppids[jnt].at(index).pid.offset=v;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_3:
            _pdpids[jnt].at(index).pid.offset=v;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_3:
            _vpids[jnt].at(index).pid.offset=v;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_3:
            _vdpids[jnt].at(index).pid.offset=v;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_3:
            _mpids[jnt].at(index).pid.offset=v;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_3:
            _cpids[jnt].at(index).pid.offset=v;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_3:
            _tpids[jnt].at(index).pid.offset=v;
        break;
        default:
            yCError(FAKEMOTIONCONTROL) << "Invalid pid type";
            return ReturnValue::return_code::return_value_error_method_failed;
        break;
    }
    return ReturnValue_ok;
}

ReturnValue FakeMotionControl::getPidOffsetRaw(const PidControlTypeEnum& pidtype, int jnt, double& v)
{
    size_t index = PidControlTypeEnum2Index(pidtype);
    switch (pidtype)
    {
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_3:
            v = _ppids[jnt].at(index).pid.offset;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_3:
            v = _pdpids[jnt].at(index).pid.offset;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_3:
            v = _vpids[jnt].at(index).pid.offset;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_3:
            v = _vdpids[jnt].at(index).pid.offset;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_3:
            v = _mpids[jnt].at(index).pid.offset;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_3:
            v = _cpids[jnt].at(index).pid.offset;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_3:
            v = _tpids[jnt].at(index).pid.offset;
        break;
        default:
            yCError(FAKEMOTIONCONTROL) << "Invalid pid type";
            return ReturnValue::return_code::return_value_error_method_failed;
        break;
    }
    yCDebug(FAKEMOTIONCONTROL) << "getPidOffsetRaw" << (yarp::conf::vocab32_t)(pidtype) << jnt << v;
    return ReturnValue_ok;
}

ReturnValue FakeMotionControl::setPidFeedforwardRaw(const PidControlTypeEnum& pidtype, int jnt, double v)
{
    yCDebug(FAKEMOTIONCONTROL) << "setPidFeedforwardRaw" << (yarp::conf::vocab32_t)(pidtype) << jnt << v;
    size_t index = PidControlTypeEnum2Index(pidtype);
    switch (pidtype)
    {
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_3:
            _ppids_ffd[jnt].at(index)=v;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_3:
            _pdpids_ffd[jnt].at(index)=v;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_3:
            _vpids_ffd[jnt].at(index)=v;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_3:
            _vdpids_ffd[jnt].at(index)=v;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_3:
            _mpids_ffd[jnt].at(index)=v;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_3:
            _cpids_ffd[jnt].at(index)=v;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_3:
            _tpids_ffd[jnt].at(index)=v;
        break;
        default:
            yCError(FAKEMOTIONCONTROL) << "Invalid pid type";
            return ReturnValue::return_code::return_value_error_method_failed;
        break;
    }
    return ReturnValue_ok;
}

ReturnValue FakeMotionControl::getPidFeedforwardRaw(const PidControlTypeEnum& pidtype, int jnt, double& v)
{
    size_t index = PidControlTypeEnum2Index(pidtype);
    switch (pidtype)
    {
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_3:
             v = _ppids_ffd[jnt].at(index);
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_3:
             v = _pdpids_ffd[jnt].at(index);
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_3:
            v = _vpids_ffd[jnt].at(index);
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_3:
            v = _vdpids_ffd[jnt].at(index);
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_3:
            v = _mpids_ffd[jnt].at(index);
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_3:
            v = _cpids_ffd[jnt].at(index);
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_3:
            v = _tpids_ffd[jnt].at(index);
        break;
        default:
            yCError(FAKEMOTIONCONTROL) << "Invalid pid type";
            return ReturnValue::return_code::return_value_error_method_failed;
        break;
    }
    yCDebug(FAKEMOTIONCONTROL) << "getPidFeedforwardRaw" << (yarp::conf::vocab32_t)(pidtype) << jnt << v;
    return ReturnValue_ok;
}

ReturnValue FakeMotionControl::isPidEnabledRaw(const PidControlTypeEnum& pidtype, int j, bool& enabled)
{
    size_t index = PidControlTypeEnum2Index(pidtype);
    switch (pidtype)
    {
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_3:
            enabled=_ppids_ena[j].at(index);
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_3:
            enabled=_pdpids_ena[j].at(index);
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_3:
            enabled=_vpids_ena[j].at(index);
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_3:
            enabled=_vdpids_ena[j].at(index);
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_3:
            enabled=_mpids_ena[j].at(index);
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_3:
            enabled=_cpids_ena[j].at(index);
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_3:
            enabled=_tpids_ena[j].at(index);
        break;
        default:
            yCError(FAKEMOTIONCONTROL) << "Invalid pid type";
            return ReturnValue::return_code::return_value_error_method_failed;
        break;
    }
    return ReturnValue_ok;
}

ReturnValue FakeMotionControl::getPidOutputRaw(const PidControlTypeEnum& pidtype, int j, double *out)
{
    size_t index = PidControlTypeEnum2Index(pidtype);
    switch (pidtype)
    {
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_3:
            *out=1.1 + j * 10;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_3:
            *out=1.1 + j * 10;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_3:
            *out=1.2 + j * 10;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_3:
            *out=1.2 + j * 10;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_3:
            *out=1.2 + j * 10;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_3:
            *out=1.3 + j * 10;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_3:
            *out=1.4 + j * 10;
        break;
        default:
            yCError(FAKEMOTIONCONTROL) << "Invalid pid type";
            return ReturnValue::return_code::return_value_error_method_failed;
        break;
    }
    yCDebug(FAKEMOTIONCONTROL) << "getPidOutputRaw" << (yarp::conf::vocab32_t)(pidtype) << j << *out;
    return ReturnValue_ok;
}

ReturnValue FakeMotionControl::getPidOutputsRaw(const PidControlTypeEnum& pidtype, double *outs)
{
    ReturnValue ret = ReturnValue_ok;
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
          yCError(FAKEMOTIONCONTROL) << "velocityMoveRaw: skipping command because board "  << " joint " << j << " is not in VOCAB_CM_VELOCITY mode";
    }
    _command_speeds[j] = sp;
    last_velocity_command[j]=yarp::os::Time::now();
    return true;
}

bool FakeMotionControl::velocityMoveRaw(const double *sp)
{
    yCTrace(FAKEMOTIONCONTROL);
    bool ret = true;
    for (int i = 0; i < _njoints; i++) {
        ret &= velocityMoveRaw(i, sp[i]);
    }
    return ret;
}


////////////////////////////////////////
//    Calibration control interface   //
////////////////////////////////////////

bool FakeMotionControl::setCalibrationParametersRaw(int j, const CalibrationParameters& params)
{
    yCTrace(FAKEMOTIONCONTROL) << "setCalibrationParametersRaw for joint" << j;
    return true;
}

bool FakeMotionControl::calibrateAxisWithParamsRaw(int j, unsigned int type, double p1, double p2, double p3)
{
    yCTrace(FAKEMOTIONCONTROL) << "calibrateRaw for joint" << j;
    return true;
}

bool FakeMotionControl::calibrationDoneRaw(int axis)
{
    return NOT_YET_IMPLEMENTED("calibrationDoneRaw");
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
    if (verbose >= VERY_VERBOSE) {
        yCTrace(FAKEMOTIONCONTROL) << "j " << j << " ref " << ref;
    }

//     if (yarp::os::Time::now()-_last_position_move_time[j]<MAX_POSITION_MOVE_INTERVAL)
//     {
//         yCWarning(FAKEMOTIONCONTROL) << "Performance warning: You are using positionMove commands at high rate (<"<< MAX_POSITION_MOVE_INTERVAL*1000.0 <<" ms). Probably position control mode is not the right control mode to use.";
//     }
//     _last_position_move_time[j] = yarp::os::Time::now();

    int mode = 0;
    getControlModeRaw(j, &mode);
    if( (mode != VOCAB_CM_POSITION) &&
        (mode != VOCAB_CM_MIXED) &&
        (mode != VOCAB_CM_IMPEDANCE_POS) &&
        (mode != VOCAB_CM_IDLE))
    {
        yCError(FAKEMOTIONCONTROL) << "positionMoveRaw: skipping command because joint " << j << " is not in VOCAB_CM_POSITION mode";
    }
    _posCtrl_references[j] = ref;
    return true;
}

bool FakeMotionControl::positionMoveRaw(const double *refs)
{
    if (verbose >= VERY_VERBOSE) {
        yCTrace(FAKEMOTIONCONTROL);
    }

    bool ret = true;
    for(int j=0, index=0; j< _njoints; j++, index++)
    {
        ret &= positionMoveRaw(j, refs[index]);
    }
    return ret;
}

bool FakeMotionControl::relativeMoveRaw(int j, double delta)
{
    if (verbose >= VERY_VERBOSE) {
        yCTrace(FAKEMOTIONCONTROL) << "j " << j << " ref " << delta;
    }
//     if (yarp::os::Time::now()-_last_position_move_time[j]<MAX_POSITION_MOVE_INTERVAL)
//     {
//         yCWarning(FAKEMOTIONCONTROL) << "Performance warning: You are using positionMove commands at high rate (<"<< MAX_POSITION_MOVE_INTERVAL*1000.0 <<" ms). Probably position control mode is not the right control mode to use.";
//     }
//     _last_position_move_time[j] = yarp::os::Time::now();

    int mode = 0;
    getControlModeRaw(j, &mode);
    if( (mode != VOCAB_CM_POSITION) &&
        (mode != VOCAB_CM_MIXED) &&
        (mode != VOCAB_CM_IMPEDANCE_POS) &&
        (mode != VOCAB_CM_IDLE))
    {
        yCError(FAKEMOTIONCONTROL) << "relativeMoveRaw: skipping command because joint " << j << " is not in VOCAB_CM_POSITION mode";
    }
    _posCtrl_references[j] += delta;
    return true;
}

bool FakeMotionControl::relativeMoveRaw(const double *deltas)
{
    if (verbose >= VERY_VERBOSE) {
        yCTrace(FAKEMOTIONCONTROL);
    }

    bool ret = true;
    for(int j=0, index=0; j< _njoints; j++, index++)
    {
        ret &= relativeMoveRaw(j, deltas[index]);
    }
    return ret;
}


bool FakeMotionControl::checkMotionDoneRaw(int j, bool *flag)
{
    if (verbose >= VERY_VERBOSE) {
        yCTrace(FAKEMOTIONCONTROL) << "j ";
    }

    *flag = false;
    return true;
}

bool FakeMotionControl::checkMotionDoneRaw(bool *flag)
{
    if (verbose >= VERY_VERBOSE) {
        yCTrace(FAKEMOTIONCONTROL);
    }

    bool ret = true;
    bool val, tot_res = true;

    for(int j=0, index=0; j< _njoints; j++, index++)
    {
        ret &= checkMotionDoneRaw(j, &val);
        tot_res &= val;
    }
    *flag = tot_res;
    return ret;
}

bool FakeMotionControl::setTrajSpeedRaw(int j, double sp)
{
    // Velocity is expressed in iDegrees/s
    // save internally the new value of speed; it'll be used in the positionMove
    int index = j ;
    _ref_speeds[index] = sp;
    return true;
}

bool FakeMotionControl::setTrajSpeedsRaw(const double *spds)
{
    // Velocity is expressed in iDegrees/s
    // save internally the new value of speed; it'll be used in the positionMove
    for(int j=0, index=0; j< _njoints; j++, index++)
    {
        _ref_speeds[index] = spds[index];
    }
    return true;
}

bool FakeMotionControl::setTrajAccelerationRaw(int j, double acc)
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

bool FakeMotionControl::setTrajAccelerationsRaw(const double *accs)
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

bool FakeMotionControl::getTrajSpeedRaw(int j, double *spd)
{
    *spd = _ref_speeds[j];
    return true;
}

bool FakeMotionControl::getTrajSpeedsRaw(double *spds)
{
    memcpy(spds, _ref_speeds, sizeof(double) * _njoints);
    return true;
}

bool FakeMotionControl::getTrajAccelerationRaw(int j, double *acc)
{
    *acc = _ref_accs[j];
    return true;
}

bool FakeMotionControl::getTrajAccelerationsRaw(double *accs)
{
    memcpy(accs, _ref_accs, sizeof(double) * _njoints);
    return true;
}

bool FakeMotionControl::stopRaw(int j)
{
    velocityMoveRaw(j,0);
    return true;
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
    if (verbose >= VERY_VERBOSE) {
        yCTrace(FAKEMOTIONCONTROL) << " -> n_joint " << n_joint;
    }

    for(int j=0; j<n_joint; j++)
    {
        yCDebug(FAKEMOTIONCONTROL, "j: %d; ref %f;\n", joints[j], refs[j]); fflush(stdout);
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
    if (verbose >= VERY_VERBOSE) {
        yCTrace(FAKEMOTIONCONTROL) << "n_joint " << _njoints;
    }

    bool ret = true;
    for(int j=0; j<n_joint; j++)
    {
        ret = ret &&relativeMoveRaw(joints[j], deltas[j]);
    }
    return ret;
}

bool FakeMotionControl::checkMotionDoneRaw(const int n_joint, const int *joints, bool *flag)
{
    if (verbose >= VERY_VERBOSE) {
        yCTrace(FAKEMOTIONCONTROL) << "n_joint " << _njoints;
    }

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

bool FakeMotionControl::setTrajSpeedsRaw(const int n_joint, const int *joints, const double *spds)
{
    if (verbose >= VERY_VERBOSE) {
        yCTrace(FAKEMOTIONCONTROL) << "n_joint " << _njoints;
    }

    bool ret = true;
    for(int j=0; j<n_joint; j++)
    {
        ret = ret &&setTrajSpeedRaw(joints[j], spds[j]);
    }
    return ret;
}

bool FakeMotionControl::setTrajAccelerationsRaw(const int n_joint, const int *joints, const double *accs)
{
    if (verbose >= VERY_VERBOSE) {
        yCTrace(FAKEMOTIONCONTROL) << "n_joint " << _njoints;
    }

    bool ret = true;
    for(int j=0; j<n_joint; j++)
    {
        ret = ret &&setTrajAccelerationRaw(joints[j], accs[j]);
    }
    return ret;
}

bool FakeMotionControl::getTrajSpeedsRaw(const int n_joint, const int *joints, double *spds)
{
    if (verbose >= VERY_VERBOSE) {
        yCTrace(FAKEMOTIONCONTROL) << "n_joint " << _njoints;
    }

    bool ret = true;
    for(int j=0; j<n_joint; j++)
    {
        ret = ret && getTrajSpeedRaw(joints[j], &spds[j]);
    }
    return ret;
}

bool FakeMotionControl::getTrajAccelerationsRaw(const int n_joint, const int *joints, double *accs)
{
    if (verbose >= VERY_VERBOSE) {
        yCTrace(FAKEMOTIONCONTROL) << "n_joint " << _njoints;
    }

    bool ret = true;
    for(int j=0; j<n_joint; j++)
    {
        ret = ret && getTrajAccelerationRaw(joints[j], &accs[j]);
    }
    return ret;
}

bool FakeMotionControl::stopRaw(const int n_joint, const int *joints)
{
    if (verbose >= VERY_VERBOSE) {
        yCTrace(FAKEMOTIONCONTROL) << "n_joint " << _njoints;
    }

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
    if (verbose > VERY_VERY_VERBOSE) {
        yCTrace(FAKEMOTIONCONTROL) << "j: " << j;
    }

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

bool FakeMotionControl::setControlModeRaw(const int j, const int _mode)
{
    if (verbose >= VERY_VERBOSE) {
        yCTrace(FAKEMOTIONCONTROL) << "j: " << j << " mode: " << yarp::os::Vocab32::decode(_mode);
    }

    if (_mode==VOCAB_CM_FORCE_IDLE)
    {
        _controlModes[j] = VOCAB_CM_IDLE;
    }
    else
    {
        _controlModes[j] = _mode;
    }
    _posCtrl_references[j] = pos[j];
    return true;
}


bool FakeMotionControl::setControlModesRaw(const int n_joint, const int *joints, int *modes)
{
    if (verbose >= VERY_VERBOSE) {
        yCTrace(FAKEMOTIONCONTROL) << "n_joints: " << n_joint;
    }

    bool ret = true;
    for(int i=0; i<n_joint; i++)
    {
        ret &= setControlModeRaw(joints[i], modes[i]);
    }
    return ret;
}

bool FakeMotionControl::setControlModesRaw(int *modes)
{
    if (verbose >= VERY_VERBOSE) {
        yCTrace(FAKEMOTIONCONTROL);
    }

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
    _mutex.lock();
    for (int i = 0; i < _njoints; i++) {
        stamps[i] = _encodersStamp[i] = _cycleTimestamp;
    }
    _mutex.unlock();
    return ret;
}

bool FakeMotionControl::getEncoderTimedRaw(int j, double *encs, double *stamp)
{
    bool ret = getEncoderRaw(j, encs);
    _mutex.lock();
    *stamp = _encodersStamp[j] = _cycleTimestamp;
    _mutex.unlock();

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
    *value = pos[m]*10;
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
    _mutex.lock();
    for (int i = 0; i < _njoints; i++) {
        stamps[i] = _encodersStamp[i] = _cycleTimestamp;
    }
    _mutex.unlock();

    return ret;
}

bool FakeMotionControl::getMotorEncoderTimedRaw(int m, double *encs, double *stamp)
{
    bool ret = getMotorEncoderRaw(m, encs);
    _mutex.lock();
    *stamp = _encodersStamp[m] = _cycleTimestamp;
    _mutex.unlock();

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

bool FakeMotionControl::setNominalCurrentRaw(int m, const double val)
{
    nominalCurrent[m] = val;
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
ReturnValue FakeMotionControl::setPosLimitsRaw(int j, double min, double max)
{
    _limitsMin[j] = min;
    _limitsMax[j] = max;
    return ReturnValue_ok;
}

ReturnValue FakeMotionControl::getPosLimitsRaw(int j, double *min, double *max)
{
    *min = _limitsMin[j];
    *max = _limitsMax[j];
    return ReturnValue_ok;
}

bool FakeMotionControl::getGearboxRatioRaw(int j, double *gearbox)
{
    return NOT_YET_IMPLEMENTED("getGearboxRatioRaw");
}

bool FakeMotionControl::setGearboxRatioRaw(int m, const double val)
{
    return NOT_YET_IMPLEMENTED("setGearboxRatioRaw");
}

bool FakeMotionControl::getTorqueControlFilterType(int j, int& type)
{
    return NOT_YET_IMPLEMENTED("getTorqueControlFilterType");
}

bool FakeMotionControl::getRotorEncoderResolutionRaw(int j, double &rotres)
{
    return NOT_YET_IMPLEMENTED("getRotorEncoderResolutionRaw");
}

bool FakeMotionControl::getJointEncoderResolutionRaw(int j, double &jntres)
{
    return NOT_YET_IMPLEMENTED("getJointEncoderResolutionRaw");
}

bool FakeMotionControl::getJointEncoderTypeRaw(int j, int &type)
{
    return NOT_YET_IMPLEMENTED("getJointEncoderTypeRaw");
}

bool FakeMotionControl::getRotorEncoderTypeRaw(int j, int &type)
{
    return NOT_YET_IMPLEMENTED("getRotorEncoderTypeRaw");
}

bool FakeMotionControl::getKinematicMJRaw(int j, double &rotres)
{
    return NOT_YET_IMPLEMENTED("getKinematicMJRaw");
}

bool FakeMotionControl::getHasTempSensorsRaw(int j, int& ret)
{
    return NOT_YET_IMPLEMENTED("getHasTempSensorsRaw");
}

bool FakeMotionControl::getHasHallSensorRaw(int j, int& ret)
{
    return NOT_YET_IMPLEMENTED("getHasHallSensorRaw");
}

bool FakeMotionControl::getHasRotorEncoderRaw(int j, int& ret)
{
    return NOT_YET_IMPLEMENTED("getHasRotorEncoderRaw");
}

bool FakeMotionControl::getHasRotorEncoderIndexRaw(int j, int& ret)
{
    return NOT_YET_IMPLEMENTED("getHasRotorEncoderIndexRaw");
}

bool FakeMotionControl::getMotorPolesRaw(int j, int& poles)
{
    return NOT_YET_IMPLEMENTED("getMotorPolesRaw");
}

bool FakeMotionControl::getRotorIndexOffsetRaw(int j, double& rotorOffset)
{
    return NOT_YET_IMPLEMENTED("getRotorIndexOffsetRaw");
}

bool FakeMotionControl::getAxisNameRaw(int axis, std::string& name)
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

// IControlLimits
ReturnValue FakeMotionControl::setVelLimitsRaw(int axis, double min, double max)
{
    _minJntCmdVelocity[axis] = min;
    _maxJntCmdVelocity[axis] = max;
    return ReturnValue_ok;
}

ReturnValue FakeMotionControl::getVelLimitsRaw(int axis, double *min, double *max)
{
    *min = _minJntCmdVelocity[axis];
    *max = _maxJntCmdVelocity[axis];
    return ReturnValue_ok;
}


// Torque control
bool FakeMotionControl::getTorqueRaw(int j, double *t)
{
    *t = _torques[j];
    return true;
}

bool FakeMotionControl::getTorquesRaw(double *t)
{
    for (int i = 0; i < _njoints; i++)
    {
        t[i]= _torques[i];
    }
    return true;
}

bool FakeMotionControl::getTorqueRangeRaw(int j, double *min, double *max)
{
    *min = -100;
    *max = 100;
    return true;
}

bool FakeMotionControl::getTorqueRangesRaw(double *min, double *max)
{
    bool ret = true;
    for (int j = 0; j < _njoints && ret; j++) {
        ret &= getTorqueRangeRaw(j, &min[j], &max[j]);
    }
    return ret;
}

bool FakeMotionControl::setRefTorquesRaw(const double *t)
{
    bool ret = true;
    for (int j = 0; j < _njoints && ret; j++) {
        ret &= setRefTorqueRaw(j, t[j]);
    }
    return ret;
}

bool FakeMotionControl::setRefTorqueRaw(int j, double t)
{
    _mutex.lock();
    _ref_torques[j]=t;

    if (t>1.0 || t< -1.0)
    {
        yCError(FAKEMOTIONCONTROL) << "Joint received a high torque command, and was put in hardware fault";
        _hwfault_code[j] = 1;
        _hwfault_message[j] = "test" + std::to_string(j) + " torque";
        _controlModes[j] =  VOCAB_CM_HW_FAULT;
    }
    _mutex.unlock();
    return true;
}

bool FakeMotionControl::setRefTorquesRaw(const int n_joint, const int *joints, const double *t)
{
    return NOT_YET_IMPLEMENTED("setRefTorquesRaw");
}

bool FakeMotionControl::getRefTorquesRaw(double *t)
{
    bool ret = true;
    for (int j = 0; j < _njoints && ret; j++) {
        ret &= getRefTorqueRaw(j, &_ref_torques[j]);
    }
    return true;
}

bool FakeMotionControl::getRefTorqueRaw(int j, double *t)
{
    _mutex.lock();
    *t = _ref_torques[j];
    _mutex.unlock();
    return true;
}

bool FakeMotionControl::getImpedanceRaw(int j, double *stiffness, double *damping)
{
    _mutex.lock();
    *stiffness = _stiffness[j];
    *damping = _damping[j];
    _mutex.unlock();
    return true;
}

bool FakeMotionControl::setImpedanceRaw(int j, double stiffness, double damping)
{
    _mutex.lock();
    _stiffness[j] = stiffness;
    _damping[j] = damping;
    _mutex.unlock();
    return true;
}

bool FakeMotionControl::setImpedanceOffsetRaw(int j, double offset)
{
    _mutex.lock();
    _force_offset[j] = offset;
    _mutex.unlock();
    return true;
}

bool FakeMotionControl::getImpedanceOffsetRaw(int j, double *offset)
{
    _mutex.lock();
    *offset = _force_offset[j];
    _mutex.unlock();
    return true;
}

bool FakeMotionControl::getCurrentImpedanceLimitRaw(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp)
{
    *min_stiff=1.0;
    *max_stiff=10.0;
    *min_damp=2.0;
    *max_damp=20.0;
    return true;
}

bool FakeMotionControl::getMotorTorqueParamsRaw(int j, MotorTorqueParameters *params)
{
    params->bemf = _kbemf[j];
    params->bemf_scale = _kbemf_scale[j];
    params->ktau = _ktau[j];
    params->ktau_scale = _ktau_scale[j];
    params->viscousPos = _viscousPos[j];
    params->viscousNeg = _viscousNeg[j];
    params->coulombPos = _coulombPos[j];
    params->coulombNeg = _coulombNeg[j];
    params->velocityThres = _velocityThres[j];
    yCDebug(FAKEMOTIONCONTROL) << "getMotorTorqueParamsRaw" << params->bemf
                                                            << params->bemf_scale
                                                            << params->ktau
                                                            << params->ktau_scale
                                                            << params->viscousPos
                                                            << params->viscousNeg
                                                            << params->coulombPos
                                                            << params->coulombNeg
                                                            << params->velocityThres;
    return true;
}

bool FakeMotionControl::setMotorTorqueParamsRaw(int j, const MotorTorqueParameters params)
{
    _kbemf[j] = params.bemf;
    _ktau[j] = params.ktau;
    _kbemf_scale[j] = params.bemf_scale;
    _ktau_scale[j] = params.ktau_scale;
    _viscousPos[j] = params.viscousPos;
    _viscousNeg[j] = params.viscousNeg;
    _coulombPos[j] = params.coulombPos;
    _coulombNeg[j] = params.coulombNeg;
    _velocityThres[j] = params.velocityThres;
    yCDebug(FAKEMOTIONCONTROL) << "setMotorTorqueParamsRaw" << params.bemf
                                                            << params.bemf_scale
                                                            << params.ktau
                                                            << params.ktau_scale
                                                            << params.viscousPos
                                                            << params.viscousNeg
                                                            << params.coulombPos
                                                            << params.coulombNeg
                                                            << params.velocityThres;
    return true;
}

// IVelocityControl interface
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
    return true;
}

bool FakeMotionControl::setPositionsRaw(const int n_joint, const int *joints, const double *refs)
{
    for(int i=0; i< n_joint; i++)
    {
        _posDir_references[joints[i]] = refs[i];
    }
    return true;
}

bool FakeMotionControl::setPositionsRaw(const double *refs)
{
    for(int i=0; i< _njoints; i++)
    {
        _posDir_references[i] = refs[i];
    }
    return true;
}


bool FakeMotionControl::getTargetPositionRaw(int axis, double *ref)
{
    if (verbose >= VERY_VERBOSE) {
        yCTrace(FAKEMOTIONCONTROL) << "j " << axis << " ref " << _posCtrl_references[axis];
    }

    int mode = 0;
    getControlModeRaw(axis, &mode);
    if( (mode != VOCAB_CM_POSITION) &&
        (mode != VOCAB_CM_MIXED) &&
        (mode != VOCAB_CM_IMPEDANCE_POS))
    {
        yCWarning(FAKEMOTIONCONTROL) << "getTargetPosition: Joint " << axis << " is not in POSITION mode, therefore the value returned by " <<
        "this call is for reference only and may not reflect the actual behaviour of the motor/firmware.";
    }
    *ref = _posCtrl_references[axis];
    return true;
}

bool FakeMotionControl::getTargetPositionsRaw(double *refs)
{
    bool ret = true;
    for (int i = 0; i < _njoints; i++) {
        ret &= getTargetPositionRaw(i, &refs[i]);
    }
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

bool FakeMotionControl::getTargetVelocityRaw(int axis, double *ref)
{
    *ref = _command_speeds[axis];
    return true;
}

bool FakeMotionControl::getTargetVelocitiesRaw(double *refs)
{
    bool ret = true;
    for (int i = 0; i<_njoints; i++)
    {
        ret &= getTargetVelocityRaw(i, &refs[i]);
    }
    return ret;
}

bool FakeMotionControl::getTargetVelocitiesRaw(int nj, const int * jnts, double *refs)
{
    bool ret = true;
    for (int i = 0; i<nj; i++)
    {
        ret &= getTargetVelocityRaw(jnts[i], &refs[i]);
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
        yCWarning(FAKEMOTIONCONTROL) << "getRefPosition: Joint " << axis << " is not in POSITION_DIRECT mode, therefore the value returned by \
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
    if (verbose > VERY_VERY_VERBOSE) {
        yCTrace(FAKEMOTIONCONTROL) << "j: " << j;
    }

    *_mode = (yarp::dev::InteractionModeEnum)_interactMode[j];
    return true;}

bool FakeMotionControl::getInteractionModesRaw(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes)
{
    bool ret = true;
    for(int j=0; j< n_joints; j++)
    {
        ret = ret && getInteractionModeRaw(joints[j], &modes[j]);
    }
    return ret;

}

bool FakeMotionControl::getInteractionModesRaw(yarp::dev::InteractionModeEnum* modes)
{
    bool ret = true;
    for (int j = 0; j < _njoints; j++) {
        ret = ret && getInteractionModeRaw(j, &modes[j]);
    }
    return ret;
}

// marco.accame: con alberto cardellino abbiamo parlato della correttezza di effettuare la verifica di quanto imposto (in setInteractionModeRaw() ed affini)
// andando a rileggere il valore nella scheda eth fino a che esso non sia quello atteso. si deve fare oppure no?
// con il interaction mode il can ora non lo fa. mentre lo fa per il control mode. perche' diverso?
bool FakeMotionControl::setInteractionModeRaw(int j, yarp::dev::InteractionModeEnum _mode)
{
    if (verbose >= VERY_VERBOSE) {
        yCTrace(FAKEMOTIONCONTROL) << "j: " << j << " interaction mode: " << yarp::os::Vocab32::decode(_mode);
    }

    _interactMode[j] = _mode;

    return true;
}


bool FakeMotionControl::setInteractionModesRaw(int n_joints, int *joints, yarp::dev::InteractionModeEnum* modes)
{
    bool ret = true;
    for(int i=0; i<n_joints; i++)
    {
        ret &= setInteractionModeRaw(joints[i], modes[i]);
    }
    return ret;
}

bool FakeMotionControl::setInteractionModesRaw(yarp::dev::InteractionModeEnum* modes)
{
    bool ret = true;
    for(int i=0; i<_njoints; i++)
    {
        ret &= setInteractionModeRaw(i, modes[i]);
    }
    return ret;

}

bool FakeMotionControl::getNumberOfMotorsRaw(int* num)
{
    *num=_njoints;
    return true;
}

bool FakeMotionControl::getTemperatureRaw(int m, double* val)
{
    *val = 37.5+double(m);
    return true;
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
    return NOT_YET_IMPLEMENTED("getTemperatureLimitRaw");
}

bool FakeMotionControl::setTemperatureLimitRaw(int m, const double temp)
{
    return NOT_YET_IMPLEMENTED("setTemperatureLimitRaw");
}

//PWM interface
bool FakeMotionControl::setRefDutyCycleRaw(int j, double v)
{
    refpwm[j] = v;
    pwm[j] = v;
    last_pwm_command[j]=yarp::os::Time::now();
    return true;
}

bool FakeMotionControl::setRefDutyCyclesRaw(const double *v)
{
    for (int i = 0; i < _njoints; i++)
    {
        setRefDutyCycleRaw(i,v[i]);
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
    *min = -3.5;
    *max = +3.5;
    return true;
}

bool FakeMotionControl::getCurrentRangesRaw(double *min, double *max)
{
    //just for testing purposes, this is not a real implementation
    for (int i = 0; i < _njoints; i++)
    {
        min[i] = -3.5;
        max[i] = +3.5;
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

yarp::dev::VAS_status  FakeMotionControl::getVirtualAnalogSensorStatusRaw(int ch)
{
    return yarp::dev::VAS_status::VAS_OK;
}

int  FakeMotionControl::getVirtualAnalogSensorChannelsRaw()
{
    return _njoints;
}

bool FakeMotionControl::updateVirtualAnalogSensorMeasureRaw(yarp::sig::Vector &measure)
{
    for (int i = 0; i < _njoints; i++)
    {
        measure[i] = _torques[i];
    }
    return true;
}

bool FakeMotionControl::updateVirtualAnalogSensorMeasureRaw(int ch, double &measure)
{
    _torques[ch] = measure;
    return true;
}

bool FakeMotionControl::getLastJointFaultRaw(int j, int& fault, std::string& message)
{
    _mutex.lock();
    fault = _hwfault_code[j];
    message = _hwfault_message[j];
    _mutex.unlock();
    return true;
}

ReturnValue FakeMotionControl::isJointBrakedRaw(int j, bool& braked) const
{
    braked = _braked[j];
    return ReturnValue_ok;
}

ReturnValue FakeMotionControl::setManualBrakeActiveRaw(int j, bool active)
{
    _braked[j] = active;
    return ReturnValue_ok;
}

ReturnValue FakeMotionControl::setAutoBrakeEnabledRaw(int j, bool enabled)
{
    _autobraked[j] = enabled;
    return ReturnValue_ok;
}

ReturnValue FakeMotionControl::getAutoBrakeEnabledRaw(int j, bool& enabled) const
{
    enabled = _autobraked[j];
    return ReturnValue_ok;
}

ReturnValue FakeMotionControl::getAxes(size_t& axes)
{
    axes = _njoints;
    return ReturnValue_ok;
}

ReturnValue FakeMotionControl::setRefVelocityRaw(int jnt, double vel)
{
    yCTrace(FAKEMOTIONCONTROL);
    if (jnt < 0 || jnt >= _njoints) {
        yCError(FAKEMOTIONCONTROL) << "setRefVelocityRaw: joint index out of bounds";
        return ReturnValue::return_code::return_value_error_method_failed;
    }
    if (vel < -_maxJntCmdVelocity[jnt] || vel > _maxJntCmdVelocity[jnt]) {
        yCError(FAKEMOTIONCONTROL) << "setRefVelocityRaw: velocity out of bounds for joint" << jnt;
        return ReturnValue::return_code::return_value_error_method_failed;
    }
    _mutex.lock();
    _dir_vel_commands[jnt] = vel;
    _mutex.unlock();
    return ReturnValue_ok;
}

ReturnValue FakeMotionControl::setRefVelocityRaw(const std::vector<double>& vels)
{
    yCTrace(FAKEMOTIONCONTROL);
    ReturnValue ret = ReturnValue_ok;
    for (int i = 0; i < _njoints; i++) {
        ret &= setRefVelocityRaw(i, vels[i]);
    }
    return ret;
}

ReturnValue FakeMotionControl::setRefVelocityRaw(const std::vector<int>& jnts, const std::vector<double>& vels)
{
    yCTrace(FAKEMOTIONCONTROL);
    if (jnts.size() != vels.size()) {
        yCError(FAKEMOTIONCONTROL) << "setRefVelocityRaw: jnts and vels vectors must have the same size";
        return ReturnValue::return_code::return_value_error_method_failed;
    }
    ReturnValue ret = ReturnValue_ok;
    for (int i = 0; i < _njoints; i++) {
        ret &= setRefVelocityRaw(jnts[i], vels[i]);
    }
    return ret;
}

ReturnValue FakeMotionControl::getRefVelocityRaw(const int jnt, double& vel)
{
    yCTrace(FAKEMOTIONCONTROL);
    if (jnt < 0 || jnt >= _njoints) {
        yCError(FAKEMOTIONCONTROL) << "setRefVelocityRaw: joint index out of bounds";
        return ReturnValue::return_code::return_value_error_method_failed;
    }
    _mutex.lock();
    vel = _dir_vel_commands[jnt];
    _mutex.unlock();
    return ReturnValue_ok;
}

ReturnValue FakeMotionControl::getRefVelocityRaw(std::vector<double>& vels)
{
    yCTrace(FAKEMOTIONCONTROL);
    ReturnValue ret = ReturnValue_ok;
    vels.resize(_njoints);
    for (int i = 0; i < _njoints; i++) {
        ret &= getRefVelocityRaw(i, vels[i]);
    }
    return ret;
}

ReturnValue FakeMotionControl::getRefVelocityRaw(const std::vector<int>& jnts, std::vector<double>& vels)
{
    yCTrace(FAKEMOTIONCONTROL);
    if (jnts.size() != vels.size()) {
        yCError(FAKEMOTIONCONTROL) << "getRefVelocityRaw: jnts and vels vectors must have the same size";
        return ReturnValue::return_code::return_value_error_method_failed;
    }
    ReturnValue ret = ReturnValue_ok;
    vels.resize(_njoints);
    for (int i = 0; i < _njoints; i++) {
        ret &= getRefVelocityRaw(jnts[i], vels[i]);
    }
    return ret;
}


ReturnValue FakeMotionControl::getPidExtraInfoRaw(const PidControlTypeEnum& pidtype, int j, yarp::dev::PidExtraInfo& units)
{
    yCTrace(FAKEMOTIONCONTROL);
    ReturnValue ret = ReturnValue_ok;

    size_t index = PidControlTypeEnum2Index(pidtype);
    switch (pidtype)
    {
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_3:
            units=_ppids[j].at(index).pidExtraInfo;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_3:
            units=_vpids[j].at(index).pidExtraInfo;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_3:
            units=_cpids[j].at(index).pidExtraInfo;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_3:
            units=_pdpids[j].at(index).pidExtraInfo;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_3:
            units=_vdpids[j].at(index).pidExtraInfo;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_3:
            units=_mpids[j].at(index).pidExtraInfo;
        break;
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_1:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_2:
        case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_3:
            units=_tpids[j].at(index).pidExtraInfo;
        break;
        default:
            yCError(FAKEMOTIONCONTROL) << "Invalid pid type";
            return ReturnValue::return_code::return_value_error_method_failed;
        break;
    }

    return ret;
}


ReturnValue FakeMotionControl::getPidExtraInfosRaw(const PidControlTypeEnum& pidtype, std::vector<yarp::dev::PidExtraInfo>& units)
{
    yCTrace(FAKEMOTIONCONTROL);
    ReturnValue ret = ReturnValue_ok;
    units.resize(_njoints);
    for (int i = 0; i < _njoints; i++) {
        ret &= getPidExtraInfoRaw(pidtype, i, units[i]);
    }
    return ret;
}


/*
bool FakeMotionControl::parseImpedanceGroup_NewFormat(Bottle& pidsGroup, ImpedanceParameters vals[])
{
    int j=0;
    Bottle xtmp;

    if (!extractGroup(pidsGroup, xtmp, "stiffness", "stiffness parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        vals[j].stiffness = xtmp.get(j+1).asFloat64();
    }

    if (!extractGroup(pidsGroup, xtmp, "damping", "damping parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        vals[j].damping = xtmp.get(j+1).asFloat64();
    }

    return true;
}
*/

/*
bool FakeMotionControl::parsePositionPidsGroup(Bottle& pidsGroup, Pid myPid[])
{
    int j=0;
    Bottle xtmp;

    if (!extractGroup(pidsGroup, xtmp, "kp", "Pid kp parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].kp = xtmp.get(j+1).asFloat64();
    }

    if (!extractGroup(pidsGroup, xtmp, "kd", "Pid kd parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].kd = xtmp.get(j+1).asFloat64();
    }

    if (!extractGroup(pidsGroup, xtmp, "ki", "Pid kp parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].ki = xtmp.get(j+1).asFloat64();
    }

    if (!extractGroup(pidsGroup, xtmp, "maxInt", "Pid maxInt parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].max_int = xtmp.get(j+1).asFloat64();
    }

    if (!extractGroup(pidsGroup, xtmp, "maxPwm", "Pid maxPwm parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].max_output = xtmp.get(j+1).asFloat64();
    }

    if (!extractGroup(pidsGroup, xtmp, "shift", "Pid shift parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].scale = xtmp.get(j+1).asFloat64();
    }

    if (!extractGroup(pidsGroup, xtmp, "ko", "Pid ko parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].offset = xtmp.get(j+1).asFloat64();
    }

    if (!extractGroup(pidsGroup, xtmp, "stictionUp", "Pid stictionUp", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].stiction_up_val = xtmp.get(j+1).asFloat64();
    }

    if (!extractGroup(pidsGroup, xtmp, "stictionDwn", "Pid stictionDwn", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].stiction_down_val = xtmp.get(j+1).asFloat64();
    }

    if (!extractGroup(pidsGroup, xtmp, "kff", "Pid kff parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].kff = xtmp.get(j+1).asFloat64();
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
            yCError(FAKEMOTIONCONTROL) << "The PID parameter limPwm was requested but was not correctly set in the configuration file, please fill it.";
            return false;
        }

        yCInfo(FAKEMOTIONCONTROL) << "Using LIMITED PWM!!";
        for (j = 0; j < _njoints; j++) {
            myPid[j].max_output = xtmp.get(j + 1).asFloat64();
        }
    }

    return true;
}
*/

/*
bool FakeMotionControl::parseTorquePidsGroup(Bottle& pidsGroup, Pid myPid[], double kbemf[], double ktau[], int filterType[], double viscousPos[], double viscousNeg[], double coulombPos[], double coulombNeg[], double velocityThres[])
{
    int j=0;
    Bottle xtmp;
    if (!extractGroup(pidsGroup, xtmp, "kp", "Pid kp parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].kp = xtmp.get(j+1).asFloat64();
    }

    if (!extractGroup(pidsGroup, xtmp, "kd", "Pid kd parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].kd = xtmp.get(j+1).asFloat64();
    }

    if (!extractGroup(pidsGroup, xtmp, "ki", "Pid kp parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].ki = xtmp.get(j+1).asFloat64();
    }

    if (!extractGroup(pidsGroup, xtmp, "maxInt", "Pid maxInt parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].max_int = xtmp.get(j+1).asFloat64();
    }

    if (!extractGroup(pidsGroup, xtmp, "maxPwm", "Pid maxPwm parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].max_output = xtmp.get(j+1).asFloat64();
    }

    if (!extractGroup(pidsGroup, xtmp, "shift", "Pid shift parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].scale = xtmp.get(j+1).asFloat64();
    }

    if (!extractGroup(pidsGroup, xtmp, "ko", "Pid ko parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].offset = xtmp.get(j+1).asFloat64();
    }

    if (!extractGroup(pidsGroup, xtmp, "stictionUp", "Pid stictionUp", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].stiction_up_val = xtmp.get(j+1).asFloat64();
    }

    if (!extractGroup(pidsGroup, xtmp, "stictionDwn", "Pid stictionDwn", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].stiction_down_val = xtmp.get(j+1).asFloat64();
    }

    if (!extractGroup(pidsGroup, xtmp, "kff",   "Pid kff parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        myPid[j].kff = xtmp.get(j+1).asFloat64();
    }

    if (!extractGroup(pidsGroup, xtmp, "kbemf", "kbemf parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        kbemf[j] = xtmp.get(j+1).asFloat64();
    }

    if (!extractGroup(pidsGroup, xtmp, "ktau", "ktau parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        ktau[j] = xtmp.get(j+1).asFloat64();
    }

    if (!extractGroup(pidsGroup, xtmp, "filterType", "filterType param", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        filterType[j] = xtmp.get(j+1).asInt32();
    }

    if (!extractGroup(pidsGroup, xtmp, "viscousPos", "viscous pos parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        viscousPos[j] = xtmp.get(j+1).asFloat64();
    }

    if (!extractGroup(pidsGroup, xtmp, "viscousNeg", "viscous neg parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        viscousNeg[j] = xtmp.get(j+1).asFloat64();
    }

    if (!extractGroup(pidsGroup, xtmp, "coulombPos", "coulomb pos parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        coulombPos[j] = xtmp.get(j+1).asFloat64();
    }

    if (!extractGroup(pidsGroup, xtmp, "coulombNeg", "coulomb neg parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        coulombNeg[j] = xtmp.get(j+1).asFloat64();
    }

    if (!extractGroup(pidsGroup, xtmp, "velocityThres", "velocity threshold parameter", _njoints)) {
        return false;
    }
    for (j=0; j<_njoints; j++) {
        velocityThres[j] = xtmp.get(j+1).asFloat64();
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
        if (!extractGroup(pidsGroup, xtmp, "limPwm", "Limited PWM", _njoints))
        {
            yCError(FAKEMOTIONCONTROL) << "The PID parameter limPwm was requested but was not correctly set in the configuration file, please fill it.";
            return false;
        }

        yCInfo(FAKEMOTIONCONTROL) << "Using LIMITED PWM!!";
        for (j = 0; j < _njoints; j++) {
            myPid[j].max_output = xtmp.get(j + 1).asFloat64();
        }
    }

    return true;
}
*/

// eof
