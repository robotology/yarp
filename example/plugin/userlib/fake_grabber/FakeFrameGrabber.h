/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/all.h>
#include <yarp/dev/all.h>
class FakeFrameGrabber : public yarp::dev::IFrameGrabberImage,
                         public yarp::dev::DeviceDriver {
private:
    int w, h;
public:
    FakeFrameGrabber() {
        h = w = 0;
    }

    bool open(int w, int h) {
        this->w = w;
        this->h = h;
        return w>0 && h>0;
    }

    virtual bool open(yarp::os::Searchable& config) {
        // extract width and height configuration, if present
        // otherwise use 128x128
        int desiredWidth = config.check("w",yarp::os::Value(128)).asInt32();
        int desiredHeight = config.check("h",yarp::os::Value(128)).asInt32();
        return open(desiredWidth,desiredHeight);
    }

    virtual bool close() {
        return true; // easy
    }

    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image);

    virtual int height() const {
        return h;
    }

    virtual int width() const {
        return w;
    }
};
