// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
* Author: Marco Randazzo <marco.randazzo@iit.it>
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

#include "laserRangefinder2DClient.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

/*! \file laserRangefinder2DClient.cpp */

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;


inline void  LaserRangefinder2DInputPortProcessor::resetStat()
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

LaserRangefinder2DInputPortProcessor::LaserRangefinder2DInputPortProcessor()
{
    state = ILaserRangefinder2D::LASER_GENERAL_ERROR;
    resetStat();
}

void LaserRangefinder2DInputPortProcessor::onRead(yarp::os::Bottle &b)
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
        if (tmpDT*1000<LASER_TIMEOUT)
        {
            state = b.get(5).asInt();
        }
        else
        {
            state = ILaserRangefinder2D::LASER_TIMEOUT;
        }
    }

    prev=now;
    count++;

    lastBottle=b;
    Stamp newStamp;
    getEnvelope(newStamp);

    //initialialization (first received data)
    if (lastStamp.isValid()==false)
    {
        lastStamp = newStamp;
    }

    //now compare timestamps
    if ((1000*(newStamp.getTime()-lastStamp.getTime()))<LASER_TIMEOUT)
    {
        state = b.get(5).asInt();
    }
    else
    {
        state = ILaserRangefinder2D::LASER_TIMEOUT;
    }
    lastStamp = newStamp;

    mutex.post();
}

inline int LaserRangefinder2DInputPortProcessor::getLast(yarp::os::Bottle &data, Stamp &stmp)
{
    mutex.wait();
    int ret=state;
    if (ret != ILaserRangefinder2D::LASER_GENERAL_ERROR)
    {
        data=lastBottle;
        stmp = lastStamp;
    }
    mutex.post();

    return ret;
}

bool LaserRangefinder2DInputPortProcessor::getData(yarp::sig::Vector &ranges)
{
    mutex.wait();
    ranges.resize(10);
    for (int i = 0; i < 1; i++)
    {
        ranges[0] = lastBottle.get(0).asInt();
    }
    mutex.post();
    return true;
}

int    LaserRangefinder2DInputPortProcessor::getStatus()
{
    mutex.wait();
    int status = lastBottle.get(3).asInt();
    mutex.post();
    return status;
}

inline int LaserRangefinder2DInputPortProcessor::getIterations()
{
    mutex.wait();
    int ret=count;
    mutex.post();
    return ret;
}

// time is in ms
void LaserRangefinder2DInputPortProcessor::getEstFrequency(int &ite, double &av, double &min, double &max)
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

bool yarp::dev::LaserRangefinder2DClient::open(yarp::os::Searchable &config)
{
    local.clear();
    remote.clear();

    local  = config.find("local").asString().c_str();
    remote = config.find("remote").asString().c_str();

    if (local=="")
    {
        yError("LaserRangefinder2DClient::open() error you have to provide valid local name");
        return false;
    }
    if (remote=="")
    {
        yError("LaserRangefinder2DClient::open() error you have to provide valid remote name");
        return false;
    }

    if (config.check("period"))
    {
        _rate = config.find("period").asInt();
    }
    else
    {
        yError("LaserRangefinder2DClient::open() missing period parameter");
        return false;
    }

    ConstString local_rpc = local;
    local_rpc += "/rpc:o";
    ConstString remote_rpc = remote;
    remote_rpc += "/rpc:i";

    if (!inputPort.open(local.c_str()))
    {
        yError("LaserRangefinder2DClient::open() error could not open port %s, check network\n",local.c_str());
        return false;
    }
    inputPort.useCallback();

    if (!rpcPort.open(local_rpc.c_str()))
    {
        yError("LaserRangefinder2DClient::open() error could not open rpc port %s, check network\n", local_rpc.c_str());
        return false;
    }

    bool ok=Network::connect(remote.c_str(), local.c_str(), "udp");
    if (!ok)
    {
        yError("LaserRangefinder2DClient::open() error could not connect to %s\n", remote.c_str());
        return false;
    }

    ok=Network::connect(local_rpc.c_str(), remote_rpc.c_str());
    if (!ok)
    {
        yError("LaserRangefinder2DClient::open() error could not connect to %s\n", remote_rpc.c_str());
       return false;
    }

    return true;
}

bool yarp::dev::LaserRangefinder2DClient::close()
{
    rpcPort.close();
    inputPort.close();
    return true;
}

int yarp::dev::LaserRangefinder2DClient::getRangeData(yarp::sig::Vector &out)
{
    inputPort.getData(out);
    return true;
}

bool yarp::dev::LaserRangefinder2DClient::getDeviceStatus(int &status)
{
    status = inputPort.getStatus();
    return true;
}

bool yarp::dev::LaserRangefinder2DClient::getDeviceInfo(yarp::os::ConstString &device_info)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_DEVICE_INFO);
    bool ok = rpcPort.write(cmd, response);
    if (CHECK_FAIL(ok, response)!=false)
    {
        device_info = response.get(2).asString();
        return true;
    }
    return false;
}

Stamp yarp::dev::LaserRangefinder2DClient::getLastInputStamp()
{
    return lastTs;
}

yarp::dev::DriverCreator *createLaserRangefinder2DClient() {
    return new DriverCreatorOf<LaserRangefinder2DClient>("laserRangefinder2DClient",
        "",
        "laserRangefinder2DClient");
}
