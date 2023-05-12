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
    if(allPartsStatus) return;

    for (int i=0; i < this->numPart; i++){
        //get a reference to the part we are interested in
        yarp::yarpDataplayer::PartsData &this_part = qutils->partDetails[i];

        //if we have alredy stopped we have nothing to do
        if(this_part.hasNotified)
            continue;

        //if this port is not active, keep progressing though the frames without
        //sending, so if the part activates it is in synch
        bool isActive = ((MainWindow*)gui)->getPartActivation(this_part.name.c_str());

        //send all available frames up to the current virtualTime
        while (this_part.currFrame <= this_part.maxFrame &&
            this->virtualTime >= this_part.timestamp[this_part.currFrame]) {
            this_part.worker->sendData(this_part.currFrame++, isActive, this->virtualTime);
        }

        //if we have sent all frames perform reset/stop
        if(this_part.currFrame > this_part.maxFrame) {
            if (qutils->repeat) {
                this->initThread();
                this_part.worker->init();
            } else {
                yInfo() << "partID: " <<  i << " has finished";
                this_part.hasNotified = true;

                //perform a check to see if ALL parts have finished
                int stopAll = 0;
                for (int x=0; x < this->numPart; x++){
                    stopAll += qutils->partDetails[x].hasNotified ? 1 : 0;
                }

                if (stopAll == this->numPart){
                    yInfo() << "All parts have Finished!";
                    if (this_part.currFrame > 1  && this->isRunning()) {
                        emit qutils->updateGuiThread();
                    }
                    qutils->stopAtEnd();
                    qutils->resetButton();
                    allPartsStatus = true;
                }
            }
        }
    }

    this->virtualTime += this->diff_seconds() * qutils->speed;
    this->tick();

    //10 Hz gui update
    static double gui_tic = 0.0;
    if(this->virtualTime < gui_tic || this->virtualTime - gui_tic > 0.1 && this->isRunning()) {
         emit qutils->updateGuiThread();
         gui_tic = this->virtualTime;
    }

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
