/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#define _USE_MATH_DEFINES

#include "Rangefinder2D_nws_yarp.h"

#include <yarp/os/LogStream.h>

#include <yarp/dev/ControlBoardInterfaces.h>

#include <cmath>
#include <sstream>

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;

YARP_LOG_COMPONENT(RANGEFINDER2D_NWS_YARP, "yarp.devices.Rangefinder2D_nws_yarp")


/**
  * It reads the data from a rangefinder sensor and sends them on one port.
  * It also creates one rpc port.
  */

    Rangefinder2D_nws_yarp::Rangefinder2D_nws_yarp() : PeriodicThread(DEFAULT_THREAD_PERIOD),
    sens_p(nullptr),
    minAngle(0),
    maxAngle(0),
    minDistance(0),
    maxDistance(0),
    resolution(0)
{}

Rangefinder2D_nws_yarp::~Rangefinder2D_nws_yarp()
{
    sens_p = nullptr;
}

/**
  * Specify which sensor this thread has to read from.
  */

bool Rangefinder2D_nws_yarp::attach(yarp::dev::PolyDriver* driver)
{
    if (driver->isValid())
    {
        driver->view(sens_p);
        m_RPC.setInterface(sens_p);
    }

    if (nullptr == sens_p)
    {
        yCError(RANGEFINDER2D_NWS_YARP, "subdevice passed to attach method is invalid");
        return false;
    }
    attach(sens_p);

    if(!sens_p->getDistanceRange(minDistance, maxDistance))
    {
        yCError(RANGEFINDER2D_NWS_YARP) << "Laser device does not provide min & max distance range.";
        return false;
    }

    if(!sens_p->getScanLimits(minAngle, maxAngle))
    {
        yCError(RANGEFINDER2D_NWS_YARP) << "Laser device does not provide min & max angle scan range.";
        return false;
    }

    if (!sens_p->getHorizontalResolution(resolution))
    {
        yCError(RANGEFINDER2D_NWS_YARP) << "Laser device does not provide horizontal resolution ";
        return false;
    }

    PeriodicThread::setPeriod(m_period);
    return PeriodicThread::start();
}

void Rangefinder2D_nws_yarp::attach(yarp::dev::IRangefinder2D *s)
{
    sens_p = s;
}

bool Rangefinder2D_nws_yarp::detach()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    sens_p = nullptr;
    return true;
}

bool Rangefinder2D_nws_yarp::read(yarp::os::ConnectionReader& connection)
{
    bool b = m_RPC.read(connection);
    if (b) {
        return true;
    } else {
        yCDebug(RANGEFINDER2D_NWS_YARP) << "read() Command failed";
        return false;
    }
}

bool Rangefinder2D_nws_yarp::threadInit()
{
    return true;
}

bool Rangefinder2D_nws_yarp::open(yarp::os::Searchable &config)
{
    if (!parseParams(config)) { return false; }

    if (!streamingPort.open(m_name))
    {
        yCError(RANGEFINDER2D_NWS_YARP, "failed to open port %s", m_name.c_str());
        return false;
    }
    std::string rpc_portname = m_name + "/rpc:i";
    if (!rpcPort.open(rpc_portname))
    {
        yCError(RANGEFINDER2D_NWS_YARP, "failed to open port %s", rpc_portname.c_str());
        return false;
    }
    rpcPort.setReader(*this);
    return true;
}

void Rangefinder2D_nws_yarp::threadRelease()
{
    streamingPort.interrupt();
    streamingPort.close();
    rpcPort.interrupt();
    rpcPort.close();
}

void Rangefinder2D_nws_yarp::run()
{
    if (sens_p!=nullptr)
    {
        bool ret = true;
        IRangefinder2D::Device_status status;
        yarp::sig::Vector ranges;
        double synchronized_timestamp = 0;
        ret &= sens_p->getRawData(ranges, &synchronized_timestamp);
        ret &= sens_p->getDeviceStatus(status);

        if (ret)
        {
            if (std::isnan(synchronized_timestamp) == false)
            {
                lastStateStamp.update(synchronized_timestamp);
            }
            else
            {
                lastStateStamp.update(yarp::os::Time::now());
            }

            int ranges_size = ranges.size();
            YARP_UNUSED(ranges_size);

            yarp::sig::LaserScan2D& b = streamingPort.prepare();
            //b.clear();
            b.scans=ranges;
            b.angle_min= minAngle;
            b.angle_max= maxAngle;
            b.range_min= minDistance;
            b.range_max= maxDistance;
            b.status=status;
            streamingPort.setEnvelope(lastStateStamp);
            streamingPort.write();

        }
        else
        {
            yCError(RANGEFINDER2D_NWS_YARP, "%s: Sensor returned error", m_name.c_str());
        }
    }
}

bool Rangefinder2D_nws_yarp::close()
{
    yCTrace(RANGEFINDER2D_NWS_YARP, "Rangefinder2D_nws_yarp::Close");
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }

    detach();
    return true;
}
