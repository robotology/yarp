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

inline void  Rangefinder2DInputPortProcessor::resetStat()
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

Rangefinder2DInputPortProcessor::Rangefinder2DInputPortProcessor()
{
    state = IRangefinder2D::DEVICE_GENERAL_ERROR;
    resetStat();
}

void Rangefinder2DInputPortProcessor::onRead(yarp::dev::LaserScan2D&b)
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

inline int Rangefinder2DInputPortProcessor::getLast(yarp::dev::LaserScan2D&data, Stamp &stmp)
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

yarp::dev::IRangefinder2D::Device_status Rangefinder2DInputPortProcessor::getStatus()
{
    mutex.lock();
    auto status = (yarp::dev::IRangefinder2D::Device_status) lastScan.status;
    mutex.unlock();
    return status;
}

inline int Rangefinder2DInputPortProcessor::getIterations()
{
    mutex.lock();
    int ret=count;
    mutex.unlock();
    return ret;
}

// time is in ms
void Rangefinder2DInputPortProcessor::getEstFrequency(int &ite, double &av, double &min, double &max)
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
    m_local_portname.clear();
    m_remote_portname.clear();

    m_local_portname = config.find("local").asString();
    m_remote_portname = config.find("remote").asString();

    m_carrier = config.check("carrier", yarp::os::Value("tcp"), "the carrier used for the connection with the server").asString();

    if (m_local_portname =="")
    {
        yCError(RANGEFINDER2DCLIENT, "open() error you have to provide valid local name");
        return false;
    }
    if (m_remote_portname =="")
    {
        yCError(RANGEFINDER2DCLIENT, "open() error you have to provide valid remote name");
        return false;
    }

    std::string local_rpc_portname = m_local_portname;
    local_rpc_portname += "/rpc:o";
    std::string remote_rpc_portname = m_remote_portname;
    remote_rpc_portname += "/rpc:i";

    if (!m_inputPort.open(m_local_portname))
    {
        yCError(RANGEFINDER2DCLIENT, "open() error could not open port %s, check network\n", m_local_portname.c_str());
        return false;
    }
    m_inputPort.useCallback();

    if (!m_rpcPort.open(local_rpc_portname))
    {
        yCError(RANGEFINDER2DCLIENT, "open() error could not open rpc port %s, check network\n", local_rpc_portname.c_str());
        return false;
    }

    bool ok=Network::connect(m_remote_portname.c_str(), m_local_portname.c_str(), m_carrier);
    if (!ok)
    {
        yCError(RANGEFINDER2DCLIENT, "open() error could not connect to %s\n", m_remote_portname.c_str());
        return false;
    }

    ok=Network::connect(local_rpc_portname, remote_rpc_portname);
    if (!ok)
    {
       yCError(RANGEFINDER2DCLIENT, "open() error could not connect to %s\n", remote_rpc_portname.c_str());
       return false;
    }

    //getScanLimits is used here to update the cached values of scan_angle_min, scan_angle_max
    double tmp_min;
    double tmp_max;
    this->getScanLimits(tmp_min, tmp_max);

    return true;
}

bool Rangefinder2D_nwc_yarp::close()
{
    m_inputPort.disableCallback();
    m_rpcPort.close();
    m_inputPort.close();
    return true;
}

bool Rangefinder2D_nwc_yarp::getRawData(yarp::sig::Vector &data, double* timestamp)
{
    yarp::dev::LaserScan2D scan;
    m_inputPort.getLast(scan, m_lastTs);

    data = scan.scans;

    if (timestamp != nullptr)
    {
        *timestamp = m_lastTs.getTime();
    }
    return true;
}

bool Rangefinder2D_nwc_yarp::getLaserMeasurement(std::vector<LaserMeasurementData> &data, double* timestamp)
{
    yarp::dev::LaserScan2D scan;
    m_inputPort.getLast(scan, m_lastTs);
    size_t size = scan.scans.size();
    data.resize(size);
    if (m_scan_angle_max < m_scan_angle_min) { yCError(RANGEFINDER2DCLIENT) << "getLaserMeasurement failed"; return false; }
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
    return true;
}

bool Rangefinder2D_nwc_yarp::getDistanceRange(double& min, double& max)
{
    Bottle cmd, response;
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_ILASER2D);
    cmd.addVocab32(VOCAB_LASER_DISTANCE_RANGE);
    bool ok = m_rpcPort.write(cmd, response);
    if (CHECK_FAIL(ok, response) != false)
    {
        min = response.get(2).asFloat64();
        max = response.get(3).asFloat64();
        return true;
    }
    return false;
}

bool Rangefinder2D_nwc_yarp::setDistanceRange(double min, double max)
{
    Bottle cmd, response;
    cmd.addVocab32(VOCAB_SET);
    cmd.addVocab32(VOCAB_ILASER2D);
    cmd.addVocab32(VOCAB_LASER_DISTANCE_RANGE);
    cmd.addFloat64(min);
    cmd.addFloat64(max);
    bool ok = m_rpcPort.write(cmd, response);
    if (ok)
    {
        m_scan_angle_min = min;
        m_scan_angle_max = max;
    }
    return (CHECK_FAIL(ok, response));
}

bool Rangefinder2D_nwc_yarp::getScanLimits(double& min, double& max)
{
    Bottle cmd, response;
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_ILASER2D);
    cmd.addVocab32(VOCAB_LASER_ANGULAR_RANGE);
    bool ok = m_rpcPort.write(cmd, response);
    if (CHECK_FAIL(ok, response) != false)
    {
        min = m_scan_angle_min = response.get(2).asFloat64();
        max = m_scan_angle_max = response.get(3).asFloat64();
        return true;
    }
    return false;
}

bool Rangefinder2D_nwc_yarp::setScanLimits(double min, double max)
{
    Bottle cmd, response;
    cmd.addVocab32(VOCAB_SET);
    cmd.addVocab32(VOCAB_ILASER2D);
    cmd.addVocab32(VOCAB_LASER_ANGULAR_RANGE);
    cmd.addFloat64(min);
    cmd.addFloat64(max);
    bool ok = m_rpcPort.write(cmd, response);
    return (CHECK_FAIL(ok, response));
}

bool Rangefinder2D_nwc_yarp::getHorizontalResolution(double& step)
{
    Bottle cmd, response;
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_ILASER2D);
    cmd.addVocab32(VOCAB_LASER_ANGULAR_STEP);
    bool ok = m_rpcPort.write(cmd, response);
    if (CHECK_FAIL(ok, response) != false)
    {
        step = response.get(2).asFloat64();
        return true;
    }
    return false;
}

bool Rangefinder2D_nwc_yarp::setHorizontalResolution(double step)
{
    Bottle cmd, response;
    cmd.addVocab32(VOCAB_SET);
    cmd.addVocab32(VOCAB_ILASER2D);
    cmd.addVocab32(VOCAB_LASER_ANGULAR_STEP);
    cmd.addFloat64(step);
    bool ok = m_rpcPort.write(cmd, response);
    return (CHECK_FAIL(ok, response));
}

bool Rangefinder2D_nwc_yarp::getScanRate(double& rate)
{
    Bottle cmd, response;
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_ILASER2D);
    cmd.addVocab32(VOCAB_LASER_SCAN_RATE);
    bool ok = m_rpcPort.write(cmd, response);
    if (CHECK_FAIL(ok, response) != false)
    {
        rate = response.get(2).asFloat64();
        return true;
    }
    return false;
}

bool Rangefinder2D_nwc_yarp::setScanRate(double rate)
{
    Bottle cmd, response;
    cmd.addVocab32(VOCAB_SET);
    cmd.addVocab32(VOCAB_ILASER2D);
    cmd.addVocab32(VOCAB_LASER_SCAN_RATE);
    cmd.addFloat64(rate);
    bool ok = m_rpcPort.write(cmd, response);
    return (CHECK_FAIL(ok, response));
}

bool Rangefinder2D_nwc_yarp::getDeviceStatus(Device_status &status)
{
    status = m_inputPort.getStatus();
    return true;
}

bool Rangefinder2D_nwc_yarp::getDeviceInfo(std::string &device_info)
{
    Bottle cmd, response;
    cmd.addVocab32(VOCAB_GET);
    cmd.addVocab32(VOCAB_ILASER2D);
    cmd.addVocab32(VOCAB_DEVICE_INFO);
    bool ok = m_rpcPort.write(cmd, response);
    if (CHECK_FAIL(ok, response)!=false)
    {
        device_info = response.get(2).asString();
        return true;
    }
    return false;
}
