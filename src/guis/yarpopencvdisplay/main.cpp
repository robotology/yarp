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
#include <yarp/sig/LayeredImage.h>

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
    yInfo() << "--layered";
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
    bool layered = rf.check("layered", Value(false), "set if the input port will receive a layered image or not").asBool();
    size_t visible_layers = 10;

    yInfo() << "yarpopencvdisplay ready. Press ESC to quit.";

    yarp::os::Contactable* inputPort = nullptr;
    if (!layered)
    {
        inputPort = new BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb>>;
    }
    else
    {
        inputPort = new BufferedPort<yarp::sig::LayeredImage>;
    }

    if (inputPort->open(local) == false)
    {
        yError() << "Failed to open port" << local;
        return 0;
    }
    else
    {
        if (layered)
        {
            yInfo() << "Opened LAYERED port:" << local;
        }
        else
        {
            yInfo() << "Opened port:" << local;
        }
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


    FlexImage* fl = nullptr;
    yarp::sig::ImageOf<yarp::sig::PixelRgb>* imgdata = nullptr;

    while(!exit)
    {
        //Receive image and draw
        {
            if (layered)
            {
                BufferedPort<yarp::sig::LayeredImage>* ip = dynamic_cast<BufferedPort<yarp::sig::LayeredImage>*>(inputPort);
                yarp::sig::LayeredImage* ly = ip->read(false);
                if (ly)
                {
                    if (fl)
                    {
                        delete fl;
                        fl = nullptr;
                    }
                    for (size_t i = 0; i < ly->layers.size(); i++)
                    {
                        if (i < visible_layers)
                        {
                            ly->layers[i].enable = true;
                        }
                        else
                        {
                            ly->layers[i].enable = false;
                        }
                    }
                    fl = new FlexImage(*ly);
                    imgdata = (reinterpret_cast<yarp::sig::ImageOf<yarp::sig::PixelRgb>*>(fl));
                }
            }
            else
            {
                BufferedPort <yarp::sig::ImageOf<yarp::sig::PixelRgb>>* ip = dynamic_cast < BufferedPort < yarp::sig::ImageOf<yarp::sig::PixelRgb>>*>(inputPort);
                yarp::sig::ImageOf<yarp::sig::PixelRgb>* ir = ip->read(false);
                if (ir)
                {
                    imgdata = ir;
                }
            }

            //if received a new frame, then draw/save it
            if (imgdata!=nullptr)
            {
                static double old = yarp::os::Time::now();
                double now = yarp::os::Time::now();
                stats.interval_time = now - old;
                old = yarp::os::Time::now();

                //if this is first received frame, allocate a buffer on the specific size
                if (frame.empty())
                {
                    yDebug() << "First frame received with size" << imgdata->width() << imgdata->height();
                    //create the buffer for the frame
                    frame = cv::Mat(imgdata->width(), imgdata->height(), CV_8UC3);
                    //if optionally requested to create an avi file, create the file
                    if (filename.empty() == false)
                    {
                        int codec = cv::VideoWriter::fourcc(codec_s[0], codec_s[1], codec_s[2], codec_s[3]);
                        cv::Size frame_size(imgdata->width(), imgdata->height());
                        bool ret = videowriter.open(filename.c_str(), codec, fps, frame_size, true);
                        if (!ret || videowriter.isOpened() == false)
                        {
                            yError("Unable to open the output file");
                            return -1;
                        }
                    }
                }
                //you also need to realloc the buffer if the received frame changed size
                else if (frame.cols != (int)imgdata->width() || frame.rows != (int)imgdata->height())
                {
                    yDebug() << "Steam has received a frame with different size" << imgdata->width() << imgdata->height();
                    frame = cv::Mat(imgdata->height(), imgdata->width(), CV_8UC3);
                }

                //copy the data from the yarp structure 'imgdata' to the opencv structure 'frame'
                double a = yarp::os::Time::now();
                for (size_t y = 0; y < imgdata->height(); y++) {
                    for (size_t x = 0; x < imgdata->width(); x++) {
                        PixelRgb& yarpPixel = imgdata->pixel(x, y);
                        frame.at<cv::Vec3b>(y, x) = cv::Vec3b(yarpPixel.b, yarpPixel.g, yarpPixel.r);
                    }
                }

                //add stats to frame
                double b = yarp::os::Time::now();
                stats.copy_time = b - a;
                if (verbose)
                {
                    yDebug("copytime: %5.3f frameintervale %5.3f", stats.copy_time, stats.interval_time);
                    drawImageExtraStuff(frame);
                }

                //save to file
                if (recording)
                {
                    videowriter.write(frame);
                }

                //draw
                cv::imshow(window_name, frame);
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
        if (keypressed == 'l') {
            visible_layers++;
            if (visible_layers > 10) {
                visible_layers = 0;
                yInfo("Changed max number of visible Layers: %d", visible_layers);
            }
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

    inputPort->close();
    delete inputPort;

    cv::destroyAllWindows();

    if (videowriter.isOpened())
    {
        videowriter.release();
    }
}
