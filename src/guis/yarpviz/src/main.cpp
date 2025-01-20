/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "MainWindow.h"
#include <QApplication>
#include <yarp/os/Network.h>

int main(int argc, char *argv[])
{
    yarp::os::Network yarp;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    //w.drawGraph();

    return a.exec();
}
