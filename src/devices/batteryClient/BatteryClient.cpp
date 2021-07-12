/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "BatteryClient.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

/*! \file BatteryClient.cpp */

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(BATTERYCLIENT, "yarp.device.batteryClient")
} // namespace

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

    //initialization (first received data)
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
    double voltage = lastBottle.get(0).asFloat64();
    mutex.unlock();
    return voltage;
}

double BatteryInputPortProcessor::getCurrent()
{
    mutex.lock();
    double current = lastBottle.get(1).asFloat64();
    mutex.unlock();
    return current;
}

double BatteryInputPortProcessor::getCharge()
{
    mutex.lock();
    double charge = lastBottle.get(2).asFloat64();
    mutex.unlock();
    return charge;
}

int    BatteryInputPortProcessor::getStatus()
{
    mutex.lock();
    int status = lastBottle.get(4).asInt32();
    mutex.unlock();
    return status;
}

double BatteryInputPortProcessor::getTemperature()
{
    mutex.lock();
    double temperature = lastBottle.get(3).asFloat64();
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

bool BatteryClient::open(yarp::os::Searchable &config)
{
    local.clear();
    remote.clear();
    yCDebug(BATTERYCLIENT) << config.toString();
    local  = config.find("local").asString();
    remote = config.find("remote").asString();

    if (local=="")
    {
        yCError(BATTERYCLIENT, "open(): Invalid local name. --local parameter missing.");
        return false;
    }
    if (remote=="")
    {
        yCError(BATTERYCLIENT, "open(): Invalid remote name. --remote parameter missing.");
        return false;
    }

    std::string local_stream = local;
    local_stream += "/data:i";
    std::string local_rpc = local;
    local_rpc += "/rpc:o";
    std::string remote_stream = remote;
    remote_stream += "/data:o";
    std::string remote_rpc = remote;
    remote_rpc += "/rpc:i";

    if (!inputPort.open(local_stream))
    {
        yCError(BATTERYCLIENT, "open(): Could not open port %s, check network", local_stream.c_str());
        return false;
    }
    inputPort.useCallback();

    if (!rpcPort.open(local_rpc))
    {
        yCError(BATTERYCLIENT, "open(): Could not open rpc port %s, check network", local_rpc.c_str());
        return false;
    }

    bool ok=Network::connect(remote_stream.c_str(), local_stream.c_str(), "udp");
    if (!ok)
    {
        yCError(BATTERYCLIENT, "open(): Could not connect %s -> %s", remote_stream.c_str(), local_stream.c_str());
        return false;
    }

    ok=Network::connect(local_rpc, remote_rpc);
    if (!ok)
    {
       yCError(BATTERYCLIENT, "open() Could not connect %s -> %s", remote_rpc.c_str(), local_rpc.c_str());
       return false;
    }

    return true;
}

bool BatteryClient::close()
{
    rpcPort.close();
    inputPort.close();
    return true;
}

bool BatteryClient::getBatteryVoltage(double &voltage)
{
    voltage = inputPort.getVoltage();
    return true;
}

bool BatteryClient::getBatteryCurrent(double &current)
{
    current = inputPort.getCurrent();
    return true;
}

bool BatteryClient::getBatteryCharge(double &charge)
{
    charge = inputPort.getCharge();
    return true;
}

bool BatteryClient::getBatteryStatus(Battery_status &status)
{
    status = (Battery_status)inputPort.getStatus();
    return true;
}

bool BatteryClient::getBatteryTemperature(double &temperature)
{
    temperature = inputPort.getTemperature();
    return true;
}

bool BatteryClient::getBatteryInfo(std::string &battery_info)
{
    Bottle cmd, response;
    cmd.addVocab32(VOCAB_IBATTERY);
    cmd.addVocab32(VOCAB_BATTERY_INFO);
    bool ok = rpcPort.write(cmd, response);
    if (CHECK_FAIL(ok, response)!=false)
    {
        battery_info = response.get(2).asString();
        return true;
    }
    return false;
}

Stamp BatteryClient::getLastInputStamp()
{
    return lastTs;
}
