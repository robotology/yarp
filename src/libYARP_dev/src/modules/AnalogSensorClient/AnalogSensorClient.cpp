// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2010 RobotCub Consortium
* Author: Lorenzo Natale
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

#include <AnalogSensorClient.h>

/*! \file AnalogSensorClient.cpp implementation of an analog sensor client class*/

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;


inline void InputPortProcessor::resetStat()
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

InputPortProcessor::InputPortProcessor()
{
    state=IAnalogSensor::AS_ERROR;
    resetStat();
}

void InputPortProcessor::onRead(yarp::sig::Vector &v)
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

    mutex.post();
}

inline int InputPortProcessor::getLast(yarp::sig::Vector &data, Stamp &stmp)
{
    mutex.wait();
    int ret=state;
    if (ret!=IAnalogSensor::AS_ERROR)
    {
        data=lastVector;
        stmp = lastStamp;
    }
    mutex.post();

    return ret;
}

inline int InputPortProcessor::getIterations()
{
    mutex.wait();
    int ret=count;
    mutex.post();
    return ret;
}

// time is in ms
void InputPortProcessor::getEstFrequency(int &ite, double &av, double &min, double &max)
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

        std::cout << "found is " << found <<  "; lenght is : " << name.length() << std::endl;
        // remove leading or trailing '/'
        if( (found == 0) || (found == name.length()-1 ) /*found starts from 0, lenght doesn't*/ )
        {
            done = false;       // we could have both leading and trailing, so let's check again
            name.erase(found,1);
        }
        else
            done = true;        // there is some '/', but their are in the middle and they are allowed
    }

    std::cout << name << std::endl;
}

bool yarp::dev::AnalogSensorClient::open(yarp::os::Searchable &config)
{   
    ConstString carrier = config.check("carrier", Value("udp"), "default carrier for streaming robot state").asString().c_str();

    local.clear();
    remote.clear();

    local  = config.find("local").asString().c_str();
    remote = config.find("remote").asString().c_str();

    if (local=="")
    {
        fprintf(stderr,"AnalogSensorClient::open() error you have to provide valid local name\n");
        return false;
    }
    if (remote=="")
    {
        fprintf(stderr,"AnalogSensorClient::open() error you have to provide valid remote name\n");
        return false;
    }

    if (config.check("period"))
    {
        _rate = config.find("period").asInt();
    }
    else
    {
        _rate = DEFAULT_THREAD_PERIOD;
        std::cout <<"Warning: part "<< deviceId <<" using default period ("<<_rate<<")\n";
    }

    ConstString local_rpc = local;
    local_rpc += "/rpc";
    ConstString remote_rpc = remote;
    remote_rpc += "/rpc";

    if (!inputPort.open(local.c_str()))
    {
        fprintf(stderr,"AnalogSensorClient::open() error could not open port %s, check network\n",local.c_str());
        return false;
    }
    inputPort.useCallback();

    if (!rpcPort.open(local_rpc.c_str()))
    {
        fprintf(stderr,"AnalogSensorClient::open() error could not open rpc port %s, check network\n", local_rpc.c_str());
        return false;
    }

    bool ok=Network::connect(remote.c_str(), local.c_str(), carrier.c_str());
    if (!ok)
    {
        fprintf(stderr,"AnalogSensorClient::open() error could not connect to %s\n", remote.c_str());
        return false;
    }

    ok=Network::connect(local_rpc.c_str(), remote_rpc.c_str());
    if (!ok)
    {
        fprintf(stderr,"AnalogSensorClient::open() error could not connect to %s\n", remote_rpc.c_str());
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
    return CHECK_FAIL(ok, response);
}

int yarp::dev::AnalogSensorClient::calibrateSensor(const yarp::sig::Vector& value)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_IANALOG);
    cmd.addVocab(VOCAB_CALIBRATE);
    Bottle& l = cmd.addList();
    for (int i = 0; i < this->getChannels(); i++)
         l.addDouble(value[i]);
    bool ok = rpcPort.write(cmd, response);
    return CHECK_FAIL(ok, response);
}

int yarp::dev::AnalogSensorClient::calibrateChannel(int ch)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_IANALOG);
    cmd.addVocab(VOCAB_CALIBRATE_CHANNEL);
    cmd.addInt(ch);
    bool ok = rpcPort.write(cmd, response);
    return CHECK_FAIL(ok, response);
}

int yarp::dev::AnalogSensorClient::calibrateChannel(int ch, double value)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_IANALOG);
    cmd.addVocab(VOCAB_CALIBRATE_CHANNEL);
    cmd.addInt(ch);
    cmd.addDouble(value);
    bool ok = rpcPort.write(cmd, response);
    return CHECK_FAIL(ok, response);
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
