/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
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
#include "yarp/os/Stamp.h"
#include "include/worker.h"
#include "include/mainwindow.h"
#include "include/log.h"

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
        Bottle tmp;
        if (utilities->withExtraColumn){
            tmp = utilities->partDetails[part].bot.get(frame).asList()->tail().tail().tail();
        } else {
            tmp = utilities->partDetails[part].bot.get(frame).asList()->tail().tail();
        }

        if (strcmp (utilities->partDetails[part].type.c_str(),"Bottle") == 0){

            Bottle& outBot = utilities->partDetails[part].bottlePort.prepare();
            outBot = tmp;

            //propagate timestamp
            Stamp ts(frame,utilities->partDetails[part].timestamp[frame]);
            utilities->partDetails[part].bottlePort.setEnvelope(ts);

            if (utilities->sendStrict){
                utilities->partDetails[part].bottlePort.writeStrict();
            } else {
                utilities->partDetails[part].bottlePort.write();
            }
        }
        if (strcmp (utilities->partDetails[part].type.c_str(),"Image:ppm") == 0 || strcmp (utilities->partDetails[part].type.c_str(),"Image") == 0){
            sendImages(part, frame);
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
    IplImage* img_ipl = nullptr;
    if (code==VOCAB_PIXEL_MONO_FLOAT) {
        img_yarp = unique_ptr<Image>(new ImageOf<PixelFloat>);
        fileValid = read(*static_cast<ImageOf<PixelFloat>*>(img_yarp.get()),tmpPath);
    }
    else {
        img_ipl=cvLoadImage(tmpPath.c_str(),CV_LOAD_IMAGE_UNCHANGED);
        if ( img_ipl!=nullptr ) {
            if (code==VOCAB_PIXEL_RGB)
            {
                img_yarp = unique_ptr<Image>(new ImageOf<PixelRgb>);
                cvCvtColor(img_ipl,img_ipl,CV_BGR2RGB);
            }
            else if (code==VOCAB_PIXEL_BGR)
                img_yarp = unique_ptr<Image>(new ImageOf<PixelBgr>);
            else if (code==VOCAB_PIXEL_RGBA)
            {
                img_yarp = unique_ptr<Image>(new ImageOf<PixelRgba>);
                cvCvtColor(img_ipl,img_ipl,CV_BGRA2RGBA);
            }
            else if (code==VOCAB_PIXEL_MONO)
                img_yarp = unique_ptr<Image>(new ImageOf<PixelMono>);
            else
            {
                img_yarp = unique_ptr<Image>(new ImageOf<PixelRgb>);
                cvCvtColor(img_ipl,img_ipl,CV_BGR2RGB);
            }
            img_yarp->resize(img_ipl->width, img_ipl->height);
            cvCopy( img_ipl, (IplImage *) img_yarp->getIplImage());
            cvReleaseImage(&img_ipl);
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
        utilities->partDetails[part].imagePort.prepare()=*img_yarp;

        Stamp ts(frame,utilities->partDetails[part].timestamp[frame]);
        utilities->partDetails[part].imagePort.setEnvelope(ts);

        if (utilities->sendStrict) {
            utilities->partDetails[part].imagePort.writeStrict();
        } else {
            utilities->partDetails[part].imagePort.write();
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
    
    return true;
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
    int static tmp = 0;
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
                    LOG("partID: %d has finished \n",i);
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
    
    this->setPeriod( (2 / utilities->speed) / 1000.0 );
    for (int i=0; i < numPart; i++){
       // virtualTime += utilities->partDetails[i].worker->getFrameRate()/4.16;//0.0024;
    }
    
    virtualTime += 0.0024;//utilities->partDetails[i].worker->getFrameRate()/4.16;//0.0024;
    
    initTime++;
    
    tmp++;
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
        selectedFrame = (int)( (utilities->partDetails[i].maxFrame * steps) /100 );
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
    this->suspend();
}

/**********************************************************/
void MasterThread::resume()
{
    PeriodicThread::resume();
}

/**********************************************************/
void MasterThread::threadRelease()
{

}




