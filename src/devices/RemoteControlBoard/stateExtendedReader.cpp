/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "stateExtendedReader.h"
#include "RemoteControlBoardLogComponent.h"
#include <cstring>

#include <yarp/os/PortablePair.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/LogStream.h>

#include <yarp/sig/Vector.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/dev/IPreciselyTimed.h>


using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

void StateExtendedInputPort::resetStat()
{
    mutex.lock();
    count=0;
    deltaT=0;
    deltaTMax=0;
    deltaTMin=1e22;
    now=Time::now();
    prev=now;
    mutex.unlock();
}

StateExtendedInputPort::StateExtendedInputPort() : deltaT{0},
                                                   deltaTMax{0},
                                                   deltaTMin{1e22},
                                                   now{Time::now()},
                                                   prev{now},
                                                   timeout{0.5},
                                                   valid{false},
                                                   count{0}
{
}

void StateExtendedInputPort::init(int numberOfJoints)
{
    last.jointPosition.resize(numberOfJoints);
    last.jointVelocity.resize(numberOfJoints);
    last.jointAcceleration.resize(numberOfJoints);
    last.motorPosition.resize(numberOfJoints);
    last.motorVelocity.resize(numberOfJoints);
    last.motorAcceleration.resize(numberOfJoints);
    last.torque.resize(numberOfJoints);
    last.pwmDutycycle.resize(numberOfJoints);
    last.current.resize(numberOfJoints);
    last.controlMode.resize(numberOfJoints);
    last.interactionMode.resize(numberOfJoints);
}

void StateExtendedInputPort::onRead(yarp::dev::impl::jointData &v)
{
    now=Time::now();
    mutex.lock();

    if (count>0)
    {
        double tmpDT=now-prev;
        deltaT+=tmpDT;
        if (tmpDT>deltaTMax)
            deltaTMax=tmpDT;
        if (tmpDT<deltaTMin)
            deltaTMin=tmpDT;
    }

    prev=now;
    count++;

    valid=true;
    last=v;
    getEnvelope(lastStamp);
    //check that timestamp are available
    if (!lastStamp.isValid())
        lastStamp.update(now);
    mutex.unlock();
}

void StateExtendedInputPort::setTimeout(const double& timeout) {
    this->timeout = timeout;
}

bool StateExtendedInputPort::getLastSingle(int j, int field, double *data, Stamp &stamp, double &localArrivalTime)
{
    mutex.lock();
    bool ret = valid;
    if (ret)
    {
        switch(field)
        {
            case VOCAB_ENCODER:
                *data = last.jointPosition[j];
                ret  = last.jointPosition_isValid;
            break;

            case VOCAB_ENCODER_SPEED:
                ret = last.jointVelocity_isValid;
                *data = last.jointVelocity[j];
                break;

            case VOCAB_ENCODER_ACCELERATION:
                ret = last.jointAcceleration_isValid;
                *data = last.jointAcceleration[j];
            break;

            case VOCAB_MOTOR_ENCODER:
                ret = last.motorPosition_isValid;
                *data = last.motorPosition[j];
            break;

            case VOCAB_MOTOR_ENCODER_SPEED:
                ret = last.motorVelocity_isValid;
                *data = last.motorVelocity[j];
            break;

            case VOCAB_MOTOR_ENCODER_ACCELERATION:
                ret = last.motorAcceleration_isValid;
                *data = last.motorAcceleration[j];
            break;

            case VOCAB_TRQ:
                ret = last.torque_isValid;
                *data = last.torque[j];
            break;

            case VOCAB_PWMCONTROL_PWM_OUTPUT:
                ret = last.pwmDutycycle_isValid;
                *data = last.pwmDutycycle[j];
            break;

            case VOCAB_AMP_CURRENT:
                ret = last.current_isValid;
                *data = last.current[j];
                break;

            default:
                yCError(REMOTECONTROLBOARD) << "RemoteControlBoard internal error while reading data. Cannot get 'single' data of type " << yarp::os::Vocab32::decode(field);
            break;
        }

        localArrivalTime=now;
        stamp = lastStamp;
        if (ret && ( (Time::now()-localArrivalTime) > timeout) )
            ret = false;
    }
    mutex.unlock();

    return ret;
}

bool StateExtendedInputPort::getLastSingle(int j, int field, int *data, Stamp &stamp, double &localArrivalTime)
{
    mutex.lock();
    bool ret = valid;
    if (ret)
    {
        switch(field)
        {
            case VOCAB_CM_CONTROL_MODE:
                ret = last.controlMode_isValid;
                *data = last.controlMode[j];
            break;

            case VOCAB_INTERACTION_MODE:
                ret = last.interactionMode_isValid;
                *data = last.interactionMode[j];
            break;

            default:
                yCError(REMOTECONTROLBOARD) << "RemoteControlBoard internal error while reading data. Cannot get 'single' data of type " << yarp::os::Vocab32::decode(field);
            break;
        }
        localArrivalTime=now;
        stamp = lastStamp;
        if (ret && ( (Time::now()-localArrivalTime) > timeout) )
            ret = false;

    }
    mutex.unlock();
    return ret;
}

bool StateExtendedInputPort::getLastVector(int field, double* data, Stamp& stamp, double& localArrivalTime)
{
    mutex.lock();
    bool ret = valid;
    if (ret)
    {
        switch(field)
        {
            case VOCAB_ENCODERS:
                ret = last.jointPosition_isValid;
                memcpy(data, last.jointPosition.data(), last.jointPosition.size() * last.jointPosition.getElementSize() );
            break;

            case VOCAB_ENCODER_SPEEDS:
                ret = last.jointVelocity_isValid;
                memcpy(data, last.jointVelocity.data(), last.jointVelocity.size() * last.jointVelocity.getElementSize() );
            break;

            case VOCAB_ENCODER_ACCELERATIONS:
                ret = last.jointAcceleration_isValid;
                memcpy(data, last.jointAcceleration.data(), last.jointAcceleration.size() * last.jointAcceleration.getElementSize() );
            break;

            case VOCAB_MOTOR_ENCODERS:
                ret = last.motorPosition_isValid;
                memcpy(data, last.motorPosition.data(), last.motorPosition.size() * last.motorPosition.getElementSize() );
            break;

            case VOCAB_MOTOR_ENCODER_SPEEDS:
                ret = last.motorVelocity_isValid;
                memcpy(data, last.motorVelocity.data(), last.motorVelocity.size() * last.motorVelocity.getElementSize() );
            break;

            case VOCAB_MOTOR_ENCODER_ACCELERATIONS:
                ret = last.motorAcceleration_isValid;
                memcpy(data, last.motorAcceleration.data(), last.motorAcceleration.size() * last.motorAcceleration.getElementSize() );
            break;

            case VOCAB_TRQS:
                ret = last.torque_isValid;
                memcpy(data, last.torque.data(), last.torque.size() * last.torque.getElementSize() );
            break;

            case VOCAB_PWMCONTROL_PWM_OUTPUTS:
                ret = last.pwmDutycycle_isValid;
                memcpy(data, last.pwmDutycycle.data(), last.pwmDutycycle.size() * last.pwmDutycycle.getElementSize());
            break;

            case VOCAB_AMP_CURRENTS:
                ret = last.current_isValid;
                memcpy(data, last.current.data(), last.current.size() * last.current.getElementSize());
                break;

            default:
                yCError(REMOTECONTROLBOARD) << "RemoteControlBoard internal error while reading data. Cannot get 'vector' data of type " << yarp::os::Vocab32::decode(field);
            break;
        }

        localArrivalTime=now;
        stamp = lastStamp;
        if (ret && ( (Time::now()-localArrivalTime) > timeout) )
            ret = false;
    }
    mutex.unlock();

    return ret;
}

bool StateExtendedInputPort::getLastVector(int field, int* data, Stamp& stamp, double& localArrivalTime)
{
    mutex.lock();
    bool ret = valid;
    if (ret)
    {
        switch(field)
        {
            case VOCAB_CM_CONTROL_MODES:
                ret = last.controlMode_isValid;
                memcpy(data, last.controlMode.data(), last.controlMode.size() * last.controlMode.getElementSize());
            break;

            case VOCAB_INTERACTION_MODES:
                ret = last.interactionMode_isValid;
                memcpy(data, last.interactionMode.data(), last.interactionMode.size() * last.interactionMode.getElementSize());
            break;

            default:
                yCError(REMOTECONTROLBOARD) << "RemoteControlBoard internal error while reading data. Cannot get 'vector' data of type " << yarp::os::Vocab32::decode(field);
            break;
        }
        localArrivalTime=now;
        stamp = lastStamp;
        if (ret && ( (Time::now()-localArrivalTime) > timeout) )
            ret = false;
    }
    mutex.unlock();
    return ret;
}

int StateExtendedInputPort::getIterations()
{
    mutex.lock();
    int ret=count;
    mutex.unlock();
    return ret;
}

// time is in ms
void StateExtendedInputPort::getEstFrequency(int &ite, double &av, double &min, double &max)
{
    mutex.lock();
    ite=count;
    min=deltaTMin*1000;
    max=deltaTMax*1000;
    if (count<1)
    {
        av=0;
    }
    else
    {
        av=deltaT/count;
    }
    av=av*1000;
    mutex.unlock();
}
