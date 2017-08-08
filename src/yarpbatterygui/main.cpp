/*
* Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
* Author: Marco Randazzo
* Date: June 2015
* email:   marco.randazzo@iit.it
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
    QApplication a(argc, argv);

    // Guis shall run always on system clock. No need to have it in synch with network clock
    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);

    if (!yarp.checkNetwork())
    {
        yError("Error initializing yarp network (is yarpserver running?)");
        return 0;
    }

    ResourceFinder rf;
    rf.setVerbose(true);
    rf.configure(argc,argv);

    if (rf.check("help"))
    {
        yInfo() << "Options:";
        yInfo() << "No options at the moment";
        return 0;
    }

    yarp::dev::IBattery*   ibat = 0;
    yarp::dev::PolyDriver* drv = 0;

    std::string robot_name = "icub";
    if (rf.check("robot"))
    {
        robot_name = rf.find("robot").asString();
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
    std::string remotePort = "/" + robot_name + "/battery:o";

    yarp::os::Property options;
    options.put("robot", robot_name.c_str());
    options.put("device", "batteryClient");
    options.put("local", localPort.c_str());
    options.put("remote", remotePort.c_str());
    options.put("period", 10);

    drv = new yarp::dev::PolyDriver(options);
    if (!drv || !(drv->isValid()))
    {
        yError("Problems instantiating the device driver");
        delete drv;
        return false;
    }

    drv->view(ibat);
    if (ibat == 0)
    {
        yError("Problems viewing the battery interface");
        drv->close();
        delete drv;
        return false;
    }

    MainWindow w(rf, ibat, NULL);
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
