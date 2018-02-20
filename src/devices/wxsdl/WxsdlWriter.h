/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

