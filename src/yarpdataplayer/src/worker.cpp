/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
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

#include <memory>
#include <yarp/os/LogStream.h>
#include "include/worker.h"
#include "include/mainwindow.h"

#ifdef HAS_OPENCV
#include <opencv2/imgcodecs/imgcodecs.hpp>
#endif

//ROS messages
#include <yarp/rosmsg/sensor_msgs/LaserScan.h>
#include <yarp/rosmsg/nav_msgs/Odometry.h>
#include <yarp/rosmsg/tf/tfMessage.h>
#include <yarp/rosmsg/tf2_msgs/TFMessage.h>
#include <yarp/rosmsg/geometry_msgs/Pose.h>
#include <yarp/rosmsg/geometry_msgs/Pose2D.h>


using namespace yarp::sig;
using namespace yarp::sig::file;
using namespace yarp::os;
using namespace std;

#ifdef HAS_OPENCV
  using namespace cv;
#endif

/**********************************************************/
MasterThread::MasterThread(QUtilities *qutilities, int numPart, QMainWindow *gui, QObject *parent) :
    QObject(parent),
    wnd(gui)
{
    qutilities->utils->dataplayerEngine->setNumPart(numPart);
}

/**********************************************************/
bool MasterThread::init()
{
    return qutilities->utils->dataplayerEngine->initThread();
}

/**********************************************************/
void MasterThread::tick()
{
    qutilities->utils->dataplayerEngine->tick();
}

/**********************************************************/
void MasterThread::stepFromCmd()
{
    //fprintf( stdout, "in step from command\n");
    bool sentAll = false;
    for (int i=0; i < qutilities->utils->dataplayerEngine->numPart; i++){
        qutilities->utils->partDetails[i].sent = 0;
    }
    qutilities->utils->totalSent = 0;

    while ( !sentAll ){
        runNormally();
        for (int i=0; i < qutilities->utils->dataplayerEngine->numPart; i++){
            if (qutilities->utils->partDetails[i].sent == 1){
                qutilities->utils->totalSent ++;
                qutilities->utils->partDetails[i].sent = 2;
            }
            if (qutilities->utils->totalSent == qutilities->utils->dataplayerEngine->numPart){
                sentAll = true;
            }
        }
    }

    for (int i=0; i < qutilities->utils->dataplayerEngine->numPart; i++){
        qutilities->utils->partDetails[i].sent = 0;
    }

    qutilities->utils->totalSent = 0;
    qutilities->utils->pauseThread();
}

/**********************************************************/
void MasterThread::runNormally()
{
    for (int i=0; i < qutilities->utils->dataplayerEngine->numPart; i++){
        bool isActive  = ((MainWindow*)wnd)->getPartActivation(qutilities->utils->partDetails[i].name.c_str());
        if ( qutilities->utils->partDetails[i].currFrame <= qutilities->utils->partDetails[i].maxFrame ){
            if ( qutilities->utils->dataplayerEngine->virtualTime >= qutilities->utils->partDetails[i].timestamp[ qutilities->utils->partDetails[i].currFrame ] ){
                if ( qutilities->utils->dataplayerEngine->initTime > 300){
                    emit qutilities->updateGuiThread();
                    qutilities->utils->dataplayerEngine->initTime = 0;
                }
                if (!qutilities->utils->partDetails[i].hasNotified){
                    qutilities->utils->partDetails[i].worker->sendData(qutilities->utils->partDetails[i].currFrame, isActive, qutilities->utils->dataplayerEngine->virtualTime );
                    qutilities->utils->partDetails[i].currFrame++;
                }
            }
        } else {
            if (qutilities->utils->repeat) {
                qutilities->utils->dataplayerEngine->initThread();
                qutilities->utils->partDetails[i].worker->init();
            } else {
                if ( !qutilities->utils->partDetails[i].hasNotified ) {
                    yInfo() << "partID: " <<  i << " has finished";
                    qutilities->utils->partDetails[i].hasNotified = true;
                }

                int stopAll = 0;
                for (int x=0; x < qutilities->utils->dataplayerEngine->numPart; x++){
                    if (qutilities->utils->partDetails[x].hasNotified){
                        stopAll++;
                    }

                    if (stopAll == qutilities->utils->dataplayerEngine->numPart){
                        yInfo() << "All parts have Finished!";
                        if (qutilities->utils->partDetails[i].currFrame > 1)
                            emit qutilities->updateGuiThread();
                        qutilities->utils->stopAtEnd();
                    }
                }
            }
        }
    }

    qutilities->utils->dataplayerEngine->virtualTime += qutilities->utils->dataplayerEngine->diff_seconds() * qutilities->utils->speed;

    tick();

    qutilities->utils->dataplayerEngine->initTime++;
}

/**********************************************************/
void MasterThread::goToPercentage(int value)
{
    qutilities->utils->dataplayerEngine->goToPercentage(value);
}

/**********************************************************/
void MasterThread::run()
{
    if (qutilities->utils->dataplayerEngine->stepfromCmd){
        stepFromCmd();
    } else {
        runNormally();
    }
}

/**********************************************************/
void MasterThread::forward(int steps)
{
    qutilities->utils->dataplayerEngine->forward(steps);
}

/**********************************************************/
void MasterThread::backward(int steps)
{
    qutilities->utils->dataplayerEngine->backward(steps);
}

/**********************************************************/
void MasterThread::pause()
{
    qutilities->utils->dataplayerEngine->pause();
}

/**********************************************************/
void MasterThread::resume()
{
    qutilities->utils->dataplayerEngine->resume();
}

/**********************************************************/
void MasterThread::release()
{
    qutilities->utils->dataplayerEngine->releaseThread();
}
