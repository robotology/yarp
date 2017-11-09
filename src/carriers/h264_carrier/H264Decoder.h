/*
 * Copyright: (C) 2017 Istituto Italiano di Tecnologia (IIT)
 * Author: Valentina Gaggero <valentina.gaggero@iit.it>
 * Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef H264DECODER_INC
#define H264DECODER_INC

#include <yarp/os/Mutex.h>
#include <yarp/sig/Image.h>
#include <yarp/os/Semaphore.h>

namespace yarp {
    namespace os {
        class H264Decoder;
    }
}
typedef struct
{
    struct
    {
        int left;   //number of pixel to crop from left
        int right;  //number of pixel to crop from right
        int top;    //number of pixel to crop from top
        int bottom; //number of pixel to crop from bottom
    }crop;
    int fps_max;    //max value of fps. it is imposed by gstreamer
    int remotePort; // the port on which the server send data
    bool verbose;   //enables debug print of gstream plugin
} h264Decoder_cfgParamters;

class yarp::os::H264Decoder
{
private:
    int remotePort;
    void *sysResource;
    h264Decoder_cfgParamters cfg;

public:
    yarp::os::Mutex mutex ; //==>create functions to work with it
    yarp::os::Semaphore semaphore;

    H264Decoder(h264Decoder_cfgParamters &config);
    ~H264Decoder();
    bool init(void);
    bool start();
    bool stop();
    yarp::sig::ImageOf<yarp::sig::PixelRgb> & getLastFrame(void);
    int getLastFrameSize(void);
    bool newFrameIsAvailable(void);
    void setReq(void);
};

#endif
