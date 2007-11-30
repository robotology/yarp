// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#ifndef WxsdlWriter_INC
#define WxsdlWriter_INC

/*
 * Uses wxwidgets and SDL to view image sequences.
 *
 */

namespace yarp {
    namespace dev {
        class WxsdlWriter;
    }
}

#include <yarp/dev/AudioVisualInterfaces.h>
#include <yarp/dev/ServiceInterfaces.h>
#include <yarp/dev/DeviceDriver.h>

/**
 * @ingroup dev_impl_media
 *
 * Uses wxwidgets and sdl to view image sequences.
 *
 */
class yarp::dev::WxsdlWriter : public IFrameWriterImage, 
            public IService,
            public DeviceDriver, public yarp::os::Thread
{
public:

    WxsdlWriter() {
        active = false;
    }
  
    virtual bool open(yarp::os::Searchable & config);
    
    virtual bool close();
  
    virtual bool putImage(yarp::sig::ImageOf<yarp::sig::PixelRgb> & image);

    virtual void run();

    virtual bool updateService();
    
    virtual bool stopService();

private:
    bool active;
};


#endif

