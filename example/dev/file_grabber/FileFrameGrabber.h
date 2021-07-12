/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/ImageFile.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IFrameGrabberImage.h>

class FileFrameGrabber :
        public yarp::dev::IFrameGrabberImage,
        public yarp::dev::DeviceDriver
{
private:
    std::string pattern{"%d.ppm"};
    std::string lastLoad;
    int first{-1};
    int last{-1};
    int at{-1};
    int h{0};
    int w{0};

    bool findImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image)
    {
        bool triedFirst = false;
        char buf[1000];
        sprintf(buf, pattern.c_str(), at);
        while (!yarp::sig::file::read(image, buf)) {
            if (at == first) {
                if (triedFirst) {
                    return false;
                }
                triedFirst = true;
            }
            if (last == -1) {
                at = first;
            } else {
                at++;
                if (at > last) {
                    at = first;
                }
            }
            sprintf(buf, pattern.c_str(), at);
        }
        lastLoad = buf;
        h = image.height();
        w = image.width();
        return true;
    }

public:
    bool open(const char* pattern, int first, int last)
    {
        this->pattern = pattern;
        this->first = first;
        this->last = last;
        at = first;
        yarp::sig::ImageOf<yarp::sig::PixelRgb> dummy;
        return findImage(dummy);
    }

    bool open(yarp::os::Searchable& config) override
    {
        std::string pattern = config.check("pattern", yarp::os::Value("%d.ppm")).asString();
        int first = config.check("first", yarp::os::Value(0)).asInt32();
        int last = config.check("last", yarp::os::Value(-1)).asInt32();
        return open(pattern.c_str(), first, last);
    }

    bool close() override
    {
        return true; // easy
    }

    bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) override
    {
        bool ok = findImage(image);
        if (ok) {
            printf("showing image %s\n", lastLoad.c_str());
            at++;
            if (last != -1 && at > last) {
                at = first;
            }
        }
        return ok;
    }

    int height() const override
    {
        return h;
    }

    int width() const override
    {
        return w;
    }
};
