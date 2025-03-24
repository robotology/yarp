/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "Battery_nws_yarp.h"
#include <sstream>
#include <string>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <time.h>
#include <stdlib.h>

using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::os;

namespace {
YARP_LOG_COMPONENT(BATTERYWRAPPER, "yarp.devices.Battery_nws_yarp")
}

Battery_nws_yarp::Battery_nws_yarp() : PeriodicThread(DEFAULT_THREAD_PERIOD)
{
    m_period = DEFAULT_THREAD_PERIOD;
    memset(m_log_buffer, 0, 1024);
}

Battery_nws_yarp::~Battery_nws_yarp()
{
    threadRelease();
    m_ibattery_p = nullptr;
}

bool Battery_nws_yarp::attach(PolyDriver* driver)
{
    if (driver==nullptr)
    {
        yCError(BATTERYWRAPPER, "Invalid pointer to device driver received");
        return false;
    }

    driver->view(m_ibattery_p);
    if (nullptr == m_ibattery_p)
    {
        yCError(BATTERYWRAPPER, "Unable to view IBattery interface");
        return false;
    }
    m_msgsImpl = std::make_unique<IBatteryMsgsImpl>(m_ibattery_p);

    PeriodicThread::setPeriod(m_period);
    PeriodicThread::start();
    return true;
}

bool Battery_nws_yarp::detach()
{
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }
    m_ibattery_p = nullptr;
    return true;
}

bool Battery_nws_yarp::read(yarp::os::ConnectionReader& connection)
{
    if (!connection.isValid()) { return false;}
    if (!m_msgsImpl) { return false;}

    bool b = m_msgsImpl->read(connection);
    if (b)
    {
        return true;
    }
    else
    {
        yCError(BATTERYWRAPPER, "read() Command failed");
        return false;
    }
}

bool Battery_nws_yarp::threadInit()
{
    return true;
}

bool Battery_nws_yarp::open(yarp::os::Searchable &config)
{
    if (!parseParams(config)) { return false; }

    m_streamingPortName  = m_name + "/data:o";
    m_rpcPortName = m_name + "/rpc:i";

    if(!initialize_YARP(config))
    {
        yCError(BATTERYWRAPPER) << m_sensorId << "Error initializing YARP ports";
        return false;
    }

    if (m_enable_log)
    {
        yCInfo(BATTERYWRAPPER, "writing to log file batteryLog.txt");
        m_logFile = fopen("batteryLog.txt", "w");
    }

    return true;
}

bool Battery_nws_yarp::initialize_YARP(yarp::os::Searchable &params)
{
    if (!m_streamingPort.open(m_streamingPortName.c_str()))
    {
        yCError(BATTERYWRAPPER) << "Error opening port" << m_streamingPortName;
        return false;
    }
    if (!m_rpcPort.open(m_rpcPortName.c_str()))
    {
        yCError(BATTERYWRAPPER) << "Error opening port" << m_rpcPortName;
        return false;
    }
    m_rpcPort.setReader(*this);
    return true;
}

void Battery_nws_yarp::threadRelease()
{
}

void Battery_nws_yarp::run()
{
    if (m_ibattery_p!=nullptr)
    {
        m_log_buffer[0] = 0;

        //acquire data from the wrapped device
        bool ret_sts, ret_chg, ret_vlt, ret_cur, ret_tmp;
        {
            double tmp;
            ret_chg = m_ibattery_p->getBatteryCharge(tmp);
            if (ret_chg) {
                m_battery_charge = tmp;
            }
        }
        {
            double tmp;
            ret_vlt = m_ibattery_p->getBatteryVoltage(tmp);
            if (ret_vlt) {
                m_battery_voltage = tmp;
            }
        }
        {
            double tmp;
            ret_cur = m_ibattery_p->getBatteryCurrent(tmp);
            if (ret_cur) {
                m_battery_current = tmp;
            }
        }
        {
            double tmp;
            ret_tmp = m_ibattery_p->getBatteryTemperature(tmp);
            if (ret_tmp) {
                m_battery_temperature = tmp;
            }
        }
        {
            IBattery::Battery_status tmp;
            ret_sts = m_ibattery_p->getBatteryStatus(tmp);
            if (ret_sts) {
                m_battery_status = tmp;
            }
        }

        if (ret_sts)
        {
            m_lastStateStamp.update();
            yarp::dev::BatteryData& b = m_streamingPort.prepare();
            b.voltage = m_battery_voltage;
            b.current = m_battery_current;
            b.charge = m_battery_charge;
            b.temperature = m_battery_temperature;
            b.status = m_battery_status;
            m_streamingPort.setEnvelope(m_lastStateStamp);
            m_streamingPort.write();

            // if the battery is not charging, checks its status of charge
            if (m_battery_status > 0.4) {
                check_battery_status(m_battery_charge);
            }

            // save data to file
            if (m_enable_log)
            {
                time_t rawtime;
                struct tm * timeinfo;
                time(&rawtime);
                timeinfo = localtime(&rawtime);
                char* battery_timestamp = asctime(timeinfo);
                std::snprintf(m_log_buffer, 1024, "battery status: %+6.1fA   % 6.1fV   charge:% 6.1f%%    time: %s", m_battery_current, m_battery_voltage, m_battery_charge, battery_timestamp);
                fprintf(m_logFile, "%s", m_log_buffer);
            }
        }
        else
        {
            yCError(BATTERYWRAPPER, "BatteryWrapper: %s: Sensor returned error", m_sensorId.c_str());
        }
    }
}

bool Battery_nws_yarp::close()
{
    yCTrace(BATTERYWRAPPER, "BatteryWrapper::Close");
    if (PeriodicThread::isRunning())
    {
        PeriodicThread::stop();
    }

    m_streamingPort.close();
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

void Battery_nws_yarp::notify_message(std::string msg)
{
#ifdef WIN32
    yCWarning(BATTERYWRAPPER, "%s", msg.c_str());
#else
    yCWarning(BATTERYWRAPPER, "%s", msg.c_str());
    std::string cmd = "echo " + msg + " | wall";
    int retval;
    retval = system(cmd.c_str());
    yCDebug(BATTERYWRAPPER) << "system executed command" << cmd.c_str() << " with return value:" << retval;
#endif
}

void Battery_nws_yarp::emergency_shutdown(std::string msg)
{
#ifdef WIN32
    std::string cmd;
    cmd = "shutdown /s /t 120 /c " + msg;
    yCWarning(BATTERYWRAPPER, "%s", msg.c_str());
    system(cmd.c_str());
#else
    std::string cmd;
    int retval;
    yCWarning(BATTERYWRAPPER, "%s", msg.c_str());
    cmd = "echo " + msg + " | wall";
    retval = system(cmd.c_str());
    yCDebug(BATTERYWRAPPER) << "system executed command" << cmd.c_str() << " with return value:" << retval;

    cmd = "sudo shutdown -h 2 " + msg;
    retval = system(cmd.c_str());
    yCDebug(BATTERYWRAPPER) << "system executed command" << cmd.c_str() << " with return value:" << retval;

#ifdef ICUB_SSH_SHUTDOWN
    cmd = "ssh icub@pc104 sudo shutdown -h 2";
    retval = system(cmd.c_str());
    yCDebug(BATTERYWRAPPER) << "system executed command" << cmd.c_str() << " with return value:" << retval;
#endif
#endif
}

void Battery_nws_yarp::check_battery_status(double battery_charge)
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

void Battery_nws_yarp::stop_robot(std::string quit_port)
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

return_get_BatteryInfo    IBatteryMsgsImpl::getBatteryInfoRPC()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return_get_BatteryInfo response;
    std::string info;

    auto ret = m_iBat->getBatteryInfo(info);
    if(!ret)
    {
        yCError(BATTERYWRAPPER) << "Could not retrieve the battery status";
        response.result = ret;
        return response;
    }

    response.result = ret;
    response.info = info;

    return response;
}

IBatteryMsgsImpl::IBatteryMsgsImpl(yarp::dev::IBattery* _iBattery)
{
    m_iBat = _iBattery;
}
