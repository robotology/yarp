/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#define _USE_MATH_DEFINES

#include "Odometry2D_nwc_yarp.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/math/Math.h>

#include <limits>
#include <cmath>

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(ODOMETRY2DCLIENT, "yarp.device.Odometry2D_nwc_yarp")
}

inline void  OdometryData2DInputPortProcessor::resetStat()
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

OdometryData2DInputPortProcessor::OdometryData2DInputPortProcessor()
{
    resetStat();
}

void OdometryData2DInputPortProcessor::onRead(yarp::dev::OdometryData& b)
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
        if (tmpDT*1000<DATA_TIMEOUT_ms)
        {
            timeout_occurred = false;
        }
        else
        {
            timeout_occurred = true;
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
    if ((1000 * (newStamp.getTime() - lastStamp.getTime())) < DATA_TIMEOUT_ms)
    {
        timeout_occurred = false;
    }
    else
    {
        timeout_occurred = true;
    }
    lastStamp = newStamp;

    mutex.unlock();
}

inline bool OdometryData2DInputPortProcessor::getLast(yarp::dev::OdometryData& data, Stamp& stmp)
{
    mutex.lock();
    data=lastScan;
    stmp = lastStamp;
    mutex.unlock();

    return true;
}

inline int OdometryData2DInputPortProcessor::getIterations()
{
    mutex.lock();
    int ret=count;
    mutex.unlock();
    return ret;
}

// time is in ms
void OdometryData2DInputPortProcessor::getEstFrequency(int &ite, double &av, double &min, double &max)
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

bool Odometry2D_nwc_yarp::open(yarp::os::Searchable &config)
{
    if (!parseParams(config)) { return false; }

    std::string local_rpc_portname = m_local+ "/rpc";
    std::string remote_rpc_portname = m_remote + "/rpc";
    std::string local_odometry_input_portname = m_local + "/odometry:i";
    std::string remote_odometry_input_portname = m_remote + "/odometry:o";

    //open receiving streaming port
    if (!m_inputPort.open(local_odometry_input_portname))
    {
        yCError(ODOMETRY2DCLIENT, "open() error could not open port %s, check network\n", local_odometry_input_portname.c_str());
        return false;
    }
    m_inputPort.useCallback();

    //open rpc port
    if (!m_rpcPort.open(local_rpc_portname))
    {
        yCError(ODOMETRY2DCLIENT, "open() error could not open rpc port %s, check network\n", local_rpc_portname.c_str());
        return false;
    }

    //connection for the streaming port
    bool ok=Network::connect(remote_odometry_input_portname, local_odometry_input_portname, m_carrier);
    if (!ok)
    {
        yCError(ODOMETRY2DCLIENT, "open() error could not connect to %s\n", m_remote.c_str());
        return false;
    }

    //connection for the rpc port
    ok=Network::connect(local_rpc_portname, remote_rpc_portname);
    if (!ok)
    {
       yCError(ODOMETRY2DCLIENT, "open() error could not connect to %s\n", remote_rpc_portname.c_str());
       return false;
    }

    if (!m_RPC.yarp().attachAsClient(m_rpcPort))
    {
       yCError(ODOMETRY2DCLIENT, "Error! Cannot attach the port as a client");
       return false;
    }

    // Check the protocol version
    if (!m_RPC.checkProtocolVersion()) {
        return false;
    }

    return true;
}

bool Odometry2D_nwc_yarp::close()
{
    m_inputPort.disableCallback();
    m_rpcPort.close();
    m_inputPort.close();
    return true;
}

ReturnValue Odometry2D_nwc_yarp::getOdometry(yarp::dev::OdometryData& odom, double* timestamp)
{
    std::lock_guard<std::mutex> lg(m_mutex);
    yarp::dev::OdometryData measure;
    bool recv = m_inputPort.getLast(measure, m_lastTs);

    odom = measure;

    if (timestamp != nullptr)
    {
        *timestamp = m_lastTs.getTime();
    }
    return ReturnValue_ok;
}

ReturnValue Odometry2D_nwc_yarp::resetOdometry()
{
    std::lock_guard<std::mutex> lg(m_mutex);
    auto ret = m_RPC.reset_odometry_RPC();
    if (!ret) {
        yCError(ODOMETRY2DCLIENT, "Unable to resetOdometry");
        return ret;
    }
    return ReturnValue_ok;
}
