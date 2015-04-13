#include <string.h>

#include <yarp/os/PortablePair.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Log.h>

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

bool StateExtendedInputPort::getLast(int j, jointData &data, Stamp &stamp, double &localArrivalTime)
{
    mutex.wait();
    bool ret=valid;
    if (ret)
    {
        data.jointPosition[0]           = last.jointPosition[j];
        data.jointPosition_isValid      = last.jointPosition_isValid;
        data.jointVelocity[0]           = last.jointVelocity[j];
        data.jointVelocity_isValid      = last.jointVelocity_isValid;
        data.jointAcceleration[0]       = last.jointAcceleration[j];
        data.jointAcceleration_isValid  = last.jointAcceleration_isValid;
        data.motorPosition[0]           = last.motorPosition[j];
        data.motorPosition_isValid      = last.motorPosition_isValid;
        data.motorVelocity[0]           = last.motorVelocity[j];
        data.motorVelocity_isValid      = last.motorVelocity_isValid;
        data.motorAcceleration[0]       = last.motorAcceleration[j];
        data.motorAcceleration_isValid  = last.motorAcceleration_isValid;
        data.torque[0]                  = last.torque[j];
        data.torque_isValid             = last.torque_isValid;
        data.pidOutput[0]               = last.pidOutput[j];
        data.pidOutput_isValid          = last.pidOutput_isValid;
        data.controlMode[0]             = last.controlMode[j];
        data.controlMode_isValid        = last.controlMode_isValid;
        data.interactionMode[0]         = last.interactionMode[j];
        data.interactionMode_isValid    = last.interactionMode_isValid;

        stamp=lastStamp;
        localArrivalTime=now;
    }
    mutex.post();
    return ret;
}

bool StateExtendedInputPort::getLast(jointData &lastData, Stamp &stmp, double &localArrivalTime)
{
    mutex.wait();
    bool ret = valid;
    if (ret)
    {
        lastData = last;
        stmp = lastStamp;
        localArrivalTime=now;
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
