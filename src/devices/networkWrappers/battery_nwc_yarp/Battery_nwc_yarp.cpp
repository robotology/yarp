/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "Battery_nwc_yarp.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

/*! \file Battery_nwc_yarp.cpp */

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(BATTERYCLIENT, "yarp.device.Battery_nwc_yarp")
} // namespace

inline void Battery_InputPortProcessor::resetStat()
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

Battery_InputPortProcessor::Battery_InputPortProcessor()
{
    state=IBattery::BATTERY_GENERAL_ERROR;
    resetStat();
}

void Battery_InputPortProcessor::onRead(yarp::dev::BatteryData &b)
{
    now=Time::now();
    mutex.lock();

    if (count>0)
    {
        double tmpDT=now-prev;
        deltaT+=tmpDT;
        if (tmpDT > deltaTMax) {
            deltaTMax = tmpDT;
        }
        if (tmpDT < deltaTMin) {
            deltaTMin = tmpDT;
        }

        //compare network time
        if (tmpDT*1000<BATTERY_TIMEOUT)
        {
            state = b.status;
        }
        else
        {
            state = IBattery::BATTERY_TIMEOUT;
        }
    }

    prev=now;
    count++;

    lastData=b;
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
        state = b.status;
    }
    else
    {
        state = IBattery::BATTERY_TIMEOUT;
    }
    lastStamp = newStamp;

    mutex.unlock();
}

inline int Battery_InputPortProcessor::getLast(yarp::dev::BatteryData &data, Stamp &stmp)
{
    mutex.lock();
    int ret=state;
    if (ret != IBattery::BATTERY_GENERAL_ERROR)
    {
        data=lastData;
        stmp = lastStamp;
    }
    mutex.unlock();

    return ret;
}

double Battery_InputPortProcessor::getVoltage()
{
    mutex.lock();
    double voltage = lastData.voltage;
    mutex.unlock();
    return voltage;
}

double Battery_InputPortProcessor::getCurrent()
{
    mutex.lock();
    double current = lastData.current;
    mutex.unlock();
    return current;
}

double Battery_InputPortProcessor::getCharge()
{
    mutex.lock();
    double charge = lastData.charge;
    mutex.unlock();
    return charge;
}

yarp::dev::IBattery::Battery_status Battery_InputPortProcessor::getStatus()
{
    mutex.lock();
    yarp::dev::IBattery::Battery_status status = (yarp::dev::IBattery::Battery_status)lastData.status;
    mutex.unlock();
    return status;
}

double Battery_InputPortProcessor::getTemperature()
{
    mutex.lock();
    double temperature = lastData.temperature;
    mutex.unlock();
    return temperature;
}

inline int Battery_InputPortProcessor::getIterations()
{
    mutex.lock();
    int ret=count;
    mutex.unlock();
    return ret;
}

// time is in ms
void Battery_InputPortProcessor::getEstFrequency(int &ite, double &av, double &min, double &max)
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

bool Battery_nwc_yarp::open(yarp::os::Searchable &config)
{
    if (!parseParams(config)) { return false; }

    std::string local_stream = m_local;
    local_stream += "/data:i";
    std::string local_rpc = m_local;
    local_rpc += "/rpc:o";
    std::string remote_stream = m_remote;
    remote_stream += "/data:o";
    std::string remote_rpc = m_remote;
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

    bool ok=Network::connect(remote_stream.c_str(), local_stream.c_str(), m_carrier);
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
    if (!m_battery_RPC.yarp().attachAsClient(rpcPort))
    {
        yCError(BATTERYCLIENT, "Error! Cannot attach the rpcPort as a client");
        return false;
    }

    return true;
}

bool Battery_nwc_yarp::close()
{
    rpcPort.close();
    inputPort.close();
    return true;
}

ReturnValue Battery_nwc_yarp::getBatteryVoltage(double &voltage)
{
    voltage = inputPort.getVoltage();
    return ReturnValue_ok;
}

ReturnValue Battery_nwc_yarp::getBatteryCurrent(double &current)
{
    current = inputPort.getCurrent();
    return ReturnValue_ok;
}

ReturnValue Battery_nwc_yarp::getBatteryCharge(double &charge)
{
    charge = inputPort.getCharge();
    return ReturnValue_ok;
}

ReturnValue Battery_nwc_yarp::getBatteryStatus(Battery_status &status)
{
    status = (Battery_status)inputPort.getStatus();
    return ReturnValue_ok;
}

ReturnValue Battery_nwc_yarp::getBatteryTemperature(double &temperature)
{
    temperature = inputPort.getTemperature();
    return ReturnValue_ok;
}

ReturnValue Battery_nwc_yarp::getBatteryInfo(std::string &battery_info)
{
    auto ret = m_battery_RPC.getBatteryInfoRPC();
    if (!ret.result)
    {
        yCError(BATTERYCLIENT, "Unable to: getBatteryInfo()");
        return ret.result;
    }
    battery_info = ret.info;
    return ret.result;
}

Stamp Battery_nwc_yarp::getLastInputStamp()
{
    return lastTs;
}
