/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
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

#define _USE_MATH_DEFINES

#include "rangefinder2D_nws_yarp.h"
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/os/LogStream.h>

#include <cmath>
#include <sstream>

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;

YARP_LOG_COMPONENT(RANGEFINDER2D_NWS_YARP, "yarp.devices.Rangefinder2D_nws_yarp")


/**
  * It reads the data from a rangefinder sensor and sends them on one port.
  * It also creates one rpc port.
  */

    Rangefinder2D_nws_yarp::Rangefinder2D_nws_yarp() : PeriodicThread(DEFAULT_THREAD_PERIOD),
    sens_p(nullptr),
    iTimed(nullptr),
    _period(DEFAULT_THREAD_PERIOD),
    minAngle(0),
    maxAngle(0),
    minDistance(0),
    maxDistance(0),
    resolution(0),
    isDeviceOwned(false)
{}

Rangefinder2D_nws_yarp::~Rangefinder2D_nws_yarp()
{
    sens_p = nullptr;
}

/**
  * Specify which sensor this thread has to read from.
  */

bool Rangefinder2D_nws_yarp::attachAll(const PolyDriverList &device2attach)
{
    if (device2attach.size() != 1)
    {
        yCError(RANGEFINDER2D_NWS_YARP, "cannot attach more than one device");
        return false;
    }

    yarp::dev::PolyDriver * Idevice2attach = device2attach[0]->poly;

    if (Idevice2attach->isValid())
    {
        Idevice2attach->view(sens_p);
        Idevice2attach->view(iTimed);
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

    PeriodicThread::setPeriod(_period);
    return PeriodicThread::start();
}

bool Rangefinder2D_nws_yarp::detachAll()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    sens_p = nullptr;
    return true;
}

void Rangefinder2D_nws_yarp::attach(yarp::dev::IRangefinder2D *s)
{
    sens_p = s;
}

void Rangefinder2D_nws_yarp::detach()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    sens_p = nullptr;
}

bool Rangefinder2D_nws_yarp::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool ok = in.read(connection);
    if (!ok) return false;

    // parse in, prepare out
    int action = in.get(0).asVocab();
    int inter  = in.get(1).asVocab();
    bool ret = false;

    if (inter == VOCAB_ILASER2D)
    {
        if (action == VOCAB_GET)
        {
            int cmd = in.get(2).asVocab();
            if (cmd == VOCAB_DEVICE_INFO)
            {
                if (sens_p)
                {
                    std::string info;
                    if (sens_p->getDeviceInfo(info))
                    {
                        out.addVocab(VOCAB_IS);
                        out.addVocab(cmd);
                        out.addString(info);
                        ret = true;
                    }
                    else
                    {
                        ret = false;
                    }
                }
            }
            else if (cmd == VOCAB_LASER_DISTANCE_RANGE)
            {
                if (sens_p)
                {
                    double max = 0;
                    double min = 0;
                    if (sens_p->getDistanceRange(min, max))
                    {
                        out.addVocab(VOCAB_IS);
                        out.addVocab(cmd);
                        out.addFloat64(min);
                        out.addFloat64(max);
                        ret = true;
                    }
                    else
                    {
                        ret = false;
                    }
                }
            }
            else if (cmd == VOCAB_LASER_ANGULAR_RANGE)
            {
                if (sens_p)
                {
                    double max = 0;
                    double min = 0;
                    if (sens_p->getScanLimits(min, max))
                    {
                        out.addVocab(VOCAB_IS);
                        out.addVocab(cmd);
                        out.addFloat64(min);
                        out.addFloat64(max);
                        ret = true;
                    }
                    else
                    {
                        ret = false;
                    }
                }
            }
            else if (cmd == VOCAB_LASER_ANGULAR_STEP)
            {
                if (sens_p)
                {
                    double step = 0;
                    if (sens_p->getHorizontalResolution(step))
                    {
                        out.addVocab(VOCAB_IS);
                        out.addVocab(cmd);
                        out.addFloat64(step);
                        ret = true;
                    }
                    else
                    {
                        ret = false;
                    }
                }
            }
            else if (cmd == VOCAB_LASER_SCAN_RATE)
            {
                if (sens_p)
                {
                    double rate = 0;
                    if (sens_p->getScanRate(rate))
                    {
                        out.addVocab(VOCAB_IS);
                        out.addVocab(cmd);
                        out.addFloat64(rate);
                        ret = true;
                    }
                    else
                    {
                        ret = false;
                    }
                }
            }
            else
            {
                yCError(RANGEFINDER2D_NWS_YARP, "Invalid command received in Rangefinder2D_nws_yarp");
            }
        }
        else if (action == VOCAB_SET)
        {
            int cmd = in.get(2).asVocab();
            if (cmd == VOCAB_LASER_DISTANCE_RANGE)
            {
                if (sens_p)
                {
                    double min = in.get(3).asInt32();
                    double max = in.get(4).asInt32();
                    sens_p->setDistanceRange(min, max);
                    ret = true;
                }
            }
            else if (cmd == VOCAB_LASER_ANGULAR_RANGE)
            {
                if (sens_p)
                {
                    double min = in.get(3).asInt32();
                    double max = in.get(4).asInt32();
                    sens_p->setScanLimits(min, max);
                    ret = true;
                }
            }
            else if (cmd == VOCAB_LASER_SCAN_RATE)
            {
                if (sens_p)
                {
                    double rate = in.get(3).asInt32();
                    sens_p->setScanRate(rate);
                    ret = true;
                }
            }
            else if (cmd == VOCAB_LASER_ANGULAR_STEP)
            {
                if (sens_p)
                {
                    double step = in.get(3).asFloat64();
                    sens_p->setHorizontalResolution(step);
                    ret = true;
                }
            }
            else
            {
                yCError(RANGEFINDER2D_NWS_YARP, "Invalid command received in Rangefinder2D_nws_yarp");
            }
        }
        else
        {
            yCError(RANGEFINDER2D_NWS_YARP, "Invalid action received in Rangefinder2D_nws_yarp");
        }
    }
    else
    {
        yCError(RANGEFINDER2D_NWS_YARP, "Invalid interface vocab received in Rangefinder2D_nws_yarp");
    }

    if (!ret)
    {
        out.clear();
        out.addVocab(VOCAB_FAILED);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != nullptr) {
        out.write(*returnToSender);
    }
    return true;
}

bool Rangefinder2D_nws_yarp::threadInit()
{
    return true;
}

bool Rangefinder2D_nws_yarp::open(yarp::os::Searchable &config)
{
    Property params;
    params.fromString(config.toString());

    if (!config.check("period"))
    {
        yCError(RANGEFINDER2D_NWS_YARP) << "missing 'period' parameter. Check you configuration file\n";
        return false;
    }
    else
        _period = config.find("period").asInt32() / 1000.0;

    if (!config.check("name"))
    {
        yCError(RANGEFINDER2D_NWS_YARP) << "Rangefinder2D_nws_yarp: missing 'name' parameter. Check you configuration file; it must be like:";
        yCError(RANGEFINDER2D_NWS_YARP) << "   name:         full name of the port, like /robotName/deviceId/sensorType:o";
        return false;
    }
    else
    {
        streamingPortName  = config.find("name").asString();
        rpcPortName = streamingPortName + "/rpc:i";
    }

    if (config.check("frame_id"))
    {
        frame_id = config.find("frame_id").asString();
    }

    if(!initialize_YARP(config) )
    {
        yCError(RANGEFINDER2D_NWS_YARP) << streamingPortName << "Error initializing YARP ports";
        return false;
    }

    if(config.check("subdevice"))
    {
        Property       p;
        PolyDriverList driverlist;
        p.fromString(config.toString(), false);
        p.put("device", config.find("subdevice").asString());

        if(!driver.open(p) || !driver.isValid())
        {
            yCError(RANGEFINDER2D_NWS_YARP) << "failed to open subdevice.. check params";
            return false;
        }

        driverlist.push(&driver, "1");
        if(!attachAll(driverlist))
        {
            yCError(RANGEFINDER2D_NWS_YARP) << "failed to open subdevice.. check params";
            return false;
        }
        isDeviceOwned = true;
    }
    return true;
}

bool Rangefinder2D_nws_yarp::initialize_YARP(yarp::os::Searchable &params)
{
    if (!streamingPort.open(streamingPortName))
        {
            yCError(RANGEFINDER2D_NWS_YARP, "failed to open port %s", streamingPortName.c_str());
            return false;
        }
    if (!rpcPort.open(rpcPortName))
        {
            yCError(RANGEFINDER2D_NWS_YARP, "failed to open port %s", rpcPortName.c_str());
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
        ret &= sens_p->getRawData(ranges);
        ret &= sens_p->getDeviceStatus(status);

        if (ret)
        {
            if(iTimed)
                lastStateStamp = iTimed->getLastInputStamp();
            else
                lastStateStamp.update(yarp::os::Time::now());

            int ranges_size = ranges.size();
            YARP_UNUSED(ranges_size);

            yarp::dev::LaserScan2D& b = streamingPort.prepare();
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
            yCError(RANGEFINDER2D_NWS_YARP, "%s: Sensor returned error", streamingPortName.c_str());
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

    detachAll();
    return true;
}
