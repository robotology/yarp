// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef _YARP2_TESTFRAMEGRABBER_
#define _YARP2_TESTFRAMEGRABBER_

#include <stdio.h>

#include <yarp/sig/ImageFile.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/AudioVisualInterfaces.h>
#include <yarp/dev/PreciselyTimed.h>
#include <yarp/os/Searchable.h>
#include <yarp/os/Time.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Log.h>

#define VOCAB_LINE VOCAB4('l','i','n','e')

namespace yarp {
    namespace dev {
        class TestFrameGrabber;
    }
}

/**
 * @ingroup dev_impl_media
 *
 * A fake camera for testing.
 * Implements the IFrameGrabberImage and IFrameGrabberControls
 * interfaces.
 */
class YARP_dev_API yarp::dev::TestFrameGrabber : public DeviceDriver,
                                                 public IFrameGrabberImage,
                                                 public IFrameGrabberImageRaw,
                                                 public IFrameGrabberControls,
                                                 public IPreciselyTimed,
                                                 public IAudioVisualStream
{
private:
    int ct;
    int bx, by;
    int w, h;
    unsigned long rnd;
    double period, freq;
    double first;
    double prev;
    int mode;
    bool use_bayer;
    bool use_mono;

public:
    /**
     * Constructor.
     */
    TestFrameGrabber() {
        ct = 0;
        freq = 30;
        period = 1/freq;
        // just for nostalgia
        w = 128;
        h = 128;
        first = 0;
        prev = 0;
        rnd = 0;
        use_bayer = false;
        use_mono = false;
    }


    virtual bool close() {
        return true;
    }

    /**
     * Configure with a set of options. These are:
     * <TABLE>
     * <TR><TD> width </TD><TD> Width of image (default 128). </TD></TR>
     * <TR><TD> height </TD><TD> Height of image (default 128). </TD></TR>
     * <TR><TD> freq </TD><TD> Frequency in Hz to generate images (default 20Hz). </TD></TR>
     * <TR><TD> period </TD><TD> Inverse of freq - only set one of these. </TD></TR>
     * <TR><TD> mode </TD><TD> Can be [line] (default), [ball], [grid], [rand], [none]. </TD></TR>
     * <TR><TD> src </TD><TD> Image file to read from (default: none). </TD></TR>
     * <TR><TD> bayer </TD><TD> Emit a bayer image. </TD></TR>
     * <TR><TD> mono </TD><TD> Emit a monochrome image. </TD></TR>
     * </TABLE>
     *
     * @param config The options to use
     * @return true iff the object could be configured.
     */
    virtual bool open(yarp::os::Searchable& config) {
        yarp::os::Value *val;
        w = config.check("width",yarp::os::Value(128),
                         "desired width of test image").asInt();
        h = config.check("height",yarp::os::Value(128),
                         "desired height of test image").asInt();
        if (config.check("freq",val,"rate of test images in Hz")) {
            freq = val->asDouble();
            period = 1/freq;
        } else if (config.check("period",val,
                                "period of test images in seconds")) {
            period = val->asDouble();
            if(period<=0)
			{
				period =0;
				freq = -1;
			}
        }
        mode = config.check("mode",
                            yarp::os::Value(VOCAB_LINE, true),
                            "bouncy [ball], scrolly [line], grid [grid], random [rand], none [none]").asVocab();

        if (config.check("src")) {
            if (!yarp::sig::file::read(background,
                                       config.check("src",
                                                    yarp::os::Value("test.ppm"),
                                                    "background image to use, if any").asString().c_str())) {
                return false;
            }
            if (background.width()>0) {
                w = background.width();
                h = background.height();
            }
        }

        use_bayer = config.check("bayer","should emit bayer test image?");
        use_mono = config.check("mono","should emit a monochrome image?");
        use_mono = use_mono||use_bayer;

        if (freq!=-1) {
            yInfo("Test grabber period %g / freq %g , mode [%s]", period, freq,
                   yarp::os::Vocab::decode(mode).c_str());
        } else {
            yInfo("Test grabber period %g / freq [inf], mode [%s]", period,
                   yarp::os::Vocab::decode(mode).c_str());
        }

		bx = w/2;
        by = h/2;
        return true;
    }

    void timing() {
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
    }

    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) {
        timing();
        createTestImage(image);
        return true;
    }


    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image) {
        timing();
        createTestImage(rgb_image);
        if (use_bayer) {
            makeSimpleBayer(rgb_image,image);
        } else {
            image.copy(rgb_image);
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
    virtual bool setExposure(double v) {
        return false;
    }
    virtual bool setSharpness(double v) {
        return false;
    }
    virtual bool setWhiteBalance(double blue, double red){
	return false;
    }
    virtual bool setHue(double v) {
        return false;
    }
    virtual bool setSaturation(double v) {
        return false;
    }
     virtual bool setGamma(double v) {
        return false;
    }
    virtual bool setShutter(double v) {
        return false;
    }
    virtual bool setGain(double v) {
        return false;
    }
    virtual bool setIris(double v) {
        return false;
    }

    virtual double getBrightness(){
        return 0.0;
    }
    virtual double getExposure(){
        return 0.0;
    }
    virtual double getSharpness(){
        return 0.0;
    }
    virtual bool getWhiteBalance(double &blue, double &red)
    {
	red=0.0;
	blue=0.0;
	return true;
    }
    virtual double getHue(){
        return 0.0;
    }
    virtual double getSaturation(){
        return 0.0;
    }
    virtual double getGamma(){
        return 0.0;
    }
    virtual double getShutter(){
        return 0.0;
    }
    virtual double getGain(){
        return 0.0;
    }
    virtual double getIris(){
        return 0.0;
    }

    virtual yarp::os::Stamp getLastInputStamp() {
        return stamp;
    }

    virtual bool hasAudio() { return false; }

    virtual bool hasVideo() { return !use_mono; }

    virtual bool hasRawVideo() {
        return use_mono;
    }

private:
    yarp::sig::ImageOf<yarp::sig::PixelRgb> background, rgb_image;
    yarp::os::Stamp stamp;
    void createTestImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image);

    bool makeSimpleBayer(yarp::sig::ImageOf<yarp::sig::PixelRgb>& src,
                         yarp::sig::ImageOf<yarp::sig::PixelMono>& bayer);

};


/**
 * @ingroup dev_runtime
 * \defgroup cmd_device_test_grabber test_grabber

 A fake framegrabber, see yarp::dev::TestFrameGrabber.

*/


#endif
