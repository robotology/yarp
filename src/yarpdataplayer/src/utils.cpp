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
