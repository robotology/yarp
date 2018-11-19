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

    yarp::dev::IBattery*   ibat = nullptr;
    yarp::dev::PolyDriver* drv = nullptr;

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
    options.put("robot", robot_name);
    options.put("device", "batteryClient");
    options.put("local", localPort);
    options.put("remote", remotePort);
    options.put("period", 10);

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

    MainWindow w(rf, ibat, nullptr);
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
