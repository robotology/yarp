/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/dev/AudioVisualInterfaces.h>

namespace yarp {
    namespace dev {
        class VfwGrabber;
    }
}


class yarp::dev::VfwGrabber : public DeviceDriver, public IFrameGrabberImage {
public:
    VfwGrabber() { system_resource = NULL; _height=_width=0; }

    virtual ~VfwGrabber() { close(); }

    virtual bool open(yarp::os::Searchable& config);
    virtual bool close();
    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image);

    virtual int height() const { return _height; }
    virtual int width() const  { return _width; }

private:
    void *system_resource;
    int _height, _width;
};


