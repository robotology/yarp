/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#define _USE_MATH_DEFINES

#include "Rangefinder2D_nwc_yarp.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/math/Math.h>

#include <limits>
#include <cmath>

/*! \file Rangefinder2D_nwc_yarp.cpp */

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;

#ifndef DEG2RAD
#define DEG2RAD M_PI/180.0
#endif

#define DEFAULT_THREAD_PERIOD 20 //ms
const int LASER_TIMEOUT=100; //ms

namespace {
YARP_LOG_COMPONENT(RANGEFINDER2DCLIENT, "yarp.device.Rangefinder2D_nwc_yarp")
}

inline void  Rangefinder2D_InputPortProcessor::resetStat()
{
    mutex.lock();
    count=0;
    deltaT=0;
    deltaTMax=0;
    deltaTMin=1e22;
    now=SystemClock::nowSystem();
    prev=now;
    mutex.unlock();
}

Rangefinder2D_InputPortProcessor::Rangefinder2D_InputPortProcessor()
{
    state = IRangefinder2D::DEVICE_GENERAL_ERROR;
    resetStat();
}

void Rangefinder2D_InputPortProcessor::onRead(yarp::sig::LaserScan2D& b)
{
    now=SystemClock::nowSystem();
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
        if (tmpDT*1000<LASER_TIMEOUT)
        {
            state = b.status;
        }
        else
        {
            state = IRangefinder2D::DEVICE_TIMEOUT;
        }
    }

    prev=now;
    count++;

    lastScan=b;
    Stamp newStamp;
    getEnvelope(newStamp);

    //initializations (first received data)
    if (lastStamp.isValid()==false)
    {
        lastStamp = newStamp;
    }

    //now compare timestamps
    if ((1000*(newStamp.getTime()-lastStamp.getTime()))<LASER_TIMEOUT)
    {
        state = b.status;
    }
    else
    {
        state = IRangefinder2D::DEVICE_TIMEOUT;
    }
    lastStamp = newStamp;

    mutex.unlock();
}

inline int Rangefinder2D_InputPortProcessor::getLast(yarp::sig::LaserScan2D& data, Stamp& stmp)
{
    mutex.lock();
    int ret=state;
    if (ret != IRangefinder2D::DEVICE_GENERAL_ERROR)
    {
        data=lastScan;
        stmp = lastStamp;
    }
    mutex.unlock();

    return ret;
}

yarp::dev::IRangefinder2D::Device_status Rangefinder2D_InputPortProcessor::getStatus()
{
    mutex.lock();
    auto status = (yarp::dev::IRangefinder2D::Device_status) lastScan.status;
    mutex.unlock();
    return status;
}

inline int Rangefinder2D_InputPortProcessor::getIterations()
{
    mutex.lock();
    int ret=count;
    mutex.unlock();
    return ret;
}

// time is in ms
void Rangefinder2D_InputPortProcessor::getEstFrequency(int &ite, double &av, double &min, double &max)
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

bool Rangefinder2D_nwc_yarp::open(yarp::os::Searchable &config)
{
    if (!parseParams(config)) { return false; }

    std::string local_rpc_portname = m_local+ "/rpc:o";
    std::string remote_rpc_portname = m_remote + "/rpc:i";

    if (!m_inputPort.open(m_local))
    {
        yCError(RANGEFINDER2DCLIENT, "open() error could not open port %s, check network\n", m_local.c_str());
        return false;
    }
    m_inputPort.useCallback();

    if (!m_rpcPort.open(local_rpc_portname))
    {
        yCError(RANGEFINDER2DCLIENT, "open() error could not open rpc port %s, check network\n", local_rpc_portname.c_str());
        return false;
    }

    bool ok=Network::connect(m_remote.c_str(), m_local.c_str(), m_carrier);
    if (!ok)
    {
        yCError(RANGEFINDER2DCLIENT, "open() error could not connect to %s\n", m_remote.c_str());
        return false;
    }

    ok=Network::connect(local_rpc_portname, remote_rpc_portname);
    if (!ok)
    {
       yCError(RANGEFINDER2DCLIENT, "open() error could not connect to %s\n", remote_rpc_portname.c_str());
       return false;
    }

    if (!m_RPC.yarp().attachAsClient(m_rpcPort))
    {
       yCError(RANGEFINDER2DCLIENT, "Error! Cannot attach the port as a client");
       return false;
    }

    //getScanLimits is used here to update the cached values of scan_angle_min, scan_angle_max
    if(!this->getScanLimits(m_scan_angle_min, m_scan_angle_max))
    {
       yCError(RANGEFINDER2DCLIENT) << "getScanLimits failed";
       return false;
    }

    return true;
}

bool Rangefinder2D_nwc_yarp::close()
{
    m_inputPort.disableCallback();
    m_rpcPort.close();
    m_inputPort.close();
    return true;
}

ReturnValue Rangefinder2D_nwc_yarp::getRawData(yarp::sig::Vector &data, double* timestamp)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    yarp::sig::LaserScan2D scan;
    m_inputPort.getLast(scan, m_lastTs);

    data = scan.scans;

    if (timestamp != nullptr)
    {
        *timestamp = m_lastTs.getTime();
    }
    return ReturnValue_ok;
}

ReturnValue Rangefinder2D_nwc_yarp::getLaserMeasurement(std::vector<LaserMeasurementData> &data, double* timestamp)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    yarp::sig::LaserScan2D scan;
    m_inputPort.getLast(scan, m_lastTs);

    size_t size = scan.scans.size();
    data.resize(size);
    if (m_scan_angle_max < m_scan_angle_min)
    {
        yCError(RANGEFINDER2DCLIENT) << "getLaserMeasurement failed";
        return ReturnValue::return_code::return_value_error_method_failed;
    }
    double laser_angle_of_view = m_scan_angle_max - m_scan_angle_min;
    for (size_t i = 0; i < size; i++)
    {
        double angle = (i / double(size)*laser_angle_of_view + m_scan_angle_min)* DEG2RAD;
        double value = scan.scans[i];
        data[i].set_polar(value,angle);
    }
    if (timestamp!=nullptr)
    {
        *timestamp = m_lastTs.getTime();
    }
    return ReturnValue_ok;
}

ReturnValue Rangefinder2D_nwc_yarp::getDistanceRange(double& min, double& max)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_RPC.getDistanceRange_RPC();
    if (!ret.retval) {
        yCError(RANGEFINDER2DCLIENT, "Unable to getDistanceRange");
        return ret.retval;
    }
    min = ret.min;
    max = ret.max;
    return ret.retval;
}

ReturnValue Rangefinder2D_nwc_yarp::setDistanceRange(double min, double max)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_RPC.setDistanceRange_RPC(min, max);
    if (!ret)
    {
        yCError(RANGEFINDER2DCLIENT, "Unable to setDistanceRange");
        return ret;
    }
    return ret;
}

ReturnValue Rangefinder2D_nwc_yarp::getScanLimits(double& min, double& max)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_RPC.getScanLimits_RPC();
    if (!ret.retval) {
        yCError(RANGEFINDER2DCLIENT, "Unable to getScanLimits");
        return ret.retval;
    }
    min = ret.min;
    max = ret.max;
    return ret.retval;
}

ReturnValue Rangefinder2D_nwc_yarp::setScanLimits(double min, double max)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_RPC.setScanLimits_RPC(min,max);
    if (!ret) {
        yCError(RANGEFINDER2DCLIENT, "Unable to setScanLimits");
        return ret;
    }
    return ret;
}

ReturnValue Rangefinder2D_nwc_yarp::getHorizontalResolution(double& step)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_RPC.getHorizontalResolution_RPC();
    if (!ret.retval) {
        yCError(RANGEFINDER2DCLIENT, "Unable to getHorizontalResolution");
        return ret.retval;
    }
    step = ret.step;
    return ret.retval;
}

ReturnValue Rangefinder2D_nwc_yarp::setHorizontalResolution(double step)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_RPC.setScanRate_RPC(step);
    if (!ret) {
        yCError(RANGEFINDER2DCLIENT, "Unable to setHorizontalResolution");
        return ret;
    }
    return ret;
}

ReturnValue Rangefinder2D_nwc_yarp::getScanRate(double& rate)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_RPC.getScanRate_RPC();
    if (!ret.retval) {
        yCError(RANGEFINDER2DCLIENT, "Unable to getScanRate");
        return ret.retval;
    }
    rate = ret.rate;
    return ret.retval;
}

ReturnValue Rangefinder2D_nwc_yarp::setScanRate(double rate)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_RPC.setScanRate_RPC(rate);
    if (!ret) {
        yCError(RANGEFINDER2DCLIENT, "Unable to setScanRate");
        return ret;
    }
    return ret;
}

ReturnValue Rangefinder2D_nwc_yarp::getDeviceStatus(Device_status &status)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    status = m_inputPort.getStatus();
    return ReturnValue_ok;
}

ReturnValue Rangefinder2D_nwc_yarp::getDeviceInfo(std::string &device_info)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_RPC.getDeviceInfo_RPC();
    if (!ret.retval)
    {
        yCError(RANGEFINDER2DCLIENT, "Unable to getDeviceInfo");
        return ret.retval;
    }
    device_info = ret.device_info;
    return ret.retval;
}
