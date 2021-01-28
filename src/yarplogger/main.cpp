/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
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

    yarp::os::ResourceFinder &rf = yarp::os::ResourceFinder::getResourceFinderSingleton();
    rf.setDefaultConfigFile("yarprunLogger.ini");           //overridden by --from parameter
    rf.setDefaultContext("yarprunLogger");                  //overridden by --context parameter
    rf.configure(argc,argv);

    QApplication a(argc, argv); // Eats the "--name" argument, therefore initialized after the ResourceFinder

    bool cannot_close = rf.check("no_stop");
    if (cannot_close)
    {
        std::signal(SIGINT, sighandler);
        std::signal(SIGTERM, sighandler);
    }

    MainWindow w;
    w.show();

    return a.exec();
}
