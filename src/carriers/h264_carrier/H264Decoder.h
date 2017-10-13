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

class yarp::os::H264Decoder
{
private:
    int remotePort;
    void *sysResource;

public:
    yarp::os::Mutex mutex ; //==>create functions to work with it
    yarp::os::Semaphore semaphore;

    H264Decoder(int remotePort);
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
