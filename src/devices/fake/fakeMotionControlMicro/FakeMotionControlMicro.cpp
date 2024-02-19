/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FakeMotionControlMicro.h"

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
YARP_LOG_COMPONENT(FAKEMOTIONCONTROLMICRO, "yarp.device.fakeMotionControlMicro")
}

void FakeMotionControlMicro::run()
{
    std::lock_guard lock(_mutex);

    for (int i=0;i <_njoints ;i++)
    {
    }
    prev_time = yarp::os::Time::now();
}

static inline bool NOT_YET_IMPLEMENTED(const char *txt)
{
    yCError(FAKEMOTIONCONTROLMICRO) << txt << "is not yet implemented";
    return true;
}

static inline bool DEPRECATED(const char *txt)
{
    yCError(FAKEMOTIONCONTROLMICRO) << txt << "has been deprecated";
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
bool FakeMotionControlMicro::extractGroup(Bottle &input, Bottle &out, const std::string &key1, const std::string &txt, int size)
{
    size++;
    Bottle &tmp=input.findGroup(key1, txt);

    if (tmp.isNull())
    {
        yCError(FAKEMOTIONCONTROLMICRO) << key1.c_str() << "parameter not found";
        return false;
    }

    if(tmp.size()!=(size_t) size)
    {
        yCError(FAKEMOTIONCONTROLMICRO) << key1.c_str() << "incorrect number of entries";
        return false;
    }

    out=tmp;
    return true;
}

void FakeMotionControlMicro::resizeBuffers()
{
    pos.resize(_njoints);
    dpos.resize(_njoints);
    vel.resize(_njoints);
    speed.resize(_njoints);
    acc.resize(_njoints);
    loc.resize(_njoints);
    amp.resize(_njoints);

    pos.zero();
    dpos.zero();
    vel.zero();
    speed.zero();
    acc.zero();
    loc.zero();
    amp.zero();
}

bool FakeMotionControlMicro::alloc(int nj)
{
    _axisMap = allocAndCheck<int>(nj);
    _controlModes = allocAndCheck<int>(nj);
    _angleToEncoder = allocAndCheck<double>(nj);
    _encodersStamp = allocAndCheck<double>(nj);
    _hwfault_code = allocAndCheck<int>(nj);
    _hwfault_message = allocAndCheck<std::string>(nj);

    _axisName = new std::string[nj];
    _jointType = new JointTypeEnum[nj];

    resizeBuffers();

    return true;
}

bool FakeMotionControlMicro::dealloc()
{
    checkAndDestroy(_axisMap);
    checkAndDestroy(_controlModes);
    checkAndDestroy(_angleToEncoder);
    checkAndDestroy(_encodersStamp);

    checkAndDestroy(_axisName);
    checkAndDestroy(_jointType);

    checkAndDestroy(_hwfault_code);
    checkAndDestroy(_hwfault_message);

    return true;
}

FakeMotionControlMicro::FakeMotionControlMicro() :
    PeriodicThread(0.01),
    ImplementEncodersTimed(this),
    ImplementMotorEncoders(this),
    ImplementAxisInfo(this),
    ImplementJointFault(this),
    _mutex(),
    _njoints       (0),
    _axisMap       (nullptr),
    _angleToEncoder(nullptr),
    _encodersStamp (nullptr),
    _axisName               (nullptr),
    _jointType              (nullptr),
    useRawEncoderData       (false),
    prev_time               (0.0),
    opened                  (false),
    verbose                 (VERY_VERBOSE)
{
    resizeBuffers();
    std::string tmp = yarp::conf::environment::get_string("VERBOSE_STICA");
    verbosewhenok = (tmp != "") ? (bool)yarp::conf::numeric::from_string<int>(tmp) :
                                  false;
}

FakeMotionControlMicro::~FakeMotionControlMicro()
{
    yCTrace(FAKEMOTIONCONTROLMICRO);
    dealloc();
}

bool FakeMotionControlMicro::initialised()
{
    return opened;
}

bool FakeMotionControlMicro::threadInit()
{
    yCTrace(FAKEMOTIONCONTROLMICRO);
    for(int i=0; i<_njoints; i++)
    {
        _controlModes[i]    = VOCAB_CM_POSITION;
    }
    prev_time = yarp::os::Time::now();
    return true;
}

void FakeMotionControlMicro::threadRelease()
{
}

bool FakeMotionControlMicro::open(yarp::os::Searchable &config)
{
    if (!this->parseParams(config)) {return false;}

    std::string str;

    //
    //  Read Configuration params from file
    //
    _njoints = m_GENERAL_joints;

    if(!alloc(_njoints))
    {
        yCError(FAKEMOTIONCONTROLMICRO) << "Malloc failed";
        return false;
    }

    if(!fromConfig(config))
    {
        yCError(FAKEMOTIONCONTROLMICRO) << "Missing parameters in config file";
        return false;
    }

    //  INIT ALL INTERFACES
    yarp::sig::Vector tmpZeros; tmpZeros.resize (_njoints, 0.0);
    yarp::sig::Vector tmpOnes;  tmpOnes.resize  (_njoints, 1.0);
    ControlBoardHelper cb(_njoints, _axisMap, _angleToEncoder, nullptr, nullptr, nullptr, nullptr);
    ControlBoardHelper cb_copy_test(cb);
    ImplementEncodersTimed::initialize(_njoints, _axisMap, _angleToEncoder, nullptr);
    ImplementMotorEncoders::initialize(_njoints, _axisMap, _angleToEncoder, nullptr);
    ImplementAxisInfo::initialize(_njoints, _axisMap);
    ImplementJointFault::initialize(_njoints, _axisMap);

    //start the rateThread
    bool init = this->start();
    if(!init)
    {
        yCError(FAKEMOTIONCONTROLMICRO) << "open() has an error in call of FakeMotionControlMicro::init() for board" ;
        return false;
    }
    else
    {
        if(verbosewhenok)
        {
            yCDebug(FAKEMOTIONCONTROLMICRO) << "init() has successfully initialized board ";
        }
    }
    opened = true;

    return true;
}

bool FakeMotionControlMicro::fromConfig(yarp::os::Searchable &config)
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
                yCError(FAKEMOTIONCONTROLMICRO, "Unknown AxisType value %s!", typeString.c_str());
                _jointType[_axisMap[i]] = VOCAB_JOINTTYPE_UNKNOWN;
                return false;
            }
        }
    }
    else
    {
        yCInfo(FAKEMOTIONCONTROLMICRO) << "Using default AxisType (revolute)";
        for (i = 0; i < _njoints; i++)  {
            _jointType[_axisMap[i]] = VOCAB_JOINTTYPE_REVOLUTE;
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
        yCInfo(FAKEMOTIONCONTROLMICRO) << "Using default Encoder=1";
        for (i = 0; i < _njoints; i++) {
            _angleToEncoder[i] = 1; }
    }

    return true;
}


bool FakeMotionControlMicro::close()
{
    std::lock_guard lock(_mutex);

    this->yarp::os::PeriodicThread::stop();

    yCTrace(FAKEMOTIONCONTROLMICRO) << " close()";

    ImplementEncodersTimed::uninitialize();
    ImplementMotorEncoders::uninitialize();
    ImplementAxisInfo::uninitialize();

//     cleanup();

    return true;
}

void FakeMotionControlMicro::cleanup()
{

}

//////////////////////// BEGIN EncoderInterface

bool FakeMotionControlMicro::setEncoderRaw(int j, double val)
{
    return NOT_YET_IMPLEMENTED("setEncoder");
}

bool FakeMotionControlMicro::setEncodersRaw(const double *vals)
{
    return NOT_YET_IMPLEMENTED("setEncoders");
}

bool FakeMotionControlMicro::resetEncoderRaw(int j)
{
    return NOT_YET_IMPLEMENTED("resetEncoder");
}

bool FakeMotionControlMicro::resetEncodersRaw()
{
    return NOT_YET_IMPLEMENTED("resetEncoders");
}

bool FakeMotionControlMicro::getEncoderRaw(int j, double *value)
{
    bool ret = true;

    // To simulate a real controlboard, we assume that the joint
    // encoders is exactly the last set by setPosition(s) or positionMove
    *value = pos[j];

    return ret;
}

bool FakeMotionControlMicro::getEncodersRaw(double *encs)
{
    bool ret = true;
    for(int j=0; j< _njoints; j++)
    {
        bool ok = getEncoderRaw(j, &encs[j]);
        ret = ret && ok;

    }
    return ret;
}

bool FakeMotionControlMicro::getEncoderSpeedRaw(int j, double *sp)
{
    // To avoid returning uninitialized memory, we set the encoder speed to 0
    *sp = 0.0;
    return true;
}

bool FakeMotionControlMicro::getEncoderSpeedsRaw(double *spds)
{
    bool ret = true;
    for(int j=0; j< _njoints; j++)
    {
        ret &= getEncoderSpeedRaw(j, &spds[j]);
    }
    return ret;
}

bool FakeMotionControlMicro::getEncoderAccelerationRaw(int j, double *acc)
{
    // To avoid returning uninitialized memory, we set the encoder acc to 0
    *acc = 0.0;

    return true;
}

bool FakeMotionControlMicro::getEncoderAccelerationsRaw(double *accs)
{
    bool ret = true;
    for(int j=0; j< _njoints; j++)
    {
        ret &= getEncoderAccelerationRaw(j, &accs[j]);
    }
    return ret;
}

///////////////////////// END Encoder Interface

bool FakeMotionControlMicro::getEncodersTimedRaw(double *encs, double *stamps)
{
    bool ret = getEncodersRaw(encs);
    _mutex.lock();
    for (int i = 0; i < _njoints; i++) {
        stamps[i] = _encodersStamp[i];
    }
    _mutex.unlock();
    return ret;
}

bool FakeMotionControlMicro::getEncoderTimedRaw(int j, double *encs, double *stamp)
{
    bool ret = getEncoderRaw(j, encs);
    _mutex.lock();
    *stamp = _encodersStamp[j];
    _mutex.unlock();

    return ret;
}

//////////////////////// BEGIN EncoderInterface

bool FakeMotionControlMicro::getNumberOfMotorEncodersRaw(int* num)
{
    *num=_njoints;
    return true;
}

bool FakeMotionControlMicro::setMotorEncoderRaw(int m, const double val)
{
    return NOT_YET_IMPLEMENTED("setMotorEncoder");
}

bool FakeMotionControlMicro::setMotorEncodersRaw(const double *vals)
{
    return NOT_YET_IMPLEMENTED("setMotorEncoders");
}

bool FakeMotionControlMicro::setMotorEncoderCountsPerRevolutionRaw(int m, const double cpr)
{
    return NOT_YET_IMPLEMENTED("setMotorEncoderCountsPerRevolutionRaw");
}

bool FakeMotionControlMicro::getMotorEncoderCountsPerRevolutionRaw(int m, double *cpr)
{
    return NOT_YET_IMPLEMENTED("getMotorEncoderCountsPerRevolutionRaw");
}

bool FakeMotionControlMicro::resetMotorEncoderRaw(int mj)
{
    return NOT_YET_IMPLEMENTED("resetMotorEncoder");
}

bool FakeMotionControlMicro::resetMotorEncodersRaw()
{
    return NOT_YET_IMPLEMENTED("reseMotortEncoders");
}

bool FakeMotionControlMicro::getMotorEncoderRaw(int m, double *value)
{
    *value = pos[m]*10;
    return true;
}

bool FakeMotionControlMicro::getMotorEncodersRaw(double *encs)
{
    bool ret = true;
    for(int j=0; j< _njoints; j++)
    {
        ret &= getMotorEncoderRaw(j, &encs[j]);

    }
    return ret;
}

bool FakeMotionControlMicro::getMotorEncoderSpeedRaw(int m, double *sp)
{
    *sp = 0.0;
    return true;
}

bool FakeMotionControlMicro::getMotorEncoderSpeedsRaw(double *spds)
{
    bool ret = true;
    for(int j=0; j< _njoints; j++)
    {
        ret &= getMotorEncoderSpeedRaw(j, &spds[j]);
    }
    return ret;
}

bool FakeMotionControlMicro::getMotorEncoderAccelerationRaw(int m, double *acc)
{
    *acc = 0.0;
    return true;
}

bool FakeMotionControlMicro::getMotorEncoderAccelerationsRaw(double *accs)
{
    bool ret = true;
    for(int j=0; j< _njoints; j++)
    {
        ret &= getMotorEncoderAccelerationRaw(j, &accs[j]);
    }
    return ret;
}

bool FakeMotionControlMicro::getMotorEncodersTimedRaw(double *encs, double *stamps)
{
    bool ret = getMotorEncodersRaw(encs);
    _mutex.lock();
    for (int i = 0; i < _njoints; i++) {
        stamps[i] = _encodersStamp[i];
    }
    _mutex.unlock();

    return ret;
}

bool FakeMotionControlMicro::getMotorEncoderTimedRaw(int m, double *encs, double *stamp)
{
    bool ret = getMotorEncoderRaw(m, encs);
    _mutex.lock();
    *stamp = _encodersStamp[m];
    _mutex.unlock();

    return ret;
}
///////////////////////// END Motor Encoder Interface


bool FakeMotionControlMicro::getAxisNameRaw(int axis, std::string& name)
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

bool FakeMotionControlMicro::getJointTypeRaw(int axis, yarp::dev::JointTypeEnum& type)
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

bool FakeMotionControlMicro::getLastJointFaultRaw(int j, int& fault, std::string& message)
{
    _mutex.lock();
    fault = _hwfault_code[j];
    message = _hwfault_message[j];
    _mutex.unlock();
    return true;
}

bool FakeMotionControlMicro::getAxes(int* ax)
{
    *ax = _njoints;
    return true;
}
// eof
