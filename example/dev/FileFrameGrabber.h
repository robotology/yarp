// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

class FileFrameGrabber : public yarp::dev::IFrameGrabberImage, 
                         public yarp::dev::DeviceDriver {
private:
    yarp::os::ConstString pattern, lastLoad;
    int first, last, at;
    int h, w;

    bool findImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) {
        bool triedFirst = false;
        char buf[1000];
        sprintf(buf,pattern.c_str(),at);
        while (!yarp::sig::file::read(image,buf)) {
            if (at==first) {
                if (triedFirst) {
                    return false;
                }
                triedFirst = true;
            }
            if (last==-1) {
                at = first;
            } else {
                at++;
                if (at>last) {
                    at = first;
                }
            }
            sprintf(buf,pattern.c_str(),at);
        }
        lastLoad = buf;
        h = image.height();
        w = image.width();
        return true;
    }

public:
    FileFrameGrabber() {
        pattern = "%d.ppm";
        first = last = -1;
        at = -1;
        h = w = 0;
    }

    bool open(const char *pattern, int first, int last) {
        this->pattern = pattern;
        this->first = first;
        this->last = last;
        at = first;
        yarp::sig::ImageOf<yarp::sig::PixelRgb> dummy;
        return findImage(dummy);
    }

    virtual bool open(yarp::os::Searchable& config) { 
        yarp::os::ConstString pattern = 
            config.check("pattern",yarp::os::Value("%d.ppm")).asString();
        int first = config.check("first",yarp::os::Value(0)).asInt();
        int last = config.check("last",yarp::os::Value(-1)).asInt();
        return open(pattern.c_str(),first,last);
    }

    virtual bool close() { 
        return true; // easy
    }

    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) {
        bool ok = findImage(image);
        if (ok) {
            printf("showing image %s\n", lastLoad.c_str());
            at++;
            if (last!=-1 && at>last) {
                at = first;
            }
        }
        return ok;
    }

    virtual int height() const {
        return h;
    }

    virtual int width() const {
        return w;
    }
};

