/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
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
        fprintf(stderr,"ERROR: check YARP network.\n");
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
