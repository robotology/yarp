// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#ifndef _YARP2_TESTFRAMEGRABBER_
#define _YARP2_TESTFRAMEGRABBER_

#include <stdio.h>

#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/os/Searchable.h>
#include <yarp/os/Time.h>
#include <yarp/os/Vocab.h>

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
class yarp::dev::TestFrameGrabber : public DeviceDriver, 
            public IFrameGrabberImage, public IFrameGrabberControls {
private:
    int ct;
    int bx, by;
    int w, h;
    unsigned long rnd;
    double period, freq;
    double first;
    double prev;
    int mode;

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
     * <TR><TD> mode </TD><TD> Can be [ball] or [line] (default). </TD></TR>
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
                            "bouncy [ball], scrolly [line], grid [grid], random [rand]").asVocab();

		if (freq!=-1)
		{
			printf("Test grabber period %g / freq %g , mode [%s]\n", period, freq,
				yarp::os::Vocab::decode(mode).c_str());
		}
		else
		{
			printf("Test grabber period %g / freq [inf], mode [%s]\n", period,
				yarp::os::Vocab::decode(mode).c_str());
		}
       
		bx = w/2;
        by = h/2;
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

        createTestImage(image);
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
    

private:
    void createTestImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image);
};


/**
 * @ingroup dev_runtime
 * \defgroup cmd_device_test_grabber test_grabber

 A fake framegrabber, see yarp::dev::TestFrameGrabber.

*/


#endif
