/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <fakeBattery.h>

#include <yarp/os/Time.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Log.h>
#include <iostream>
#include <string.h>
#include <time.h>
#include <stdlib.h>

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;

FakeBattery::FakeBattery(int period) : PeriodicThread((double)period / 1000.0)
{
    logEnable = false;
    verboseEnable = false;
    screenEnable = false;
    debugEnable = false;
    shutdownEnable = false;
}

FakeBattery::~FakeBattery()
{
}

bool FakeBattery::open(yarp::os::Searchable& config)
{
    Bottle& group_general = config.findGroup("GENERAL");

    int period = config.find("thread_period").asInt32();
    setPeriod((double)period / 1000.0);

    if (group_general.isNull())
    {
        yWarning() << "GENERAL group parameters missing, assuming default";
    }
    else
    {
        // Other options
        this->logEnable = group_general.check("logToFile", Value(0), "enable / disable the log to file").asBool();
        this->verboseEnable = group_general.check("verbose", Value(0), "enable/disable the verbose mode").asBool();
        this->screenEnable = group_general.check("screen", Value(0), "enable/disable the screen output").asBool();
        this->debugEnable = group_general.check("debug", Value(0), "enable/disable the debug mode").asBool();
        this->shutdownEnable = group_general.check("shutdown", Value(0), "enable/disable the automatic shutdown").asBool();
    }

    PeriodicThread::start();
    return true;
}

bool FakeBattery::close()
{
    //stop the thread
    PeriodicThread::stop();

    return true;
}

bool FakeBattery::threadInit()
{
    battery_info = "fake battery system v1.0";
    battery_voltage     = 0.0;
    battery_current     = 0.0;
    battery_charge      = 0.0;
    battery_temperature = 0.0;
    timeStamp = yarp::os::Time::now();
    memset(log_buffer, 0, 1024);

    if (logEnable)
    {
        yInfo("writing to log file batteryLog.txt");
        logFile = fopen("batteryLog.txt", "w");
    }

    return true;
}

void FakeBattery::run()
{
    double timeNow=yarp::os::Time::now();
    mutex.wait();

    //if 100ms have passed since the last received message
    if (timeStamp+0.1<timeNow)
    {
        //status=IBattery::BATTERY_TIMEOUT;
    }

    log_buffer[0] = 0;
    
    battery_voltage += 0.5;
    battery_current += 0.5;
    battery_charge += 0.5;
    battery_temperature += 0.5;

    if (battery_voltage > 50)
    {
        battery_voltage = 40.0;
        battery_current = 5.0;
        battery_charge = 72.0;
        battery_temperature = 35.0;
    }



    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char* battery_timestamp = asctime(timeinfo);
    snprintf(log_buffer, 1024, "battery status: %+6.1fA   % 6.1fV   charge:% 6.1f%%    time: %s", battery_current, battery_voltage, battery_charge, battery_timestamp);

    // if the battery is not charging, checks its status of charge
    if (battery_current>0.4) check_battery_status();

    // print data to screen
    if (screenEnable)
    {
        yDebug("FakeBattery::run() log_buffer is: %s", log_buffer);
    }

    // save data to file
    if (logEnable)
    {
        fprintf(logFile, "%s", log_buffer);
    }

    mutex.post();
}

bool FakeBattery::getBatteryVoltage(double &voltage)
{
    this->mutex.wait();
    voltage = battery_voltage;
    this->mutex.post();
    return true;
}

bool FakeBattery::getBatteryCurrent(double &current)
{
    this->mutex.wait();
    current = battery_current;
    this->mutex.post();
    return true;
}

bool FakeBattery::getBatteryCharge(double &charge)
{
    this->mutex.wait();
    charge = battery_charge;
    this->mutex.post();
    return true;
}

bool FakeBattery::getBatteryStatus(Battery_status &status)
{
    this->mutex.wait();
    status = BATTERY_OK_IN_USE;
    this->mutex.post();
    return true;
}

bool FakeBattery::getBatteryTemperature(double &temperature)
{
    this->mutex.wait();
    temperature = 20;
    this->mutex.post();
    return true;
}

bool FakeBattery::getBatteryInfo(string &info)
{
    this->mutex.wait();
    info = battery_info;
    this->mutex.post();
    return true;
}

void FakeBattery::threadRelease()
{
    yTrace("FakeBattery Thread released\n");
}

void FakeBattery::notify_message(string msg)
{
#ifdef WIN32
    yWarning("%s", msg.c_str());
#else
    yWarning("%s", msg.c_str());
    string cmd = "echo " + msg + " | wall";
    system(cmd.c_str());
#endif
}

void emergency_shutdown(string msg)
{
#ifdef WIN32
    string cmd;
    cmd = "shutdown /s /t 120 /c " + msg;
    yWarning("%s", msg.c_str());
    system(cmd.c_str());
#else
    string cmd;
    yWarning("%s", msg.c_str());
    cmd = "echo " + msg + " | wall";
    system(cmd.c_str());

    cmd = "sudo shutdown -h 2 " + msg;
    system(cmd.c_str());

    cmd = "ssh icub@pc104 sudo shutdown -h 2";
    system(cmd.c_str());
#endif
}

void FakeBattery::check_battery_status()
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

    if (battery_charge < 15)
    {
        if (notify_15) { notify_message("WARNING: battery charge below 15%"); notify_15 = false; }
    }
    if (battery_charge < 12)
    {
        if (notify_12) { notify_message("WARNING: battery charge below 12%"); notify_12 = false; }
    }
    if (battery_charge < 10)
    {
        if (notify_10) { notify_message("WARNING: battery charge below 10%"); notify_10 = false; }
    }
    if (battery_charge < 5)
    {
        if (notify_0)
        {
            if (shutdownEnable)
            {
                emergency_shutdown("CRITICAL WARNING: battery charge below critical level 5%. The robot will be stopped and the system will shutdown in 2mins.");
                stop_robot("/icub/quit");
                stop_robot("/ikart/quit");
                notify_0 = false;
            }
            else
            {
                notify_message("CRITICAL WARNING: battery charge reached critical level 5%, but the emergency shutodown is currently disabled!");
                notify_0 = false;
            }
        }
    }
}

void FakeBattery::stop_robot(string quit_port)
{
    //typical quit_port:
    // "/icub/quit"
    // "/ikart/quit"
    //if (yarp_found)
    {
        Port port_shutdown;
        port_shutdown.open((localName + "/shutdown").c_str());
        yarp::os::Network::connect((localName + "/shutdown").c_str(), quit_port.c_str());
        Bottle bot;
        bot.addString("quit");
        port_shutdown.write(bot);
        port_shutdown.interrupt();
        port_shutdown.close();
    }
}
