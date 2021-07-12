/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#if defined(_WIN32)
    #pragma warning (disable : 4099)
    #pragma warning (disable : 4250)
    #pragma warning (disable : 4520)
#endif

#include <dirent.h>

#if defined(_WIN32)
    #undef max                  /*conflict with pango lib coverage.h*/
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #include <cerrno>
    #include <sys/stat.h>
    #define GetCurrentDir getcwd
#endif

#include <yarp/os/Time.h>
#include <cstdio>              /* defines FILENAME_MAX */
#include "include/utils.h"

#include <iostream>
#include <utility>
#include "include/mainwindow.h"

 //ROS messages
#include <yarp/rosmsg/sensor_msgs/LaserScan.h>
#include <yarp/rosmsg/nav_msgs/Odometry.h>
#include <yarp/rosmsg/tf/tfMessage.h>
#include <yarp/rosmsg/tf2_msgs/TFMessage.h>
#include <yarp/rosmsg/geometry_msgs/Pose.h>
#include <yarp/rosmsg/geometry_msgs/Pose2D.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace std;

/**********************************************************/
QUtilities::~QUtilities()
{
}
/**********************************************************/
QUtilities::QUtilities(QObject *parent) : QObject(parent), qengine(nullptr)
{
    connect(this,SIGNAL(updateGuiThread()),(MainWindow*)parent,
            SLOT(onUpdateGuiRateThread()),Qt::BlockingQueuedConnection);

    connect(this,SIGNAL(pause()),(MainWindow*)parent,
            SLOT(resetButtonOnStop()),Qt::BlockingQueuedConnection);

    connect(this,SIGNAL(errorMessage(QString)),(MainWindow*)parent,
            SLOT(onErrorMessage(QString)),Qt::QueuedConnection);
}

/**********************************************************/
void QUtilities::stepThread()
{
    this->qengine->stepfromCmd = true;
    yInfo() << "asking qutils to step the thread";
    if ( this->qengine->isRunning() ){
        yInfo() << "asking qutils to pause the thread";
        this->qengine->pause();
    }
    if ( this->qengine->isSuspended() ){
        yInfo() << "asking qutils to resume the thread";
        this->qengine->resume();
    } else if ( !this->qengine->isRunning() ) {
        yInfo() << "asking qutils to start the thread";
        for (int i=0; i < totalThreads; i++){
            this->partDetails[i].worker->init();
        }
        this->qengine->start();
    }
    yInfo() << "ok................ \n";

}

/**********************************************************/
void QUtilities::resetButton()
{
    pause();
}
