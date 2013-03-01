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

#if defined(WIN32)
    #pragma warning (disable : 4099)
    #pragma warning (disable : 4250)
    #pragma warning (disable : 4520)
#endif

#include "yarp/os/Stamp.h"
#include "iCub/worker.h"
#include "iCub/main_window.h"

using namespace yarp::sig;
using namespace yarp::os;
using namespace cv;

/**********************************************************/
WorkerClass::WorkerClass(int part, int numThreads)
{
    this->part = part;
    utilities = NULL;
    this->numThreads = numThreads;
    initTime = 0.0;
    frameRate = 0.0;
    isActive = true;
}
/**********************************************************/
bool WorkerClass::init()
{
    utilities->partDetails[part].sent = 0;
    initTime = 0.0;
    frameRate = 0.0;
    utilities->partDetails[part].hasNotified = false;
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
    if (initTime == 0 )
    {
        initTime = Time::now();
    }
    else
    {
        double t = Time::now();
        frameRate = t-initTime;
        initTime = t;
    }
    if (isActive)
    {
        Bottle tmp = utilities->partDetails[part].bot.get(frame).asList()->tail().tail();
        if (strcmp (utilities->partDetails[part].type.c_str(),"Bottle") == 0)
        {
            
            Bottle& outBot = utilities->partDetails[part].bottlePort.prepare();
            outBot = tmp;

            //propagate timestamp
            Stamp ts(frame,utilities->partDetails[part].timestamp[frame]);
            utilities->partDetails[part].bottlePort.setEnvelope(ts);

            if (utilities->sendStrict)
                utilities->partDetails[part].bottlePort.writeStrict();
            else
                utilities->partDetails[part].bottlePort.write();
        }
        if (strcmp (utilities->partDetails[part].type.c_str(),"Image:ppm") == 0)
            sendImages(part, frame);
    }
    utilities->partDetails[part].sent++;
}
/**********************************************************/
double WorkerClass::getFrameRate()
{
    frameRate = frameRate*1000;
    return frameRate;
}
/**********************************************************/
int WorkerClass::sendImages(int part, int frame)
{
    IplImage* img = NULL;
    string tmpPath = utilities->partDetails[part].path;
    string tmpName = utilities->partDetails[part].bot.get(frame).asList()->tail().tail().get(0).asString().c_str();
    tmpPath = tmpPath + tmpName;
    img = cvLoadImage( tmpPath.c_str(), CV_LOAD_IMAGE_UNCHANGED );

    if( img == 0 )
    {
        fprintf( stderr, "Cannot load file %s!\n", tmpPath.c_str() );
        return 1;
    }else
    {
        cvCvtColor( img, img, CV_BGR2RGB );
        ImageOf<PixelRgb> &temp = utilities->partDetails[part].imagePort.prepare();
        temp.resize(img->width,img->height);
        cvCopyImage( img, (IplImage *) temp.getIplImage());

        //propagate timestamp
        Stamp ts(frame,utilities->partDetails[part].timestamp[frame]);
        utilities->partDetails[part].imagePort.setEnvelope(ts);

        if (utilities->sendStrict)
            utilities->partDetails[part].imagePort.writeStrict();
        else
            utilities->partDetails[part].imagePort.write();
        cvReleaseImage(&img);
    }
    return 0;
}
/**********************************************************/
void WorkerClass::setManager(Utilities *utilities)
{
    this->utilities= utilities;
}

/**********************************************************/
MasterThread::MasterThread( Utilities *utilities, int numPart, MainWindow *gui) : RateThread (2)
{
    //stepfromCmd = false;
    this->wnd = gui;
    this->numPart = numPart;
    this->utilities = utilities;
    guiUpdate = new UpdateGui( this->utilities, this->numPart, this->wnd );
    timePassed = 0.0;
    virtualTime = 0.0;
    initTime = 0;
}

/**********************************************************/
bool MasterThread::threadInit()
{
    initTime = 0;
    guiUpdate->threadInit();

     //if (!stepfromCmd)
     {
        for (int i =0; i < numPart; i++)
            utilities->partDetails[i].currFrame = 0;

        virtualTime = utilities->partDetails[0].timestamp[ utilities->partDetails[0].currFrame ];
     }
    return true;
}
/**********************************************************/
void MasterThread::stepFromCmd()
{
    //fprintf( stdout, "in step from command\n");
    bool sentAll = false;
    for (int i=0; i < numPart; i++)
        utilities->partDetails[i].sent = 0;
    utilities->totalSent = 0;

    while ( !sentAll )
    {
        runNormally();
        for (int i=0; i < numPart; i++)
        {
            if (utilities->partDetails[i].sent == 1)
            {
                utilities->totalSent ++;
                utilities->partDetails[i].sent = 2;
            }
            if (utilities->totalSent == numPart)
                sentAll = true;
        }
    }

    for (int i=0; i < numPart; i++)
        utilities->partDetails[i].sent = 0;
    
    utilities->totalSent = 0;
    utilities->pauseThread();
}
/**********************************************************/
void MasterThread::runNormally()
{
    for (int i=0; i < numPart; i++)
    {
        bool isActive  = wnd->getPartActivation(utilities->partDetails[i].name.c_str());
        if ( utilities->partDetails[i].currFrame < utilities->partDetails[i].maxFrame)
        {
            if ( virtualTime >= utilities->partDetails[i].timestamp[ utilities->partDetails[i].currFrame ] )
            {
                utilities->partDetails[i].worker->sendData(utilities->partDetails[i].currFrame, isActive, virtualTime );
                if ( initTime > 300)
                {
                    guiUpdate->updateGuiRateThread();
                    initTime = 0;
                }
                utilities->partDetails[i].currFrame++;
            }
        }
        else
        {
            if (utilities->repeat)
            {
                this->threadInit();
                utilities->partDetails[i].worker->init();
            }
            else
            {
                if ( !utilities->partDetails[i].hasNotified )
                {
                    fprintf(stdout, "partID: %d has finished \n",i);
                    utilities->partDetails[i].hasNotified = true;
                }
                int stopAll = 0;
                for (int x=0; x < numPart; x++)
                {
                    if (utilities->partDetails[x].hasNotified)
                        stopAll++;

                    if (stopAll == numPart)
                    {
                        //guiUpdate->updateGuiRateThread();
                        fprintf(stdout, "All parts have Finished!\n");
                        utilities->stopAtEnd();
                    }
                }
            }
        }
    }
    this->setRate( (int) (2 / utilities->speed) );
    virtualTime += 0.002; // increase by two millisecond
    //fprintf(stdout, "virtualTime: %lf \n", virtualTime);
    initTime++;
}
/**********************************************************/
void MasterThread::run()
{
    if (stepfromCmd)
    {
        stepFromCmd();
    }
    else
        runNormally();
}
/**********************************************************/
void MasterThread::forward(int steps)
{
    int selectedFrame = 0;
    for (int i=0; i < numPart; i++)
    {
        selectedFrame = (int)( (utilities->partDetails[i].maxFrame * steps) /100 );
        if ( utilities->partDetails[i].currFrame < utilities->partDetails[i].maxFrame - selectedFrame)
        {
            utilities->partDetails[i].currFrame += selectedFrame;
            if (i == 0)
                virtualTime = utilities->partDetails[i].timestamp[utilities->partDetails[i].currFrame];
        }
        else
            fprintf(stdout, "cannot go any forward, out of range..\n");
    }
}
/**********************************************************/
void MasterThread::backward(int steps)
{
    int selectedFrame = 0;
    for (int i=0; i < numPart; i++)
    {
        selectedFrame = (int)( (utilities->partDetails[i].maxFrame * steps) /100 );
        if ( utilities->partDetails[i].currFrame > selectedFrame)
        {
            utilities->partDetails[i].currFrame -= selectedFrame;
            if (i == 0)
                virtualTime = utilities->partDetails[i].timestamp[utilities->partDetails[i].currFrame];
        }
        else
            fprintf(stdout, "cannot go any backwards, out of range..\n");
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
    RateThread::resume();
}
/**********************************************************/
void MasterThread::threadRelease()
{

}
/**********************************************************/
UpdateGui::UpdateGui( Utilities *utilities, int numPart, MainWindow *gui)
{
    this->wnd = gui;
    this->utilities = utilities;
    this->numPart = numPart;
    this->percentage = 0;
}
/**********************************************************/
bool UpdateGui::threadInit()
{
    percentage = 0;
    return true;
}
/**********************************************************/
void UpdateGui::threadRelease()
{

}
/**********************************************************/
void UpdateGui::run()
{
    for (int i=0; i < numPart; i++)
    {
        if ( wnd->getPartActivation(utilities->partDetails[i].name.c_str()) )
        {
            if ( utilities->partDetails[i].bot.get(1).asList()->get(2).isString() && utilities->partDetails[i].type == "Bottle")
            {
                //avoid checking frame rate for string data
                wnd->setFrameRate(utilities->partDetails[i].name.c_str(), 0);
            }
            else
            {
                if (utilities->partDetails[i].currFrame <= utilities->partDetails[i].maxFrame)
                {
                    int rate = (int)utilities->partDetails[i].worker->getFrameRate();
                    wnd->setFrameRate(utilities->partDetails[i].name.c_str(),rate);
                }
            }
            percentage = 0;
            percentage = ( utilities->partDetails[i].currFrame *100 ) / utilities->partDetails[i].maxFrame;
            wnd->setPartProgress( utilities->partDetails[i].name.c_str(), percentage );

            //fprintf(stdout, "part %d is at frame %d of %d therefore %d\n",i, utilities->partDetails[i].currFrame, utilities->partDetails[i].maxFrame, percentage);
        }
        if(i == 0)
            wnd->setPlayProgress(percentage);
    }

}
/**********************************************************/
void UpdateGui::updateGuiRateThread()
{
    this->run();
}



