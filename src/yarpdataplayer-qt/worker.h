/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Vadim Tikhanoff
 * email:  vadim.tikhanoff@iit.it
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
#include <QObject>

#include <yarp/sig/Image.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/RateThread.h>
#include <yarp/sig/ImageFile.h>
#include "utils.h"
#include <yarp/os/Event.h>
#include <yarp/os/Time.h>
#include <QMainWindow>

#ifndef __WORKER_H__
#define __WORKER_H__

class Utilities;
//class MainWindow;

/**********************************************************/
class WorkerClass : public QObject
{
protected:
    Utilities *utilities;

    void run();
    int part;
    int percentage;
    int numThreads;
    int currFrame;
    bool isActive;
    double frameRate, initTime, virtualTime;
    yarp::os::Semaphore semIndex;

public:
    /**
    * Worker class that does the work of sending the data for each part
    */
    WorkerClass(int part, int numThread);
    /**
    * Function that sets the manager to utilities class
    */
    void setManager(Utilities *utilities);
    /**
    * Function that sends the images
    */
    int sendImages( int part, int id );
    /**
    * Function that returns the frame rate
    */
    double getFrameRate();
    /**
    * Function that pepares and "steps" the sending of the data
    */
    void sendData(int id, bool shouldSend, double virtualTime);
    /**
    * init
    */
    bool init();
    /**
    * release
    */
    void release();
    /**
    * Function that resets the time
    */
    void resetTime();



};
/**********************************************************/
//class UpdateGui : public QObject
//{
//protected:
//    Utilities *utilities;
//    QMainWindow* wnd;
//    int numPart;
//    int percentage;
//public:
//    UpdateGui(Utilities *utilities, int numPart, QMainWindow *gui );
//    void run();
//    //void updateGuiRateThread();
//    bool threadInit();
//    void threadRelease();


//};
/**********************************************************/
class MasterThread : public QObject,  public yarp::os::RateThread
{

    friend class Utilities;

protected:
    Utilities *utilities;
    //UpdateGui *guiUpdate;
    int numPart;

public:
    int                     numThreads;
    double                  timePassed, initTime, virtualTime;
    bool                    stepfromCmd;
    QMainWindow* wnd;

    /**
     * Master thread class
     */
    MasterThread(Utilities *utilities, int numPart, QMainWindow *gui, QObject *parent = NULL);
    /**
     * Thread init
     */
    bool threadInit();
    /**
     * Thread release
     */
    void threadRelease();
    /**
     * Function that steps forwards the data set
     */
    void forward(int steps);
    /**
     * Function that steps backwards the data set
     */
    void backward(int steps);
    /**
     * Function that pauses data set
     */
    void pause();
    /**
     * Function that resumes the data set
     */
    void resume();
    /**
     * Run function
     */
    void run();
    /**
     * Function that steps from command rpc
     */
    void stepFromCmd();
    /**
     * Function that steps normally (without using terminal or rpc)
     */
    void runNormally();

    void goToPercentage(int value);


};


#endif
