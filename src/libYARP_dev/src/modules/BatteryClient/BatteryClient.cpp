// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
* Author: Marco Randazzo <marco.randazzo@iit.it>
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

#include <BatteryClient.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

/*! \file BatteryClient.cpp */

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;


inline void BatteryInputPortProcessor::resetStat()
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

BatteryInputPortProcessor::BatteryInputPortProcessor()
{
    state=IBattery::BATTERY_GENERAL_ERROR;
    resetStat();
}

void BatteryInputPortProcessor::onRead(yarp::os::Bottle &b)
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
        if (tmpDT*1000<BATTERY_TIMEOUT)
        {
            state = b.get(5).asInt();
        }
        else
        {
            state = IBattery::BATTERY_TIMEOUT;
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
    if ((1000*(newStamp.getTime()-lastStamp.getTime()))<BATTERY_TIMEOUT)
    {
        state = b.get(5).asInt();
    }
    else
    {
        state = IBattery::BATTERY_TIMEOUT;
    }
    lastStamp = newStamp;

    mutex.post();
}

inline int BatteryInputPortProcessor::getLast(yarp::os::Bottle &data, Stamp &stmp)
{
    mutex.wait();
    int ret=state;
    if (ret != IBattery::BATTERY_GENERAL_ERROR)
    {
        data=lastBottle;
        stmp = lastStamp;
    }
    mutex.post();

    return ret;
}

double BatteryInputPortProcessor::getVoltage()
{
    mutex.wait();
    double voltage = lastBottle.get(0).asInt();
    mutex.post();
    return voltage;
}

double BatteryInputPortProcessor::getCurrent()
{
    mutex.wait();
    double current = lastBottle.get(1).asInt();
    mutex.post();
    return current;
}

double BatteryInputPortProcessor::getCharge()
{
    mutex.wait();
    double charge = lastBottle.get(2).asInt();
    mutex.post();
    return charge;
}

int    BatteryInputPortProcessor::getStatus()
{
    mutex.wait();
    int status = lastBottle.get(3).asInt();
    mutex.post();
    return status;
}

double BatteryInputPortProcessor::getTemperature()
{
    mutex.wait();
    double temperature = lastBottle.get(4).asInt();
    mutex.post();
    return temperature;
}

inline int BatteryInputPortProcessor::getIterations()
{
    mutex.wait();
    int ret=count;
    mutex.post();
    return ret;
}

// time is in ms
void BatteryInputPortProcessor::getEstFrequency(int &ite, double &av, double &min, double &max)
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

bool yarp::dev::BatteryClient::open(yarp::os::Searchable &config)
{
    local.clear();
    remote.clear();

    local  = config.find("local").asString().c_str();
    remote = config.find("remote").asString().c_str();

    if (local=="")
    {
        yError("BatteryClient::open() error you have to provide valid local name");
        return false;
    }
    if (remote=="")
    {
        yError("BatteryClient::open() error you have to provide valid remote name");
        return false;
    }

    if (config.check("period"))
    {
        _rate = config.find("period").asInt();
    }
    else
    {
        yError("BatteryClient::open() missing period parameter");
        return false;
    }

    ConstString local_rpc = local;
    local_rpc += "/rpc:o";
    ConstString remote_rpc = remote;
    remote_rpc += "/rpc:i";

    if (!inputPort.open(local.c_str()))
    {
        yError("BatteryClient::open() error could not open port %s, check network\n",local.c_str());
        return false;
    }
    inputPort.useCallback();

    if (!rpcPort.open(local_rpc.c_str()))
    {
        yError("BatteryClient::open() error could not open rpc port %s, check network\n", local_rpc.c_str());
        return false;
    }

    bool ok=Network::connect(remote.c_str(), local.c_str(), "udp");
    if (!ok)
    {
        yError("BatteryClient::open() error could not connect to %s\n", remote.c_str());
        return false;
    }

    ok=Network::connect(local_rpc.c_str(), remote_rpc.c_str());
    if (!ok)
    {
        fprintf(stderr,"BatteryClient::open() error could not connect to %s\n", remote_rpc.c_str());
       return false;
    }

    return true;
}

bool yarp::dev::BatteryClient::close()
{
    rpcPort.close();
    inputPort.close();
    return true;
}

bool yarp::dev::BatteryClient::getBatteryVoltage(double &voltage)
{
    voltage = inputPort.getVoltage();
    return true;
}

bool yarp::dev::BatteryClient::getBatteryCurrent(double &current)
{
    current = inputPort.getCurrent();
    return true;
}

bool yarp::dev::BatteryClient::getBatteryCharge(double &charge)
{
    charge = inputPort.getCharge();
    return true;
}

bool yarp::dev::BatteryClient::getBatteryStatus(int &status)
{
    status = inputPort.getStatus();
    return true;
}

bool yarp::dev::BatteryClient::getBatteryTemperature(double &temperature)
{
    temperature = inputPort.getStatus();
    return true;
}

bool yarp::dev::BatteryClient::getBatteryInfo(yarp::os::ConstString &battery_info)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_IBATTERY);
    cmd.addVocab(VOCAB_BATTERY_INFO);
    bool ok = rpcPort.write(cmd, response);
    if (CHECK_FAIL(ok, response)!=false)
    {
        battery_info = response.get(2).asString();
        return true;
    }
    return false;
}

Stamp yarp::dev::BatteryClient::getLastInputStamp()
{
    return lastTs;
}

yarp::dev::DriverCreator *createBatteryClient() {
    return new DriverCreatorOf<BatteryClient>("batteryClient",
        "",
        "batteryClient");
}
