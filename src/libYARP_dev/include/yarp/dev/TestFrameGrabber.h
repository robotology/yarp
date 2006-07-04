// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef _YARP2_TESTFRAMEGRABBER_
#define _YARP2_TESTFRAMEGRABBER_

#include <stdio.h>

#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/os/Searchable.h>
#include <yarp/os/Time.h>

namespace yarp {
    namespace dev {
        class TestFrameGrabber;
    }
}

/**
 * An implementation of IFrameGrabberImage and IFrameGrabberControls
 * for testing.
 */
class yarp::dev::TestFrameGrabber : public DeviceDriver, 
            public IFrameGrabberImage, public IFrameGrabberControls {
private:
    int ct;
    int w, h;
    double period, freq;
    double first;
    double prev;

public:
    /**
     * Constructor.
     */
    TestFrameGrabber() {
        ct = 0;
        freq = 20;
        period = 1/freq;
        // just for nostalgia
        w = 128;
        h = 128;
        first = 0;
        prev = 0;
    }


    virtual bool close() {
        return true;
    }

   /**
     * Configure with a set of options. These are:
     * <TABLE>
     * <TR><TD> width/w </TD><TD> Width of image (default 128). </TD></TR>
     * <TR><TD> height/h </TD><TD> Height of image (default 128). </TD></TR>
     * <TR><TD> freq </TD><TD> Frequency in Hz to generate images (default 20Hz). </TD></TR>
     * <TR><TD> period </TD><TD> Inverse of freq - only set one of these. </TD></TR>
     * </TABLE>
     *
     * @param config The options to use
     * @return true iff the object could be configured.
     */
    virtual bool open(yarp::os::Searchable& prop) {
        yarp::os::Value *val;
        if (prop.check("width",val)||prop.check("w",val)) {
            w = val->asInt();
        }
        if (prop.check("height",val)||prop.check("h",val)) {
            h = val->asInt();
        }
        if (prop.check("freq",val)) {
            freq = val->asDouble();
            period = 1/freq;
        }
        if (prop.check("period",val)) {
            period = val->asDouble();
            freq = 1/period;
        }
        printf("Test grabber period %g / freq %g\n", period, freq);
        return true;
    }

    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) {

        double now = yarp::os::Time::now();

        if (now-prev>1000) {
            first = now;
            prev = now;
        }
        double dt = period-(now-prev);

        if (dt>0) {
            yarp::os::Time::delay(dt);
        }
        
        // this is the controlled instant when we consider the
        // image as going out
        prev += period;

        image.resize(w,h);
        image.zero();
        for (int i=0; i<image.width(); i++) {
            image.pixel(i,ct).r = 255;
        }
        ct++;
        if (ct>=image.height()) {
            ct = 0;
        }
        return true;
    }
    
    virtual int height() const {
        return h;
    }

    virtual int width() const {
        return w;
    }

    virtual bool setBrightness(double v) {
        return false;
    }

    virtual bool setShutter(double v) {
        return false;
    }

    virtual bool setGain(double v) {
        return false;
    }

    virtual double getBrightness() const {
        return 0;
    }

    virtual double getShutter() const {
        return 0;
    }

    virtual double getGain() const {
        return 0;
    }
};


#endif
