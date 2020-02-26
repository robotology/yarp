/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
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
#include <string>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <time.h>
#include <stdlib.h>

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;
using namespace std;


BatteryWrapper::BatteryWrapper() : PeriodicThread(DEFAULT_THREAD_PERIOD)
{
    m_period = DEFAULT_THREAD_PERIOD;
    m_ibattery_p = nullptr;
    m_ownDevices = false;
    memset(m_log_buffer, 0, 1024);
}

BatteryWrapper::~BatteryWrapper()
{
    threadRelease();
    m_ibattery_p = nullptr;
}

bool BatteryWrapper::attachAll(const PolyDriverList &battery2attach)
{
    if (m_ownDevices)
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
        Idevice2attach->view(m_ibattery_p);
    }

    if(nullptr == m_ibattery_p)
    {
        yError("BatteryWrapper: subdevice passed to attach method is invalid");
        return false;
    }
    attach(m_ibattery_p);
    PeriodicThread::setPeriod(m_period);
    return PeriodicThread::start();
}

bool BatteryWrapper::detachAll()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    m_ibattery_p = nullptr;
    return true;
}

void BatteryWrapper::attach(yarp::dev::IBattery *s)
{
    m_ibattery_p=s;
}

void BatteryWrapper::detach()
{
    m_ibattery_p = nullptr;
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
            if (m_ibattery_p)
            {
                std::string info;
                m_ibattery_p->getBatteryInfo(info);
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
    if (returnToSender != nullptr)
    {
        out.write(*returnToSender);
    }
    return true;
}

bool BatteryWrapper::threadInit()
{
    return true;
}

bool BatteryWrapper::open(yarp::os::Searchable &config)
{
    Property params;
    params.fromString(config.toString());

    if (!config.check("period"))
    {
        m_period = 1.0;
        yWarning() << "BatteryWrapper: missing 'period' parameter. Assuming default value 1.0 s\n";
    }
    else
    {
        m_period = config.find("period").asFloat32();
    }
    yInfo() << "BatteryWrapper using period: " << m_period << "s";

    if (!config.check("quitPortName"))
    {
        m_quitPortName = config.find("quitPortName").asString();
    }

    if (!config.check("name"))
    {
        yError() << "BatteryWrapper: missing 'name' parameter. Check you configuration file; it must be like:";
        yError() << "--name:    prefix of the ports opened by the device, e.g. /robotName/battery1";
        yError() << "/data:o and /rpc:i are automatically appended by the wrapper at the end";
        return false;
    }
    else
    {
        m_streamingPortName  = config.find("name").asString() + "/data:o";
        m_rpcPortName = config.find("name").asString() + "/rpc:i";
    }

    m_enable_shutdown = config.check("enable_shutdown", Value(0), "enable/disable the automatic shutdown").asBool();
    m_enable_log      = config.check("enable_log", Value(0),      "enable/disable log to file").asBool();

    if(!initialize_YARP(config))
    {
        yError() << m_sensorId << "Error initializing YARP ports";
        return false;
    }

    if (m_enable_log)
    {
        yInfo("writing to log file batteryLog.txt");
        m_logFile = fopen("batteryLog.txt", "w");
    }

    if (config.check("subdevice"))
    {
        PolyDriverList driverlist;
        Property p;
        p.fromString(config.toString());
        p.unput("device");
        p.unput("subdevice");
        p.put("device", config.find("subdevice").asString());
        p.setMonitor(config.getMonitor(), "subdevice"); // pass on any monitoring

        if (!m_driver.open(p) || !m_driver.isValid())
        {
            yError() << "BatteryWrapper: failed to open subdevice.. check params";
            return false;
        }

        driverlist.push(&m_driver, "1");
        if (!attachAll(driverlist))
        {
            yError() << "BatteryWrapper: failed to open subdevice.. check params";
            return false;
        }
        m_ownDevices = true;
    }

    return true;
}

bool BatteryWrapper::initialize_YARP(yarp::os::Searchable &params)
{
    if (!m_streamingPort.open(m_streamingPortName.c_str()))
    {
        yError() << "Error opening port " << m_streamingPortName << "\n";
        return false;
    }
    if (!m_rpcPort.open(m_rpcPortName.c_str()))
    {
        yError() << "Error opening port " << m_rpcPortName << "\n";
        return false;
    }
    m_rpcPort.setReader(*this);
    return true;
}

void BatteryWrapper::threadRelease()
{
}

void BatteryWrapper::run()
{
    if (m_ibattery_p!=nullptr)
    {
        m_log_buffer[0] = 0;
        double battery_charge  = 0;
        double battery_voltage = 0;
        double battery_current = 0;
        double battery_temperature = 0;
        IBattery::Battery_status status;

        bool ret = true;
        ret &= m_ibattery_p->getBatteryCharge(battery_charge);
        ret &= m_ibattery_p->getBatteryVoltage(battery_voltage);
        ret &= m_ibattery_p->getBatteryCurrent(battery_current);
        ret &= m_ibattery_p->getBatteryTemperature(battery_temperature);
        ret &= m_ibattery_p->getBatteryStatus(status);

        if (ret)
        {
            m_lastStateStamp.update();
            yarp::os::Bottle& b = m_streamingPort.prepare();
            b.clear();
            b.addFloat64(battery_voltage); //0
            b.addFloat64(battery_current); //1
            b.addFloat64(battery_charge);  //2
            b.addFloat64(battery_temperature); //3
            b.addInt32(status); //4
            m_streamingPort.setEnvelope(m_lastStateStamp);
            m_streamingPort.write();

            // if the battery is not charging, checks its status of charge
            if (battery_current>0.4) check_battery_status(battery_charge);

            // save data to file
            if (m_enable_log)
            {
                time_t rawtime;
                struct tm * timeinfo;
                time(&rawtime);
                timeinfo = localtime(&rawtime);
                char* battery_timestamp = asctime(timeinfo);
                snprintf(m_log_buffer, 1024, "battery status: %+6.1fA   % 6.1fV   charge:% 6.1f%%    time: %s", battery_current, battery_voltage, battery_charge, battery_timestamp);
                fprintf(m_logFile, "%s", m_log_buffer);
            }
        }
        else
        {
            yError("BatteryWrapper: %s: Sensor returned error", m_sensorId.c_str());
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
    m_driver.close();

    m_streamingPort.interrupt();
    m_streamingPort.close();
    m_rpcPort.interrupt();
    m_rpcPort.close();

    // save data to file
    if (m_enable_log)
    {
        fclose(m_logFile);
    }

    PeriodicThread::stop();
    detachAll();
    return true;
}

void BatteryWrapper::notify_message(string msg)
{
#ifdef WIN32
    yWarning("%s", msg.c_str());
#else
    yWarning("%s", msg.c_str());
    string cmd = "echo " + msg + " | wall";
    int retval;
    retval = system(cmd.c_str());
    yDebug() << "system executed command" << cmd.c_str() << " with return value:" << retval;
#endif
}

void BatteryWrapper::emergency_shutdown(string msg)
{
#ifdef WIN32
    string cmd;
    cmd = "shutdown /s /t 120 /c " + msg;
    yWarning("%s", msg.c_str());
    system(cmd.c_str());
#else
    string cmd;
    int retval;
    yWarning("%s", msg.c_str());
    cmd = "echo " + msg + " | wall";
    retval = system(cmd.c_str());
    yDebug() << "system executed command" << cmd.c_str() << " with return value:" << retval;

    cmd = "sudo shutdown -h 2 " + msg;
    retval = system(cmd.c_str());
    yDebug() << "system executed command" << cmd.c_str() << " with return value:" << retval;

#ifdef ICUB_SSH_SHUTDOWN
    cmd = "ssh icub@pc104 sudo shutdown -h 2";
    retval = system(cmd.c_str());
    yDebug() << "system executed command" << cmd.c_str() << " with return value:" << retval;
#endif
#endif
}

void BatteryWrapper::check_battery_status(double battery_charge)
{
    static bool notify_15 = true;
    static bool notify_12 = true;
    static bool notify_10 = true;
    static bool notify_0 = true;

    if (battery_charge > 20)
    {
        notify_15 = true;
        notify_12 = true;
        notify_10 = true;
        notify_0 = true;
    }

    if (battery_charge < 5)
    {
        if (notify_0)
        {
            if (m_enable_shutdown)
            {
                emergency_shutdown("CRITICAL WARNING: battery charge below critical level 5%. The robot will be stopped and the system will shutdown in 2mins.");
                if (m_quitPortName != "") { stop_robot(m_quitPortName); }
                notify_0 = false;
            }
            else
            {
                notify_message("CRITICAL WARNING: battery charge reached critical level 5%, but the emergency shutodown is currently disabled!");
                notify_0 = false;
            }
        }
    }
    else if (battery_charge < 10)
    {
        if (notify_10) { notify_message("WARNING: battery charge below 10%"); notify_10 = false; }
    }
    else if (battery_charge < 12)
    {
        if (notify_12) { notify_message("WARNING: battery charge below 12%"); notify_12 = false; }
    }
    else if (battery_charge < 15)
    {
        if (notify_15) { notify_message("WARNING: battery charge below 15%"); notify_15 = false; }
    }
}

void BatteryWrapper::stop_robot(string quit_port)
{
    //typical quit_port:
    // "/icub/quit"
    // "/ikart/quit"

    Port port_shutdown;
    port_shutdown.open((m_streamingPortName + "/shutdown:o").c_str());
    yarp::os::Network::connect((m_streamingPortName + "/shutdown:o").c_str(), quit_port.c_str());
    Bottle bot;
    bot.addString("quit");
    port_shutdown.write(bot);
    port_shutdown.interrupt();
    port_shutdown.close();
}
