/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Os.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>
#include <yarp/sig/Vector.h>

#include <QApplication>
#include <QMainWindow>
#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <ctime>

#include "display.h"
#include "ui_display.h"

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;

int main(int argc, char *argv[])
{
    // Guis shall run always on system clock. No need to have it in synch with network clock
    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);

    if (!yarp.checkNetwork())
    {
        yError("Error initializing yarp network (is yarpserver running?)");
        return 0;
    }

    ResourceFinder rf;
    rf.configure(argc,argv);

    QApplication a(argc, argv);

    if (rf.check("help"))
    {
        yInfo() << "Options:";
        yInfo() << "--local <name>              The name of local opened by this module. e.g. batteryMonitor";
        yInfo() << "--remote <name>             The prefix name of the port to connect to, e.g. /mybattery.";
        yInfo() << "                            The port name is completed by the client adding /data:o, /rpc:i";
        yInfo() << "--refresh_period <seconds>  Refresh period of the gui. Default value: 10s";
        yInfo() << "--keep-above                Keep window above the others";
        return 0;
    }

    yarp::dev::IBattery*   ibat = nullptr;
    yarp::dev::PolyDriver* drv = nullptr;

    if (rf.check("robot"))
    {
        yError() << "This option is deprecated. Use --remote instead";
        return 0;
    }

    if (rf.check("name"))
    {
        yError() << "This option is deprecated. Use --remote instead";
        return 0;
    }

    std::string remotePort = "";
    if (rf.check("remote"))
    {
        remotePort = rf.find("remote").asString();
    }

    double refresh_period = 10.0;
    if (rf.check("refresh_period"))
    {
        refresh_period = rf.find("refresh_period").asFloat32();
        if (refresh_period <= 0)
        {
            yError() << "refresh_period cannot be <=0. Using default value.";
            refresh_period = 10;
        }
        yInfo() << "refresh_period set to:" << refresh_period << "s";
    }
    else
    {
        yInfo() << "refresh_period parameter missing. Using default value:" << refresh_period << "s";
    }

    char pname[500];
    int trial = 0;
    bool b;
    do
    {
        sprintf(pname, "/batteryMonitor%d:i", trial);
        b = yarp::os::Network::exists(pname);
        trial++;
    } while (b == true);
    std::string localPort = pname;

    if (rf.check("local"))
    {
        localPort = rf.find("local").asString();
    }
    yInfo() << "Using local port:" << localPort;

    yarp::os::Property options;
    options.put("device", "batteryClient");
    options.put("local", localPort);
    options.put("remote", remotePort);

    drv = new yarp::dev::PolyDriver(options);
    if (!drv || !(drv->isValid()))
    {
        yError("Problems instantiating the device driver");
        delete drv;
        return 1;
    }

    drv->view(ibat);
    if (ibat == nullptr)
    {
        yError("Problems viewing the battery interface");
        drv->close();
        delete drv;
        return 2;
    }

    MainWindow w(rf, ibat, nullptr, refresh_period);

    if (rf.check("keep-above"))
    {
        w.setWindowFlags(Qt::WindowStaysOnTopHint);
    }

    w.show();
    int ret = a.exec();

    if (drv)
    {
        yInfo("Closing Battery Interface");
        drv->close();
        delete drv;
    }

    return ret;

}
