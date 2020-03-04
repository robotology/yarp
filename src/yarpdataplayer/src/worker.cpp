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
#include "include/log.h"
#if CV_MAJOR_VERSION >= 3
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
WorkerClass::WorkerClass(int part, int numThreads) :
    utilities(nullptr),
    part(part),
    percentage(0),
    numThreads(numThreads),
    currFrame(0),
    isActive(true),
    frameRate(0.0),
    initTime(0.0),
    virtualTime(0.0),
    startTime(0.0)
{}

/**********************************************************/
bool WorkerClass::init()
{
    utilities->partDetails[part].sent = 0;
    initTime = 0.0;
    frameRate = 0.0;
    utilities->partDetails[part].hasNotified = false;

    #ifdef HAS_OPENCV
        yInfo("USING OPENCV FOR SENDING THE IMAGES\n");
    #else
        yInfo("USING YARP IMAGE FOR SENDING THE IMAGES\n");
    #endif

    return true;
}

/**********************************************************/
void WorkerClass::release()
{

}

/**********************************************************/
void WorkerClass::resetTime()
{
    initTime = 0.0;
    virtualTime = 0.0;
}

/**********************************************************/
void WorkerClass::sendData(int frame, bool isActive, double virtualTime)
{
    this->virtualTime = virtualTime;
    this->isActive = isActive;
    currFrame = frame;
    this->run();
}

/**********************************************************/
void WorkerClass::run()
{
    int frame = currFrame;
    if (initTime == 0 ){
        initTime = Time::now();
        startTime = yarp::os::Time::now();
    } else {
        double t = Time::now();
        frameRate = t-initTime;

        //LOG("initTime %lf t= %lf frameRate %lf\n", initTime, t, frameRate);
        initTime = t;
    }

    if (isActive)
    {
        int ret=-1;
        if (strcmp (utilities->partDetails[part].type.c_str(),"Image:ppm") == 0 ||
            strcmp (utilities->partDetails[part].type.c_str(),"Image") == 0)  {
            ret = sendImages(part, frame);
        }
        else if (strcmp(utilities->partDetails[part].type.c_str(), "Bottle") == 0)  {
            ret = sendBottle(part, frame);
            // the above line can be safely replaced with sendGenericData<Bottle>.
            // I kept it for no particular reason, thinking that maybe it could be convenient (later)
            // to process Bottles in a different way.
        }
        else if (strcmp(utilities->partDetails[part].type.c_str(), "sensor_msgs/LaserScan") == 0)  {
            ret = sendGenericData<yarp::rosmsg::sensor_msgs::LaserScan>(part, frame);
        }
        else if (strcmp(utilities->partDetails[part].type.c_str(), "nav_msgs/Odometry") == 0) {
            ret = sendGenericData<yarp::rosmsg::nav_msgs::Odometry>(part, frame);
        }
        else if (strcmp(utilities->partDetails[part].type.c_str(), "tf2_msgs/tf") == 0) {
            ret = sendGenericData<yarp::rosmsg::tf2_msgs::TFMessage>(part, frame);
        }
        else if (strcmp(utilities->partDetails[part].type.c_str(), "geometry_msgs/Pose") == 0) {
            ret = sendGenericData<yarp::rosmsg::geometry_msgs::Pose>(part, frame);
        }
        else if (strcmp(utilities->partDetails[part].type.c_str(), "geometry_msgs/Pose2D") == 0) {
            ret = sendGenericData<yarp::rosmsg::geometry_msgs::Pose2D>(part, frame);
        }
        else  {
            LOG("Unknown data type: %s", utilities->partDetails[part].type.c_str());
        }

        if (ret==-1)  {
            LOG("Failed to send data: %s", utilities->partDetails[part].type.c_str());
        }
    }
    utilities->partDetails[part].sent++;
}

/**********************************************************/
double WorkerClass::getFrameRate()
{
    //LOG("FRAME RATE %lf\n", frameRate );
    frameRate = frameRate*1000;
    return frameRate;
}

/**********************************************************/
double WorkerClass::getTimeTaken()
{
    return yarp::os::Time::now()-startTime;
}

/**********************************************************/
int WorkerClass::sendBottle(int part, int frame)
{
    Bottle tmp;
    if (utilities->withExtraColumn) {
        tmp = utilities->partDetails[part].bot.get(frame).asList()->tail().tail().tail();
    }
    else {
        tmp = utilities->partDetails[part].bot.get(frame).asList()->tail().tail();
    }

    yarp::os::BufferedPort<Bottle>* the_port = dynamic_cast<yarp::os::BufferedPort<yarp::os::Bottle>*> (utilities->partDetails[part].outputPort);
    if (the_port == nullptr)
    {  LOG_ERROR("dynamic_cast failed"); return -1;}

    Bottle& outBot = the_port->prepare();
    outBot = tmp;

    //propagate timestamp
    Stamp ts(frame, utilities->partDetails[part].timestamp[frame]);
    the_port->setEnvelope(ts);

    if (utilities->sendStrict) {
        the_port->writeStrict();
    }
    else {
        the_port->write();
    }
    return 0;
}

/**********************************************************/
int WorkerClass::sendImages(int part, int frame)
{
    string tmpPath = utilities->partDetails[part].path;
    string tmpName, tmp;
    bool fileValid = false;
    if (utilities->withExtraColumn) {
        tmpName = utilities->partDetails[part].bot.get(frame).asList()->tail().tail().get(1).asString();
        tmp = utilities->partDetails[part].bot.get(frame).asList()->tail().tail().tail().tail().toString();
    } else {
        tmpName = utilities->partDetails[part].bot.get(frame).asList()->tail().tail().get(0).asString();
        tmp = utilities->partDetails[part].bot.get(frame).asList()->tail().tail().tail().toString();
    }

    int code = 0;
    if (tmp.size()>0) {
        tmp.erase(tmp.begin());
        tmp.erase(tmp.end()-1);
        code = Vocab::encode(tmp);
    }

    tmpPath = tmpPath + tmpName;
    unique_ptr<Image> img_yarp = nullptr;

#ifdef HAS_OPENCV
    cv::Mat cv_img;
    if (code==VOCAB_PIXEL_MONO_FLOAT) {
        img_yarp = unique_ptr<Image>(new ImageOf<PixelFloat>);
        fileValid = read(*static_cast<ImageOf<PixelFloat>*>(img_yarp.get()),tmpPath);
    }
    else {
#if CV_MAJOR_VERSION >= 3
        cv_img = cv::imread(tmpPath, cv::ImreadModes::IMREAD_UNCHANGED);
#else
        cv_img = cv::imread(tmpPath, CV_LOAD_IMAGE_UNCHANGED);
#endif
        if ( cv_img.data != nullptr ) {
            if (code==VOCAB_PIXEL_RGB)
            {
                img_yarp = unique_ptr<Image>(new ImageOf<PixelRgb>);
                *img_yarp = yarp::cv::fromCvMat<PixelRgb>(cv_img);
            }
            else if (code==VOCAB_PIXEL_BGR)
            {
                img_yarp = unique_ptr<Image>(new ImageOf<PixelBgr>);
                *img_yarp = yarp::cv::fromCvMat<PixelBgr>(cv_img);
            }
            else if (code==VOCAB_PIXEL_RGBA)
            {
                img_yarp = unique_ptr<Image>(new ImageOf<PixelRgba>);
                *img_yarp = yarp::cv::fromCvMat<PixelRgba>(cv_img);
            }
            else if (code==VOCAB_PIXEL_MONO){
                img_yarp = unique_ptr<Image>(new ImageOf<PixelMono>);
                *img_yarp = yarp::cv::fromCvMat<PixelMono>(cv_img);
            }
            else
            {
                img_yarp = unique_ptr<Image>(new ImageOf<PixelRgb>);
                *img_yarp = yarp::cv::fromCvMat<PixelRgb>(cv_img);
            }
            fileValid = true;
        }
    }
#else
    if (code==VOCAB_PIXEL_RGB) {
        img_yarp = unique_ptr<Image>(new ImageOf<PixelRgb>);
        fileValid = read(*static_cast<ImageOf<PixelRgb>*>(img_yarp.get()),tmpPath.c_str());
    } else if (code==VOCAB_PIXEL_BGR) {
        img_yarp = unique_ptr<Image>(new ImageOf<PixelBgr>);
        fileValid = read(*static_cast<ImageOf<PixelBgr>*>(img_yarp.get()),tmpPath.c_str());
    } else if (code==VOCAB_PIXEL_RGBA) {
        img_yarp = unique_ptr<Image>(new ImageOf<PixelRgba>);
        fileValid = read(*static_cast<ImageOf<PixelRgba>*>(img_yarp.get()),tmpPath.c_str());
    } else if (code==VOCAB_PIXEL_MONO_FLOAT) {
        img_yarp = unique_ptr<Image>(new ImageOf<PixelFloat>);
        fileValid = read(*static_cast<ImageOf<PixelFloat>*>(img_yarp.get()),tmpPath.c_str());
    } else if (code==VOCAB_PIXEL_MONO) {
        img_yarp = unique_ptr<Image>(new ImageOf<PixelMono>);
        fileValid = read(*static_cast<ImageOf<PixelMono>*>(img_yarp.get()),tmpPath.c_str());
    } else {
        img_yarp = unique_ptr<Image>(new ImageOf<PixelRgb>);
        fileValid = read(*static_cast<ImageOf<PixelRgb>*>(img_yarp.get()),tmpPath.c_str());
    }
#endif
    if (!fileValid) {
        LOG_ERROR("Cannot load file %s !\n", tmpPath.c_str() );
        return 1;
    }
    else
    {
        yarp::os::BufferedPort<yarp::sig::Image>* the_port = dynamic_cast<yarp::os::BufferedPort<yarp::sig::Image>*> (utilities->partDetails[part].outputPort);
        if (the_port == nullptr) { yFatal() << "dynamic_cast failed"; }

        the_port->prepare()=*img_yarp;

        Stamp ts(frame,utilities->partDetails[part].timestamp[frame]);
        the_port->setEnvelope(ts);

        if (utilities->sendStrict) {
            the_port->writeStrict();
        } else {
            the_port->write();
        }
    }

    return 0;
}
/**********************************************************/
void WorkerClass::setManager(Utilities *utilities)
{
    this->utilities= utilities;
}

/**********************************************************/
MasterThread::MasterThread(Utilities *utilities, int numPart, QMainWindow *gui, QObject *parent) :
    QObject(parent),
    PeriodicThread (0.002),
    utilities(utilities),
    numPart(numPart),
    numThreads(0),
    timePassed(0.0),
    initTime(0),
    virtualTime(0.0),
    stepfromCmd(false),
    wnd(gui)
{}

/**********************************************************/
bool MasterThread::threadInit()
{
    initTime = 0;

    for (int i =0; i < numPart; i++){
        utilities->partDetails[i].currFrame = 0;
    }

    //virtualTime = utilities->partDetails[0].timestamp[ utilities->partDetails[0].currFrame ];
    virtualTime = utilities->minTimeStamp;

    LOG("virtual time is %lf\n", virtualTime);

    initialize();

    return true;
}

/**********************************************************/
void MasterThread::initialize()
{
    lastUpdate = std::chrono::high_resolution_clock::now();
    dtSeconds = 0.f;
    fps = 0.f;
}

/**********************************************************/
void MasterThread::tick()
{
    Moment now = std::chrono::high_resolution_clock::now();

    const unsigned diff = std::chrono::duration_cast<std::chrono::microseconds>(now - lastUpdate).count();
    lastUpdate = now;

    dtSeconds = diff / (1000.f * 1000.f); // micro-seconds -> seconds
    fps = 1.f / dtSeconds;
}

/**********************************************************/
void MasterThread::stepFromCmd()
{
    //fprintf( stdout, "in step from command\n");
    bool sentAll = false;
    for (int i=0; i < numPart; i++){
        utilities->partDetails[i].sent = 0;
    }
    utilities->totalSent = 0;

    while ( !sentAll ){
        runNormally();
        for (int i=0; i < numPart; i++){
            if (utilities->partDetails[i].sent == 1){
                utilities->totalSent ++;
                utilities->partDetails[i].sent = 2;
            }
            if (utilities->totalSent == numPart){
                sentAll = true;
            }
        }
    }

    for (int i=0; i < numPart; i++){
        utilities->partDetails[i].sent = 0;
    }

    utilities->totalSent = 0;
    utilities->pauseThread();
}

/**********************************************************/
void MasterThread::runNormally()
{

    for (int i=0; i < numPart; i++){
        bool isActive  = ((MainWindow*)wnd)->getPartActivation(utilities->partDetails[i].name.c_str());
        if ( utilities->partDetails[i].currFrame <= utilities->partDetails[i].maxFrame ){
            if ( virtualTime >= utilities->partDetails[i].timestamp[ utilities->partDetails[i].currFrame ] ){
                if ( initTime > 300){
                    emit utilities->updateGuiThread();
                    initTime = 0;
                }
                if (!utilities->partDetails[i].hasNotified){
                    utilities->partDetails[i].worker->sendData(utilities->partDetails[i].currFrame, isActive, virtualTime );
                    utilities->partDetails[i].currFrame++;
                }
            }
        } else {
            if (utilities->repeat) {
                this->threadInit();
                utilities->partDetails[i].worker->init();
            } else {
                if ( !utilities->partDetails[i].hasNotified ) {
                    LOG("partID: %d has finished \n", i);
                    utilities->partDetails[i].hasNotified = true;
                }

                int stopAll = 0;
                for (int x=0; x < numPart; x++){
                    if (utilities->partDetails[x].hasNotified){
                        stopAll++;
                    }

                    if (stopAll == numPart){
                        LOG("All parts have Finished!\n");
                        if (utilities->partDetails[i].currFrame > 1)
                            emit utilities->updateGuiThread();
                        utilities->stopAtEnd();
                    }
                }
            }
        }
    }

    virtualTime += diff_seconds() * utilities->speed;

    tick();

    initTime++;
}

/**********************************************************/
void MasterThread::goToPercentage(int value)
{
    for (int i=0; i < numPart; i++){
        int maxFrame = utilities->partDetails[i].maxFrame;
        int currFrame = (value * maxFrame) / 100;
        utilities->partDetails[i].currFrame = currFrame;
    }
    virtualTime = utilities->partDetails[0].timestamp[ utilities->partDetails[0].currFrame ];
}

/**********************************************************/
void MasterThread::run()
{
    if (stepfromCmd){
        stepFromCmd();
    } else {
        runNormally();
    }
}

/**********************************************************/
void MasterThread::forward(int steps)
{
    int selectedFrame = 0;
    for (int i=0; i < numPart; i++){
        selectedFrame = (int)( (utilities->partDetails[i].maxFrame * steps) / 100 );
        if ( utilities->partDetails[i].currFrame < utilities->partDetails[i].maxFrame - selectedFrame){
            utilities->partDetails[i].currFrame += selectedFrame;
            if (i == 0){
                virtualTime = utilities->partDetails[i].timestamp[utilities->partDetails[i].currFrame];
            }
        } else {
            LOG( "cannot go any forward, out of range\n");
        }
    }
}

/**********************************************************/
void MasterThread::backward(int steps)
{
    int selectedFrame = 0;
    for (int i=0; i < numPart; i++){
        selectedFrame = (int)( (utilities->partDetails[i].maxFrame * steps) /100 );
        if ( utilities->partDetails[i].currFrame > selectedFrame){
            utilities->partDetails[i].currFrame -= selectedFrame;
            if (i == 0){
                virtualTime = utilities->partDetails[i].timestamp[utilities->partDetails[i].currFrame];
            }
        } else {
            LOG( "cannot go any backwards, out of range..\n");
        }
    }
}

/**********************************************************/
void MasterThread::pause()
{
    pauseStart = yarp::os::Time::now();
    this->suspend();
}

/**********************************************************/
void MasterThread::resume()
{
    pauseEnd = yarp::os::Time::now();
    virtualTime -= pauseEnd - pauseStart;
    PeriodicThread::resume();
}

/**********************************************************/
void MasterThread::threadRelease()
{

}
