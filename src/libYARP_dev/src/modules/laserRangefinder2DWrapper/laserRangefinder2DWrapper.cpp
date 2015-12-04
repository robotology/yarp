// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <sstream>
#include "laserRangefinder2DWrapper.h"
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;

// needed for the driver factory.
yarp::dev::DriverCreator *createLaserRangefinder2DWrapper() {
    return new DriverCreatorOf<yarp::dev::LaserRangefinder2DWrapper>("laserRangefinder2DWrapper",
        "laserRangefinder2DWrapper",
        "yarp::dev::LaserRangefinder2Dwrapper");
}


/**
  * It reads the data from an battery sensor and sends them on one port.
  * It also creates one rpc port.
  */

LaserRangefinder2DWrapper::LaserRangefinder2DWrapper() : RateThread(DEFAULT_THREAD_PERIOD)
{
    _rate = DEFAULT_THREAD_PERIOD;
    laser_p = NULL;
}

LaserRangefinder2DWrapper::~LaserRangefinder2DWrapper()
{
    threadRelease();
    laser_p = NULL;
}

/**
  * Specify which battery sensor this thread has to read from.
  */

bool LaserRangefinder2DWrapper::attachAll(const PolyDriverList &device2attach)
{
    if (device2attach.size() != 1)
    {
        yError("LaserRangefinder2DWrapper: cannot attach more than one device");
        return false;
    }

    yarp::dev::PolyDriver * Idevice2attach = device2attach[0]->poly;

    if (Idevice2attach->isValid())
    {
        Idevice2attach->view(laser_p);
    }

    if (NULL == laser_p)
    {
        yError("LaserRangefinder2DWrapper: subdevice passed to attach method is invalid");
        return false;
    }
    attach(laser_p);
    RateThread::setRate(_rate);
    RateThread::start();

    return true;
}

bool LaserRangefinder2DWrapper::detachAll()
{
    laser_p = NULL;
    return true;
}

void LaserRangefinder2DWrapper::attach(yarp::dev::ILaserRangefinder2D *s)
{
    laser_p = s;
}

void LaserRangefinder2DWrapper::detach()
{
    laser_p = NULL;
}

bool LaserRangefinder2DWrapper::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool ok = in.read(connection);
    if (!ok) return false;

    // parse in, prepare out
    int code = in.get(0).asVocab();
    bool ret = false;
    if (code == VOCAB_ILASER2D)
    {
        int cmd = in.get(1).asVocab();
        if (cmd == VOCAB_DEVICE_INFO)
        {
            if (laser_p)
            {
                yarp::os::ConstString info;
                laser_p->getDeviceInfo(info);
                out.addVocab(VOCAB_IS);
                out.addVocab(cmd);
                out.addString(info);
                ret = true;
            }
        }
        else
        {
            yError("Invalid vocab received in LaserRangefinder2DWrapper");
        }
    }
    else
    {
        yError("Invalid vocab received in LaserRangefinder2DWrapper");
    }

    if (!ret)
    {
        out.clear();
        out.addVocab(VOCAB_FAILED);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender != NULL) {
        out.write(*returnToSender);
    }
    return true;
}

bool LaserRangefinder2DWrapper::threadInit()
{
    // open data port
    if (!streamingPort.open(streamingPortName.c_str()))
        {
            yError("LaserRangefinder2DWrapper: failed to open port %s", streamingPortName.c_str());
            return false;
        }
    return true;
}

void LaserRangefinder2DWrapper::setId(const std::string &id)
{
    sensorId=id;
}

std::string LaserRangefinder2DWrapper::getId()
{
    return sensorId;
}


bool LaserRangefinder2DWrapper::open(yarp::os::Searchable &config)
{
    Property params;
    params.fromString(config.toString().c_str());

    if (!config.check("period"))
    {
        yError() << "BatteryWrapper: missing 'period' parameter. Check you configuration file\n";
        return false;
    }
    else
        _rate = config.find("period").asInt();

    if (!config.check("name"))
    {
        yError() << "BatteryWrapper: missing 'name' parameter. Check you configuration file; it must be like:";
        yError() << "   name:         full name of the port, like /robotName/deviceId/sensorType:o";
        return false;
    }
    else
    {
        streamingPortName  = config.find("name").asString().c_str();
        rpcPortName = streamingPortName + "/rpc:i";
        setId("batteryWrapper");
    }

    if(!initialize_YARP(config) )
    {
        yError() << sensorId << "Error initializing YARP ports";
        return false;
    }
    return true;
}

bool LaserRangefinder2DWrapper::initialize_YARP(yarp::os::Searchable &params)
{
    streamingPort.open(streamingPortName.c_str());
    rpcPort.open(rpcPortName.c_str() );
    rpcPort.setReader(*this);
    return true;
}

void LaserRangefinder2DWrapper::threadRelease()
{
    streamingPort.interrupt();
    streamingPort.close();
    rpcPort.interrupt();
    rpcPort.close();
}

void LaserRangefinder2DWrapper::run()
{
    if (laser_p!=0)
    {
        yarp::sig::Vector ranges;

        bool ret = true;
        ret &= laser_p->getRangeData(ranges);

        if (ret)
        {
            lastStateStamp.update();
            yarp::os::Bottle& b = streamingPort.prepare();
            b.clear();
            b.addDouble(ranges[0]);
            streamingPort.setEnvelope(lastStateStamp);
            streamingPort.write();
        }
        else
        {
            yError("BatteryWrapper: %s: Sensor returned error", sensorId.c_str());
        }
    }
}

bool LaserRangefinder2DWrapper::close()
{
    yTrace("LaserRangefinder2DWrapper::Close");
    if (RateThread::isRunning())
    {
        RateThread::stop();
    }

    RateThread::stop();
    detachAll();
    return true;
}
