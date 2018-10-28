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

#include "BatteryWrapper.h"
#include <sstream>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;

// needed for the driver factory.
yarp::dev::DriverCreator *createBatteryWrapper() {
    return new DriverCreatorOf<yarp::dev::BatteryWrapper>("batteryWrapper",
        "batteryWrapper",
        "yarp::dev::BatteryWrapper");
}


/**
  * It reads the data from an battery sensor and sends them on one port.
  * It also creates one rpc port.
  */

BatteryWrapper::BatteryWrapper() : PeriodicThread(DEFAULT_THREAD_PERIOD)
{
    _period = DEFAULT_THREAD_PERIOD;
    ibattery_p = nullptr;
    ownDevices = false;
}

BatteryWrapper::~BatteryWrapper()
{
    threadRelease();
    ibattery_p = nullptr;
}

/**
  * Specify which battery sensor this thread has to read from.
  */

bool BatteryWrapper::attachAll(const PolyDriverList &battery2attach)
{
    if (ownDevices)
    {
        return false;
    }

    if (battery2attach.size() != 1)
    {
        yError("BatteryWrapper: cannot attach more than one device");
        return false;
    }

    yarp::dev::PolyDriver * Idevice2attach = battery2attach[0]->poly;

    if (Idevice2attach->isValid())
    {
        Idevice2attach->view(ibattery_p);
    }

    if(nullptr == ibattery_p)
    {
        yError("BatteryWrapper: subdevice passed to attach method is invalid");
        return false;
    }
    attach(ibattery_p);
    PeriodicThread::setPeriod(_period);
    return PeriodicThread::start();
}

bool BatteryWrapper::detachAll()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    ibattery_p = nullptr;
    return true;
}

void BatteryWrapper::attach(yarp::dev::IBattery *s)
{
    ibattery_p=s;
}

void BatteryWrapper::detach()
{
    ibattery_p = nullptr;
}

bool BatteryWrapper::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool ok = in.read(connection);
    if (!ok) return false;

    // parse in, prepare out
    int code = in.get(0).asVocab();
    bool ret = false;
    if (code == VOCAB_IBATTERY)
    {
        int cmd = in.get(1).asVocab();
        if (cmd == VOCAB_BATTERY_INFO)
        {
            if (ibattery_p)
            {
                std::string info;
                ibattery_p->getBatteryInfo(info);
                out.addVocab(VOCAB_IS);
                out.addVocab(cmd);
                out.addString(info);
                ret = true;
            }
        }
        else
        {
            yError("Invalid vocab received in BatteryWrapper");
        }
    }
    else
    {
        yError("Invalid vocab received in BatteryWrapper");
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

bool BatteryWrapper::threadInit()
{

    return true;
}

void BatteryWrapper::setId(const std::string &id)
{
    sensorId=id;
}

std::string BatteryWrapper::getId()
{
    return sensorId;
}


bool BatteryWrapper::open(yarp::os::Searchable &config)
{
    Property params;
    params.fromString(config.toString());

    if (!config.check("period"))
    {
        _period = 1.0;
        yWarning() << "BatteryWrapper: missing 'period' parameter. Assuming default value 1.0 s\n";
    }
    else
    {
        _period = config.find("period").asInt32() / 1000.0;
    }
    yInfo() << "BatteryWrapper using period: " << _period << "s";

    if (!config.check("name"))
    {
        yError() << "BatteryWrapper: missing 'name' parameter. Check you configuration file; it must be like:";
        yError() << "   name:         full name of the port, like /robotName/deviceId/sensorType:o";
        return false;
    }
    else
    {
        streamingPortName  = config.find("name").asString();
        rpcPortName = streamingPortName + "/rpc:i";
        setId("batteryWrapper");
    }

    if(!initialize_YARP(config))
    {
        yError() << sensorId << "Error initializing YARP ports";
        return false;
    }

    if (config.check("subdevice"))
    {
        Property       p;
        PolyDriverList driverlist;

        p.fromString(config.toString(), false);
        p.put("device", config.find("subdevice").asString());
        p.unput("subdevice");

        if (!driver.open(p) || !driver.isValid())
        {
            yError() << "BatteryWrapper: failed to open subdevice.. check params";
            return false;
        }

        driverlist.push(&driver, "1");
        if (!attachAll(driverlist))
        {
            yError() << "BatteryWrapper: failed to open subdevice.. check params";
            return false;
        }
        ownDevices = true;
    }

    return true;
}

bool BatteryWrapper::initialize_YARP(yarp::os::Searchable &params)
{
    if (!streamingPort.open(streamingPortName.c_str()))
    {
        yError() << "Error opening port " << streamingPortName << "\n";
        return false;
    }
    if (!rpcPort.open(rpcPortName.c_str()))
    {
        yError() << "Error opening port " << rpcPortName << "\n";
        return false;
    }
    rpcPort.setReader(*this);
    return true;
}

void BatteryWrapper::threadRelease()
{
}

void BatteryWrapper::run()
{
    if (ibattery_p!=nullptr)
    {
        double charge  = 0;
        double voltage = 0;
        double current = 0;
        double temperature = 0;
        IBattery::Battery_status status;

        bool ret = true;
        ret &= ibattery_p->getBatteryCharge(charge);
        ret &= ibattery_p->getBatteryVoltage(voltage);
        ret &= ibattery_p->getBatteryCurrent(current);
        ret &= ibattery_p->getBatteryTemperature(temperature);
        ret &= ibattery_p->getBatteryStatus(status);

        if (ret)
        {
            lastStateStamp.update();
            yarp::os::Bottle& b = streamingPort.prepare();
            b.clear();
            b.addFloat64(voltage); //0
            b.addFloat64(current); //1
            b.addFloat64(charge);  //2
            b.addFloat64(temperature); //3
            b.addInt32(status); //4
            streamingPort.setEnvelope(lastStateStamp);
            streamingPort.write();
        }
        else
        {
            yError("BatteryWrapper: %s: Sensor returned error", sensorId.c_str());
        }
    }
}

bool BatteryWrapper::close()
{
    yTrace("BatteryWrapper::Close");
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }

    //close the device
    driver.close();

    streamingPort.interrupt();
    streamingPort.close();
    rpcPort.interrupt();
    rpcPort.close();

    PeriodicThread::stop();
    detachAll();
    return true;
}
