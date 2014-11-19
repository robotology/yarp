// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2010 RobotCub Consortium
* Author: Lorenzo Natale
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/


/*! \file AnalogSensorClient.cpp implementation of an analog sensor client class*/

#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/dev/IAnalogSensor.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/dev/PolyDriver.h>

namespace yarp {
    namespace dev {
        class AnalogSensorClient;
    }
}

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;

const int ANALOG_TIMEOUT=100; //ms

class InputPortProcessor:public BufferedPort<yarp::sig::Vector>
{
    yarp::sig::Vector lastVector;
    Semaphore mutex;
    Stamp lastStamp;
    double deltaT;
    double deltaTMax;
    double deltaTMin;
    double prev;
    double now;

    int state;
    int count;

public:

    inline void resetStat()
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

    InputPortProcessor()
    {
        state=IAnalogSensor::AS_ERROR;
        resetStat();
    }

    virtual void onRead(yarp::sig::Vector &v)
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

    inline int getLast(yarp::sig::Vector &data, Stamp &stmp)
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

    inline int getIterations()
    {
        mutex.wait();
        int ret=count;
        mutex.post();
        return ret;
    }

    // time is in ms
    void getEstFrequency(int &ite, double &av, double &min, double &max)
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

    int getState()
    { return state; }

    int getChannels()
    {
        if (state==IAnalogSensor::AS_ERROR)
        {
            return 0;
        }
        else
            return (int)lastVector.size();
    }

};



/**
* @ingroup dev_impl_wrapper
*
* The client side of any IAnalogSensor capable device.
* Still single thread! concurrent access is unsafe.
*/
class yarp::dev::AnalogSensorClient: public DeviceDriver,
                          public IPreciselyTimed,
                          public IAnalogSensor
{
protected:
    InputPortProcessor inputPort;
    Port rpcPort;
    ConstString local;
    ConstString remote;
    Stamp lastTs; //used by IPreciselyTimed
public:

    /* DevideDriver methods */
    bool open(Searchable& config);
    bool close();

    /* IAnalogSensor methods*/
    int read(yarp::sig::Vector &out);

    /* Check the state value of a given channel.
    * @param ch: channel number.
    * @return status.
    */
    int getState(int ch);

    /* Get the number of channels of the sensor.
     * @return number of channels (0 in case of errors).
     */
    int getChannels();

    /* Calibrates the whole sensor.
     * @return status.
     */
    int calibrateSensor();

    /* Calibrates the whole sensor, using a vector of calibration values.
     * @param value: a vector of calibration values.
     * @return status.
     */
    int calibrateSensor(const yarp::sig::Vector& value);

    /* Calibrates one single channel.
     * @param ch: channel number.
     * @return status.
     */
    int calibrateChannel(int ch);

    /* Calibrates one single channel.
     * @param ch: channel number.
     * @param value: calibration value.
     * @return status.
     */
    int calibrateChannel(int ch, double value);

    /* IPreciselyTimed methods */
    /**
    * Get the time stamp for the last read data
    * @return last time stamp.
    */
    Stamp getLastInputStamp();
};

bool yarp::dev::AnalogSensorClient::open(yarp::os::Searchable &config)
{
    remote = config.find("remote").asString().c_str();
    local = config.find("local").asString().c_str();

    ConstString carrier = config.check("carrier",
        Value("udp"),
        "default carrier for streaming robot state").asString().c_str();

    if (local=="")
    {
        yError("AnalogSensorClient: you have to provide valid local name");
        return false;
    }
    if (remote=="")
    {
        yError("AnalogSensorClient: you have to provide valid remote name");
        return false;
    }

    ConstString local_rpc = local;
    local_rpc += "/rpc:o";
    ConstString remote_rpc = remote;
    remote_rpc += "/rpc:i";

    if (!inputPort.open(local.c_str()))
    {
        yError("AnalogSensorClient: could not open port %s, check network",local.c_str());
        return false;
    }
    inputPort.useCallback();

    if (!rpcPort.open(local_rpc.c_str()))
    {
        yError("AnalogSensorClient: could not open rpc port %s, check network", local_rpc.c_str());
        return false;
    }

    bool ok=Network::connect(remote.c_str(), local.c_str(), carrier.c_str());
    if (!ok)
    {
        yError("AnalogSensorClient: could not connect to %s", remote.c_str());
        return false;
    }

    ok=Network::connect(local_rpc.c_str(), remote_rpc.c_str());
    if (!ok)
    {
       yError("AnalogSensorClient: could not connect to %s", remote_rpc.c_str());
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
