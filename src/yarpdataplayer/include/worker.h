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

#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include "include/log.h"

#include <yarp/sig/Image.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Stamp.h>
#include <yarp/sig/ImageFile.h>
#include "include/utils.h"
#include <yarp/os/Event.h>
#include <yarp/os/Time.h>
#include <QMainWindow>

#include <chrono>

#ifdef HAS_OPENCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <yarp/cv/Cv.h>
#endif

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
    double startTime;

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
    * Functions that sends data (many different types)
    */
    int sendBottle(int part, int id);
    int sendImages( int part, int id);

    template <class T>
    int sendGenericData(int part, int id);

    /**
    * Function that returns the frame rate
    */
    double getFrameRate();
    /**
     * Function that returns the time taken
     */
    double getTimeTaken();
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
class MasterThread : public QObject,  public yarp::os::PeriodicThread
{

    friend class Utilities;

protected:
    Utilities *utilities;
    //UpdateGui *guiUpdate;
    int numPart;

public:
    int                     numThreads;
    double                  timePassed, initTime, virtualTime;
    double                  pauseStart{0.0}, pauseEnd{0.0};
    bool                    stepfromCmd;

    using Moment = std::chrono::time_point<std::chrono::high_resolution_clock>;

    void initialize();

    void tick();

    float diff_seconds() const { return dtSeconds; }
    float framesPerSecond() const { return fps; }

    QMainWindow* wnd;

    /**
     * Master thread class
     */
    MasterThread(Utilities *utilities, int numPart, QMainWindow *gui, QObject *parent = NULL);

    bool threadInit() override;
    /**
     * Thread release
     */
    void threadRelease() override;
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
    void run() override;
    /**
     * Function that steps from command rpc
     */
    void stepFromCmd();
    /**
     * Function that steps normally (without using terminal or rpc)
     */
    void runNormally();

    void goToPercentage(int value);

private:
    Moment lastUpdate;
    float dtSeconds, fps;

};


#endif
