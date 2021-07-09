/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
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
QEngine::QEngine(QUtilities *qutilities, int subDirCnt, QMainWindow *gui, QObject *parent) :
    QObject(parent),
    qutils(qutilities),
    gui(gui)
{
    this->numPart = subDirCnt;
    thread = new QMasterThread();
    this->setThread(thread);
    thread->setEngine(*this);
    this->setUtilities(this->qutils);
}

/**********************************************************/
QEngine::~QEngine()
{
    delete thread;
}

/**********************************************************/
void QEngine::stepFromCmd()
{
    yInfo() << "in step from command";
    bool sentAll = false;
    for (int i=0; i < this->numPart; i++){
        qutils->partDetails[i].sent = 0;
    }
    qutils->totalSent = 0;

    while ( !sentAll ){
        runNormally();
        for (int i=0; i < this->numPart; i++){
            if (qutils->partDetails[i].sent == 1){
                qutils->totalSent ++;
                qutils->partDetails[i].sent = 2;
            }
            if (qutils->totalSent == this->numPart){
                sentAll = true;
            }
        }
    }

    for (int i=0; i < this->numPart; i++){
        qutils->partDetails[i].sent = 0;
    }

    qutils->totalSent = 0;
    qutils->qengine->pause();
    for (int i=0; i < this->numPart; i++){
        qutils->qengine->virtualTime = qutils->partDetails[i].timestamp[qutils->partDetails[i].currFrame];
    }
    qutils->qengine->virtualTime = qutils->partDetails[0].timestamp[qutils->partDetails[0].currFrame];
    yInfo() << "ok................ ";
    qutils->qengine->stepfromCmd = false;
}

/**********************************************************/
void QEngine::runNormally()
{
    for (int i=0; i < this->numPart; i++){
        bool isActive = ((MainWindow*)gui)->getPartActivation(qutils->partDetails[i].name.c_str());
        if ( qutils->partDetails[i].currFrame <= qutils->partDetails[i].maxFrame ){
            if ( this->virtualTime >= qutils->partDetails[i].timestamp[ qutils->partDetails[i].currFrame ] ){
                if ( this->initTime > 300 && this->virtualTime < qutils->partDetails[i].timestamp[qutils->partDetails[i].timestamp.length()-1]){
                    emit qutils->updateGuiThread();
                    this->initTime = 0;
                }
                if (!qutils->partDetails[i].hasNotified){
                    qutils->partDetails[i].worker->sendData(qutils->partDetails[i].currFrame, isActive, this->virtualTime );
                    qutils->partDetails[i].currFrame++;
                }
            }
        } else {
            if (qutils->repeat) {
                this->initThread();
                qutils->partDetails[i].worker->init();
            } else {
                if ( !qutils->partDetails[i].hasNotified ) {
                    yInfo() << "partID: " <<  i << " has finished";
                    qutils->partDetails[i].hasNotified = true;
                }

                int stopAll = 0;
                for (int x=0; x < this->numPart; x++){
                    if (qutils->partDetails[x].hasNotified){
                        stopAll++;
                    }

                    if (stopAll == this->numPart){
                        yInfo() << "All parts have Finished!";
                        if (qutils->partDetails[i].currFrame > 1)
                            emit qutils->updateGuiThread();
                        qutils->stopAtEnd();
                        qutils->resetButton();
                        allPartsStatus = true;
                    }
                }
            }
        }
    }

    this->virtualTime += this->diff_seconds() * qutils->speed;
    this->tick();
    this->initTime++;
}

/**********************************************************/
void QEngine::QMasterThread::run()
{
    if (this->qEngine->stepfromCmd){
        this->qEngine->stepFromCmd();
    } else {
        this->qEngine->runNormally();
    }
}
