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
        yInfo() << "--port <name>         the full name of the output port";
        yInfo() << "--step_lin_vel        the increase step of linear velocity (m/s)";
        yInfo() << "--step_ang_vel        the increase step of angular velocity (deg/s)";
        yInfo() << "--default_lin_vel     the default value for max linear velocity (m/s)";
        yInfo() << "--default_ang_vel     the default value for max angular velocity (deg/s)";
        return 0;
    }

    MainWindow w(rf, nullptr);
    w.show();
    int ret = a.exec();

    return ret;

}
