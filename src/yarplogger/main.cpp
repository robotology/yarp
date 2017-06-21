/*
 * Copyright (C)2014  iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo
 * email:  marco.randazzo@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

#include "mainwindow.h"
#include <QApplication>
#include <yarp/logger/YarpLogger.h>
#include <yarp/os/Os.h>
#include <yarp/os/Time.h>
#include <cstdio>
#include <csignal>

static void sighandler (int signal)
{
    fprintf(stdout,"This module cannot be closed with CTRL-C. Use <quit> menu to exit\n");
}

int main(int argc, char *argv[])
{
    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);
    if (!yarp.checkNetwork())
    {
        fprintf(stderr,"ERROR: check Yarp network.\n");
        return -1;
    }

    QApplication a(argc, argv);
    yarp::os::ResourceFinder rf;
    rf.setVerbose(true);
    rf.setDefaultConfigFile("yarprunLogger.ini");           //overridden by --from parameter
    rf.setDefaultContext("yarprunLogger");                  //overridden by --context parameter
    rf.configure(argc,argv);

    bool cannot_close = rf.check("no_stop");
    if (cannot_close)
    {
        std::signal(SIGINT, sighandler);
        std::signal(SIGTERM, sighandler);
    }

    MainWindow w(rf);
    w.show();

    return a.exec();
}
