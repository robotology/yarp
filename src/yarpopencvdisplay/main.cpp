/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#define _USE_MATH_DEFINES

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdio>
#include <limits>
#include <cmath>
#include <opencv2/core/version.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/highgui.hpp>
#include <vector>

#include <yarp/dev/Drivers.h>
#include <yarp/os/Network.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/LogStream.h>
#include <yarp/sig/Image.h>

using namespace yarp::os;
using namespace yarp::sig;

CvFont font;
CvFont fontBig;

const CvScalar color_bwhite = cvScalar(200,200,255);
const CvScalar color_white  = cvScalar(255,255,255);
const CvScalar color_red    = cvScalar(0,0,255);
const CvScalar color_yellow = cvScalar(0,255,255);
const CvScalar color_black  = cvScalar(0,0,0);
const CvScalar color_gray   = cvScalar(100,100,100);

struct stats_struct
{
    double interval_time = 0;
    double copy_time = 0;
} stats;

void drawImageExtraStuff(IplImage* img)
{
    //cvLine(img,cvPoint(0,0),cvPoint(img->width,img->height),color_black);

    char buff[20];
    snprintf(buff, 20, "%5.3f", stats.interval_time);
    cvPutText(img, buff, cvPoint(20,20), &font, cvScalar(90, 90, 90, 0));

}

void display_help()
{
    yInfo() << "Available options:";
    yInfo() << "--verbose";
    yInfo() << "--remote <portname>";
    yInfo() << "--local <portname>";
    yInfo() << "--carrier <carriername>";
}

int main(int argc, char *argv[])
{
    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);

    ResourceFinder rf;
    rf.setDefaultConfigFile("yarpopencvdisplay.ini");
    rf.configure(argc, argv);
    if (rf.check("help"))
    {
        display_help();
        return 0;
    }

    bool verbose = rf.check("verbose", Value(false), "verbose [0/1]").asBool();
    std::string remote  = rf.check("remote", Value(""), "remote port name").asString();
    std::string carrier = rf.check("carrier", Value("fast_tcp"), "carrier name").asString();
    std::string local   = rf.check("local", Value("/yarpopencvdisplay:i"), "local port name").asString();

    BufferedPort <yarp::sig::ImageOf<yarp::sig::PixelRgb>> inputPort;
    if (inputPort.open(local) == false)
    {
        yError() << "Failed to open port" << local;
        return 0;
    }
    if (!remote.empty())
    {
        if (yarp::os::Network::connect(remote, local, carrier))
        {
            yInfo() << "Successfully connected to port:" << remote;
        }
        else
        {
            yError() << "Failed to connect to port:" << remote;
            return 0;
        }
    }

    std::string window_name = "yarpopencvdisplay: " + local;
    IplImage* iplimg = nullptr;
    cvNamedWindow(window_name.c_str(),CV_WINDOW_AUTOSIZE);
    cvInitFont(&font,    CV_FONT_HERSHEY_SIMPLEX, 0.4, 0.4, 0, 1, CV_AA);
    cvInitFont(&fontBig, CV_FONT_HERSHEY_SIMPLEX, 0.8, 0.8, 0, 1, CV_AA);

    bool exit = false;

    while(!exit)
    {
        void *v = cvGetWindowHandle(window_name.c_str());
        if (v == nullptr)
        {
            exit = true;
            break;
        }

        //Receive image and draw
        {
            auto* imgport = inputPort.read(false);
            if (imgport)
            {
                static double old = yarp::os::Time::now();
                double now = yarp::os::Time::now();
                stats.interval_time = now - old;
                old = yarp::os::Time::now();

                if (iplimg == nullptr)
                {
                    iplimg = cvCreateImage(cvSize(imgport->width(), imgport->height()), IPL_DEPTH_8U, 3);
                }
                else if (iplimg->width != imgport->width() || iplimg->height != imgport->height())

                {
                    cvReleaseImage(&iplimg);
                    iplimg = cvCreateImage(cvSize(imgport->width(), imgport->height()), IPL_DEPTH_8U, 3);
                }

                double a = yarp::os::Time::now();
                for (int y = 0; y < imgport->height(); y++) {
                    for (int x = 0; x < imgport->width(); x++) {
                        PixelRgb& yarpPixel = imgport->pixel(x, y);
                        CvScalar cvPixel = cvScalar(yarpPixel.b, yarpPixel.g, yarpPixel.r);
                        cvSet2D(iplimg, y, x, cvPixel);
                    }
                }
                double b = yarp::os::Time::now();
                stats.copy_time = b - a;
            }
            if (verbose)
            {
                yDebug("copytime: %5.3f frameintervale %5.3f", stats.copy_time, stats.interval_time);
                drawImageExtraStuff(iplimg);
            }
            cvShowImage(window_name.c_str(), iplimg);
        }

        SystemClock::delaySystem(0.001);

        //if ESC is pressed, exit.
        int keypressed = cvWaitKey(2); //wait 2ms. Lower values do not work under Linux
        keypressed &= 0xFF; //this mask is required in Linux systems
        if (keypressed == 27)
        {
            exit = true;
        }
        if(keypressed == 'w')
        {
        }
        if(keypressed == 's')
        {
        }
        if(keypressed == 'v' )
        {
           verbose= (!verbose);
           if (verbose) {
               yInfo("verbose mode is now ON");
           } else {
               yInfo("verbose mode is now OFF");
           }
        }
    }

    inputPort.close();

    cvDestroyAllWindows();

    if (iplimg)
    {
        cvReleaseImage(&iplimg);
    }
}
