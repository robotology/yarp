/*
 * Copyright (C) 2013 Istituto Italiano di Tecnologia (IIT)
 * Authors: Marco Randazzo <marco.randazzo@iit.it>, David Estevez <destevez@ing.uc3m.es>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
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

BatteryWrapper::BatteryWrapper() : RateThread(DEFAULT_THREAD_PERIOD)
{
    _rate = DEFAULT_THREAD_PERIOD;
    battery_p = nullptr;
}

BatteryWrapper::~BatteryWrapper()
{
    battery_p = nullptr;
}

/**
  * Specify which battery sensor this thread has to read from.
  */

bool BatteryWrapper::attachAll(const PolyDriverList &battery2attach)
{
    //check if we already instantiated a subdevice previously
    if (ownDevices)
        return false;

    if (battery2attach.size() != 1)
    {
        yError("BatteryWrapper: cannot attach more than one device");
        return false;
    }

    yarp::dev::PolyDriver * Idevice2attach = battery2attach[0]->poly;

    if (Idevice2attach->isValid())
    {
        Idevice2attach->view(battery_p);
    }

    if(nullptr == battery_p)
    {
        yError("BatteryWrapper: subdevice passed to attach method is invalid");
        return false;
    }

    attach(battery_p);
    RateThread::setRate(_rate);
    return RateThread::start();
}

bool BatteryWrapper::detachAll()
{
    //check if we already instantiated a subdevice previously
    if (ownDevices)
        return false;

    battery_p = nullptr;
    return true;
}

void BatteryWrapper::attach(yarp::dev::IBattery *s)
{
    battery_p=s;
}

void BatteryWrapper::detach()
{
    battery_p = nullptr;
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
            if (battery_p)
            {
                yarp::os::ConstString info;
                battery_p->getBatteryInfo(info);
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

bool BatteryWrapper::openDeferredAttach(yarp::os::Searchable &prop)
{
    // nothing to do here?
    if( (subDeviceOwned != nullptr) || (ownDevices == true) )
        yError() << "BatteryWrapper: something wrong with the initialization.";
    return true;
}

bool BatteryWrapper::openAndAttachSubDevice(yarp::os::Searchable &prop)
{
    Property p;
    subDeviceOwned = new PolyDriver;
    p.fromString(prop.toString().c_str());

    p.unput("device");
    p.put("device", prop.find("subdevice").asString());  // subdevice was already checked before

    // if error occour during open, quit here.
    yDebug("Opening BatteryWrapper subdevice...");
    subDeviceOwned->open(p);

    if (!subDeviceOwned->isValid())
    {
        yError() << "Opening BatteryWrapper subdevice \"" << prop.find("subdevice").asString()
                 << "\"... FAILED";
        return false;
    }

    subDeviceOwned->view(battery_p);

    if (battery_p == nullptr)
    {
        yError() << "Opening IBattery interface of BatteryWrapper subdevice \"" << prop.find("subdevice").asString()
               << "\"... FAILED";
        return false;
    }

    attach(battery_p);
    RateThread::setRate(_rate);
    return RateThread::start();
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
    params.fromString(config.toString().c_str());
    yTrace() << "BatteryWrapper params are: " << config.toString();

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
        setId("BatteryWrapper");
    }

    if(!initialize_YARP(config) )
    {
        yError() << sensorId << "Error initializing YARP ports";
        return false;
    }

    // check if we need to create subdevice or if they are
    // passed later on thorugh attachAll()
    if(config.check("subdevice"))
    {
        ownDevices=true;
        if(! openAndAttachSubDevice(config))
        {
            yError("BatteryWrapper: error while opening subdevice\n");
            return false;
        }
    }
    else
    {
        ownDevices=false;
        if(!openDeferredAttach(config))
            return false;
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

void BatteryWrapper::run()
{
    if (battery_p!=nullptr)
    {
        double charge  = 0;
        double voltage = 0;
        double current = 0;
        double temperature = 0;

        bool ret = true;
        ret &= battery_p->getBatteryCharge(charge);
        ret &= battery_p->getBatteryVoltage(voltage);
        ret &= battery_p->getBatteryCurrent(current);
        ret &= battery_p->getBatteryTemperature(temperature);

        if (ret)
        {
            lastStateStamp.update();
            yarp::os::Bottle& b = streamingPort.prepare();
            b.clear();
            b.addDouble(voltage);
            b.addDouble(current);
            b.addDouble(charge);
            b.addDouble(temperature);
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
    if (RateThread::isRunning())
    {
        RateThread::stop();
    }

    detachAll();

    if(subDeviceOwned)
    {
        subDeviceOwned->close();
        delete subDeviceOwned;
        subDeviceOwned = nullptr;
    }

    // close ports
    streamingPort.interrupt();
    streamingPort.close();
    rpcPort.interrupt();
    rpcPort.close();

    return true;
}
