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

#include <yarp/sig/Image.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/RateThread.h>
#include "iCub/utils.h"
#include <cv.h>
#include <highgui.h>
#include <yarp/os/Event.h>
#include <yarp/os/Time.h>

#ifndef __WORKER_H__
#define __WORKER_H__

class Utilities;
class MainWindow;

/**********************************************************/
class WorkerClass
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
class UpdateGui
{
protected:
    Utilities *utilities;
    MainWindow* wnd;
    int numPart;
    int percentage;
public:
    UpdateGui(Utilities *utilities, int numPart, MainWindow *gui);
    void run();
    void updateGuiRateThread();
    bool threadInit();
    void threadRelease();
};
/**********************************************************/
class MasterThread : public yarp::os::RateThread
{
protected:
    Utilities *utilities;
    UpdateGui *guiUpdate;
    int numPart;

public:
    int                     numThreads;
    double                  timePassed, initTime, virtualTime;
    bool                    stepfromCmd;
    MainWindow* wnd;

    MasterThread(Utilities *utilities, int numPart, MainWindow* gui);
    bool threadInit();
    void threadRelease();
    void forward(int steps);
    void backward(int steps);
    void pause();
    void resume();
    void run();
    void stepFromCmd();
    void runNormally();
};


#endif
