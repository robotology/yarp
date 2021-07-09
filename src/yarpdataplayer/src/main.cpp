/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "include/mainwindow.h"
#include <QApplication>

#include <yarp/dataplayer/YarpDataplayer.h>

#if defined(_WIN32)
    #pragma warning (disable : 4099)
    #pragma warning (disable : 4250)
    #pragma warning (disable : 4520)
#endif

#include <iostream>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Network.h>
#include <QtGlobal>

#include <yarp/os/Os.h>

#if defined(_WIN32)
    #include <windows.h>
#else
    #include <cerrno>
    #include <sys/types.h>
    #include <csignal>
#endif

using namespace std;
using namespace yarp::os;

int main(int argc, char *argv[])
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#else
    qputenv("QT_DEVICE_PIXEL_RATIO", QByteArray("auto"));
#endif
    setEnergySavingModeState(false);
    QApplication a(argc, argv);

    Network yarp;
    if (!yarp.checkNetwork()){
        fprintf(stderr, "Sorry YARP network does not seem to be available, is the yarp server available?\n");
        return 1;
    }

    yarp::os::ResourceFinder rf;
    rf.setDefaultConfigFile( "config.ini" );        //overridden by --from parameter
    rf.setDefaultContext( "yarpdataplayer" );        //overridden by --context parameter
    rf.configure( argc, argv );

    MainWindow w(rf);

    if (rf.check("hidden")){
        w.hide();
    }else{
        w.show();
    }

    int returnFlag = (a.exec()!=0?1:0);
    setEnergySavingModeState(true);
    return returnFlag;
}
