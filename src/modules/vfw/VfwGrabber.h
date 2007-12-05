// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
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


