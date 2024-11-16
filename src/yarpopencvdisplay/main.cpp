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
#include <opencv2/opencv.hpp>
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

const cv::Scalar color_bwhite (200, 200, 255);
const cv::Scalar color_white (255, 255, 255);
const cv::Scalar color_red (0, 0, 255);
const cv::Scalar color_yellow (0, 255, 255);
const cv::Scalar color_black (0, 0, 0);
const cv::Scalar color_gray (100, 100, 100);

struct stats_struct
{
    double interval_time = 0;
    double copy_time = 0;
} stats;

void drawImageExtraStuff(cv::Mat& img)
{
    //cvLine(img,cvPoint(0,0),cvPoint(img->width,img->height),color_black);

    char buff[20];
    snprintf(buff, 20, "%5.3f", stats.interval_time);
    cv::putText(img, buff, cv::Point(20, 20), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(90, 90, 90), 1, cv::LINE_AA);
}

void display_help()
{
    yInfo() << "Available options:";
    yInfo() << "--stats";
    yInfo() << "--remote <portname>";
    yInfo() << "--local <portname>";
    yInfo() << "--carrier <carriername>";
    yInfo() << "--record_filename <filename>";
    yInfo() << "--record_start";
    yInfo() << "--record_fps";
    yInfo() << "--record_codec";
}

int main(int argc, char* argv[])
{
    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);

    ResourceFinder rf;
    rf.setDefaultConfigFile("yarpopencvdisplay.ini");
    rf.configure(argc, argv);
    if (rf.check("help")) {
        display_help();
        return 0;
    }

    std::string filename = rf.check("record_filename", Value(""), "filename e.g. output.avi").asString();
    std::string codec_s = rf.check("record_codec", Value("MJPG"), "codec (max 4 chars, e.g. MJPG)").asString();
    if (codec_s.size() > 4)
    {
        yInfo() << "Invalid codec. Should be e 4 chars string, e.g. MJPG, DIVX, MP4V etc.";
        return 0;
    }
    bool recording = rf.check("record_start", Value(false), "automatically start recording on receiving the first frame").asBool();
    int fps = rf.check("record_fps", Value(30), "fps of the output video file").asInt32();
    if (!filename.empty())
    {
        yInfo() << "Output file will be stored:" << filename << "(fps:" << fps << ", codec:" << codec_s << ")";
        if (recording)
        {
            yInfo() << "record_start option found. Recoding will start on first received frame";
        }
    }

    bool verbose = rf.check("stats", Value(false), "verbose stats [0/1]").asBool();
    std::string remote = rf.check("remote", Value(""), "remote port name").asString();
    std::string carrier = rf.check("carrier", Value("fast_tcp"), "carrier name").asString();
    std::string local = rf.check("local", Value("/yarpopencvdisplay:i"), "local port name").asString();

    yInfo() << "yarpopencvdisplay ready. Press ESC to quit.";

    BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb>> inputPort;
    if (inputPort.open(local) == false) {
        yError() << "Failed to open port" << local;
        return 0;
    }
    if (!remote.empty()) {
        if (yarp::os::Network::connect(remote, local, carrier)) {
            yInfo() << "Successfully connected to port:" << remote;
        } else {
            yError() << "Failed to connect to port:" << remote;
            return 0;
        }
    } else
    {
        yInfo() << "Waiting for port connection...";
    }

    std::string window_name = "yarpopencvdisplay: " + local;
    cv::Mat frame;
    cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);
    cv::VideoWriter videowriter;

    bool exit = false;

    while(!exit)
    {
        //Receive image and draw
        {
            auto* imgport = inputPort.read(false);
            if (imgport)
            {
                static double old = yarp::os::Time::now();
                double now = yarp::os::Time::now();
                stats.interval_time = now - old;
                old = yarp::os::Time::now();

                //if first received frame
                if (frame.empty())
                {
                    yDebug() << "First frame received with size" << imgport->width() << imgport->height();
                    //create the buffer for the frame
                    frame = cv::Mat(imgport->width(), imgport->height(), CV_8UC3);
                    //if optionally requested to create an avi file, cretate the file
                    if (filename.empty() == false)
                    {
                        int codec = cv::VideoWriter::fourcc(codec_s[0], codec_s[1], codec_s[2], codec_s[3]);
                        cv::Size frame_size(imgport->width(), imgport->height());
                        bool ret = videowriter.open(filename.c_str(), codec, fps, frame_size, true);
                        if (!ret || videowriter.isOpened() == false)
                        {
                            yError("Unable to open the output file");
                            return -1;
                        }
                    }
                }
                else if (frame.cols != (int)imgport->width() || frame.rows != (int)imgport->height())
                {
                    frame = cv::Mat(imgport->height(), imgport->width(), CV_8UC3);
                }

                double a = yarp::os::Time::now();
                for (size_t y = 0; y < imgport->height(); y++) {
                    for (size_t x = 0; x < imgport->width(); x++) {
                        PixelRgb& yarpPixel = imgport->pixel(x, y);
                        frame.at<cv::Vec3b>(y, x) = cv::Vec3b(yarpPixel.b, yarpPixel.g, yarpPixel.r);
                    }
                }
                double b = yarp::os::Time::now();
                stats.copy_time = b - a;

                if (!frame.empty())
                {
                    if (verbose)
                    {
                        yDebug("copytime: %5.3f frameintervale %5.3f", stats.copy_time, stats.interval_time);
                        drawImageExtraStuff(frame);
                    }

                    if (recording) {
                        videowriter.write(frame);
                    }

                    cv::imshow(window_name, frame);
                }
            }
        }

        SystemClock::delaySystem(0.001);

        //if ESC is pressed, exit.
        int keypressed = cv::waitKey(2); //wait 2ms. Lower values do not work under Linux
        keypressed &= 0xFF; //this mask is required in Linux systems
        if (keypressed == 27)
        {
            exit = true;
        }
        if(keypressed == 'w')
        {
            recording = (!recording);
            if (recording) {
                yInfo("recording is now ON");
            } else {
                yInfo("recording is now OFF");
            }
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

    cv::destroyAllWindows();

    if (videowriter.isOpened())
    {
        videowriter.release();
    }
}
