/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "AnalogSensorClient.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Value.h>

/*! \file AnalogSensorClient.cpp implementation of an analog sensor client class*/

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;

namespace
{
    inline int checkResponse(bool ok, const yarp::os::Bottle& response)
    {
        const yarp::os::Value & v = response.get(0);
        return ok && v.isInt32() ? v.asInt32() : IAnalogSensor::AS_ERROR;
    }
}

inline void InputPortProcessor::resetStat()
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

InputPortProcessor::InputPortProcessor()
{
    state=IAnalogSensor::AS_ERROR;
    resetStat();
}

void InputPortProcessor::onRead(yarp::sig::Vector &v)
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

        //compare network time
        if (tmpDT*1000<ANALOG_TIMEOUT)
        {
            state=IAnalogSensor::AS_OK;
        }
        else
        {
            state=IAnalogSensor::AS_TIMEOUT;
        }
    }

    prev=now;
    count++;

    lastVector=v;
    Stamp newStamp;
    getEnvelope(newStamp);

    //initialialization (first received data)
    if (lastStamp.isValid()==false)
    {
        lastStamp = newStamp;
    }

    //now compare timestamps
    if ((1000*(newStamp.getTime()-lastStamp.getTime()))<ANALOG_TIMEOUT)
    {
        state=IAnalogSensor::AS_OK;
    }
    else
    {
        state=IAnalogSensor::AS_TIMEOUT;
    }
    lastStamp = newStamp;

    mutex.unlock();
}

inline int InputPortProcessor::getLast(yarp::sig::Vector &data, Stamp &stmp)
{
    mutex.lock();
    int ret=state;
    if (ret!=IAnalogSensor::AS_ERROR)
    {
        data=lastVector;
        stmp = lastStamp;
    }
    mutex.unlock();

    return ret;
}

inline int InputPortProcessor::getIterations()
{
    mutex.lock();
    int ret=count;
    mutex.unlock();
    return ret;
}

// time is in ms
void InputPortProcessor::getEstFrequency(int &ite, double &av, double &min, double &max)
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

int InputPortProcessor::getState()
{
    return state;
}

int InputPortProcessor::getChannels()
{
    if (state==IAnalogSensor::AS_ERROR)
    {
        return 0;
    }
    else
    {
        return (int)lastVector.size();
    }
}

void  yarp::dev::AnalogSensorClient::removeLeadingTrailingSlashesOnly(std::string &name)
{
    bool done = false;
    while(!done)
    {
        std::size_t found = name.find('/');
        // if no '/' found, I'm done
        if (found == std::string::npos)
        {
            done = true;
            continue;
        }

        yDebug() << "found is " << found <<  "; length is : " << name.length();
        // remove leading or trailing '/'
        if( (found == 0) || (found == name.length()-1 ) /*found starts from 0, length doesn't*/ )
        {
            done = false;       // we could have both leading and trailing, so let's check again
            name.erase(found,1);
        }
        else
            done = true;        // there is some '/', but their are in the middle and they are allowed
    }

    yDebug() << name;
}

bool yarp::dev::AnalogSensorClient::open(yarp::os::Searchable &config)
{
    std::string carrier = config.check("carrier", Value("udp"), "default carrier for streaming robot state").asString();

    local.clear();
    remote.clear();

    local  = config.find("local").asString();
    remote = config.find("remote").asString();

    if (local=="")
    {
        yError("AnalogSensorClient::open() error you have to provide valid local name");
        return false;
    }
    if (remote=="")
    {
        yError("AnalogSensorClient::open() error you have to provide valid remote name\n");
        return false;
    }

    std::string local_rpc = local;
    local_rpc += "/rpc:o";
    std::string remote_rpc = remote;
    remote_rpc += "/rpc:i";

    if (!inputPort.open(local))
    {
        yError("AnalogSensorClient::open() error could not open port %s, check network", local.c_str());
        return false;
    }
    inputPort.useCallback();

    if (!rpcPort.open(local_rpc))
    {
        yError("AnalogSensorClient::open() error could not open rpc port %s, check network", local_rpc.c_str());
        return false;
    }

    bool ok=Network::connect(remote.c_str(), local.c_str(), carrier.c_str());
    if (!ok)
    {
        yError("AnalogSensorClient::open() error could not connect to %s", remote.c_str());
        return false;
    }

    ok=Network::connect(local_rpc, remote_rpc);
    if (!ok)
    {
        yError("AnalogSensorClient::open() error could not connect to %s\n", remote_rpc.c_str());
       return false;
    }

    return true;
}

bool yarp::dev::AnalogSensorClient::close()
{
    rpcPort.close();
    inputPort.close();
    return true;
}

int yarp::dev::AnalogSensorClient::read(yarp::sig::Vector &out)
{
    return inputPort.getLast(out, lastTs);
}

int yarp::dev::AnalogSensorClient::getState(int ch)
{
    //not implemented yet
    return AS_OK;
}

int yarp::dev::AnalogSensorClient::getChannels()
{
    return inputPort.getChannels();
}

int yarp::dev::AnalogSensorClient::calibrateSensor()
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_IANALOG);
    cmd.addVocab(VOCAB_CALIBRATE);
    bool ok = rpcPort.write(cmd, response);
    return checkResponse(ok, response);
}

int yarp::dev::AnalogSensorClient::calibrateSensor(const yarp::sig::Vector& value)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_IANALOG);
    cmd.addVocab(VOCAB_CALIBRATE);
    Bottle& l = cmd.addList();
    for (int i = 0; i < this->getChannels(); i++)
         l.addFloat64(value[i]);
    bool ok = rpcPort.write(cmd, response);
    return checkResponse(ok, response);
}

int yarp::dev::AnalogSensorClient::calibrateChannel(int ch)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_IANALOG);
    cmd.addVocab(VOCAB_CALIBRATE_CHANNEL);
    cmd.addInt32(ch);
    bool ok = rpcPort.write(cmd, response);
    return checkResponse(ok, response);
}

int yarp::dev::AnalogSensorClient::calibrateChannel(int ch, double value)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_IANALOG);
    cmd.addVocab(VOCAB_CALIBRATE_CHANNEL);
    cmd.addInt32(ch);
    cmd.addFloat64(value);
    bool ok = rpcPort.write(cmd, response);
    return checkResponse(ok, response);
}

Stamp yarp::dev::AnalogSensorClient::getLastInputStamp()
{
    return lastTs;
}

yarp::dev::DriverCreator *createAnalogSensorClient() {
    return new DriverCreatorOf<AnalogSensorClient>("analogsensorclient",
        "",
        "AnalogSensorClient");
}
