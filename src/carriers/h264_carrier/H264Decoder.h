/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef H264DECODER_INC
#define H264DECODER_INC

#include <mutex>
#include <yarp/sig/Image.h>
#include <yarp/os/Semaphore.h>

struct h264Decoder_cfgParamters
{
    h264Decoder_cfgParamters() :
        crop{0,0,0,0},
        fps_max(0),
        remotePort(-1)
    {}

    struct
    {
        int left;   //number of pixel to crop from left
        int right;  //number of pixel to crop from right
        int top;    //number of pixel to crop from top
        int bottom; //number of pixel to crop from bottom
    } crop;

    int fps_max;    //max value of fps. it is imposed by gstreamer
    int remotePort; // the port on which the server send data
    bool removeJitter; //If true, the carrier reorders and removes duplicate RTP packets as they are received from a network source.
};

class H264Decoder
{
private:
    void *sysResource;
    h264Decoder_cfgParamters cfg;

public:
    std::mutex mutex ; //==>create functions to work with it
    yarp::os::Semaphore semaphore;

    H264Decoder(h264Decoder_cfgParamters &config);
    ~H264Decoder();
    bool init();
    bool start();
    bool stop();
    yarp::sig::ImageOf<yarp::sig::PixelRgb>& getLastFrame();
    int getLastFrameSize();
    bool newFrameIsAvailable();
    void setReq();
};

#endif
