/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "BatteryClient.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

/*! \file BatteryClient.cpp */

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;


inline void BatteryInputPortProcessor::resetStat()
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

BatteryInputPortProcessor::BatteryInputPortProcessor()
{
    state=IBattery::BATTERY_GENERAL_ERROR;
    resetStat();
}

void BatteryInputPortProcessor::onRead(yarp::os::Bottle &b)
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
        if (tmpDT*1000<BATTERY_TIMEOUT)
        {
            state = b.get(5).asInt32();
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
        state = b.get(5).asInt32();
    }
    else
    {
        state = IBattery::BATTERY_TIMEOUT;
    }
    lastStamp = newStamp;

    mutex.unlock();
}

inline int BatteryInputPortProcessor::getLast(yarp::os::Bottle &data, Stamp &stmp)
{
    mutex.lock();
    int ret=state;
    if (ret != IBattery::BATTERY_GENERAL_ERROR)
    {
        data=lastBottle;
        stmp = lastStamp;
    }
    mutex.unlock();

    return ret;
}

double BatteryInputPortProcessor::getVoltage()
{
    mutex.lock();
    double voltage = lastBottle.get(0).asInt32();
    mutex.unlock();
    return voltage;
}

double BatteryInputPortProcessor::getCurrent()
{
    mutex.lock();
    double current = lastBottle.get(1).asInt32();
    mutex.unlock();
    return current;
}

double BatteryInputPortProcessor::getCharge()
{
    mutex.lock();
    double charge = lastBottle.get(2).asInt32();
    mutex.unlock();
    return charge;
}

int    BatteryInputPortProcessor::getStatus()
{
    mutex.lock();
    int status = lastBottle.get(3).asInt32();
    mutex.unlock();
    return status;
}

double BatteryInputPortProcessor::getTemperature()
{
    mutex.lock();
    double temperature = lastBottle.get(4).asInt32();
    mutex.unlock();
    return temperature;
}

inline int BatteryInputPortProcessor::getIterations()
{
    mutex.lock();
    int ret=count;
    mutex.unlock();
    return ret;
}

// time is in ms
void BatteryInputPortProcessor::getEstFrequency(int &ite, double &av, double &min, double &max)
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
        _rate = config.find("period").asInt32();
    }
    else
    {
        yError("BatteryClient::open() missing period parameter");
        return false;
    }

    std::string local_rpc = local;
    local_rpc += "/rpc:o";
    std::string remote_rpc = remote;
    remote_rpc += "/rpc:i";

    if (!inputPort.open(local.c_str()))
    {
        yError("BatteryClient::open() error could not open port %s, check network",local.c_str());
        return false;
    }
    inputPort.useCallback();

    if (!rpcPort.open(local_rpc.c_str()))
    {
        yError("BatteryClient::open() error could not open rpc port %s, check network", local_rpc.c_str());
        return false;
    }

    bool ok=Network::connect(remote.c_str(), local.c_str(), "udp");
    if (!ok)
    {
        yError("BatteryClient::open() error could not connect to %s", remote.c_str());
        return false;
    }

    ok=Network::connect(local_rpc.c_str(), remote_rpc.c_str());
    if (!ok)
    {
       yError("BatteryClient::open() error could not connect to %s", remote_rpc.c_str());
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
    temperature = inputPort.getTemperature();
    return true;
}

bool yarp::dev::BatteryClient::getBatteryInfo(std::string &battery_info)
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
