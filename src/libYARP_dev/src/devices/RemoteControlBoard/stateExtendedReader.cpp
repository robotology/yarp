#include <string.h>

#include <yarp/os/PortablePair.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/LogStream.h>

#include <yarp/sig/Vector.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/dev/PreciselyTimed.h>

#include <stateExtendedReader.hpp>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

void StateExtendedInputPort::resetStat()
{
    mutex.wait();
    count=0;
    deltaT=0;
    deltaTMax=0;
    deltaTMin=1e22;
    now=Time::now();
    prev=now;
    mutex.post();
}

StateExtendedInputPort::StateExtendedInputPort()
{
    valid=false;
    resetStat();
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
    last.pidOutput.resize(numberOfJoints);
    last.controlMode.resize(numberOfJoints);
    last.interactionMode.resize(numberOfJoints);
}

void StateExtendedInputPort::onRead(jointData &v)
{
    now=Time::now();
    mutex.wait();

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
    mutex.post();
}

bool StateExtendedInputPort::getLastSingle(int j, int field, double *data, Stamp &stamp, double &localArrivalTime)
{
    mutex.wait();
    bool ret=valid;
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

            case VOCAB_OUTPUT:
                ret = last.pidOutput_isValid;
                *data = last.pidOutput[j];
            break;

            default:
                yError() << "RemoteControlBoard internal error whil reading data. Cannot get 'single' data of type " << yarp::os::Vocab::decode(field);
            break;
        }

        localArrivalTime=now;
        stamp = lastStamp;
    }
    mutex.post();

    return ret;
}

bool StateExtendedInputPort::getLastSingle(int j, int field, int *data, Stamp &stamp, double &localArrivalTime)
{
    mutex.wait();
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
                yError() << "RemoteControlBoard internal error whil reading data. Cannot get 'single' data of type " << yarp::os::Vocab::decode(field);
            break;
        }
        localArrivalTime=now;
        stamp = lastStamp;
    }
    mutex.post();
    return ret;
}

bool StateExtendedInputPort::getLastVector(int field, double* data, Stamp& stamp, double& localArrivalTime)
{
    mutex.wait();
    bool ret = valid;
    if (ret)
    {
        switch(field)
        {
            case VOCAB_ENCODERS:
                ret = last.jointPosition_isValid;
                std::copy(last.jointPosition.begin(), last.jointPosition.end(), data);
            break;

            case VOCAB_ENCODER_SPEEDS:
                ret = last.jointVelocity_isValid;
                std::copy(last.jointVelocity.begin(), last.jointVelocity.end(), data);
            break;

            case VOCAB_ENCODER_ACCELERATIONS:
                ret = last.jointAcceleration_isValid;
                std::copy(last.jointAcceleration.begin(), last.jointAcceleration.end(), data);
            break;

            case VOCAB_MOTOR_ENCODERS:
                ret = last.motorPosition_isValid;
                std::copy(last.motorPosition.begin(), last.motorPosition.end(), data);
            break;

            case VOCAB_MOTOR_ENCODER_SPEEDS:
                ret = last.motorVelocity_isValid;
                std::copy(last.motorVelocity.begin(), last.motorVelocity.end(), data);
            break;

            case VOCAB_MOTOR_ENCODER_ACCELERATIONS:
                ret = last.motorAcceleration_isValid;
                std::copy(last.motorAcceleration.begin(), last.motorAcceleration.end(), data);
            break;

            case VOCAB_TRQS:
                ret = last.torque_isValid;
                std::copy(last.torque.begin(), last.torque.end(), data);
            break;

            case VOCAB_OUTPUTS:
                ret = last.pidOutput_isValid;
                std::copy(last.pidOutput.begin(), last.pidOutput.end(), data);
            break;

            default:
                yError() << "RemoteControlBoard internal error whil reading data. Cannot get 'vector' data of type " << yarp::os::Vocab::decode(field);
            break;
        }

        localArrivalTime=now;
        stamp = lastStamp;
    }
    mutex.post();

    return ret;
}

bool StateExtendedInputPort::getLastVector(int field, int* data, Stamp& stamp, double& localArrivalTime)
{
    mutex.wait();
    bool ret = valid;
    if (ret)
    {
        switch(field)
        {
            case VOCAB_CM_CONTROL_MODES:
                ret = last.controlMode_isValid;
                std::copy(last.controlMode.begin(), last.controlMode.end(), data);
            break;

            case VOCAB_INTERACTION_MODES:
                ret = last.interactionMode_isValid;
                std::copy(last.interactionMode.begin(), last.interactionMode.end(), data);
            break;

            default:
                yError() << "RemoteControlBoard internal error whil reading data. Cannot get 'vector' data of type " << yarp::os::Vocab::decode(field);
            break;
        }
        localArrivalTime=now;
        stamp = lastStamp;
    }
    mutex.post();
    return ret;
}

int StateExtendedInputPort::getIterations()
{
    mutex.wait();
    int ret=count;
    mutex.post();
    return ret;
}

// time is in ms
void StateExtendedInputPort::getEstFrequency(int &ite, double &av, double &min, double &max)
{
    mutex.wait();
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
    mutex.post();
}
