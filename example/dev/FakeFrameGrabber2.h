/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

class FakeFrameGrabber : public yarp::dev::IFrameGrabberImage, 
                         public yarp::dev::IFrameGrabberControls, 
                         public yarp::dev::DeviceDriver {
private:
    int w, h;
    int ct;
    yarp::sig::PixelRgb circlePixel;
    double brightness;
public:
    FakeFrameGrabber() {
        h = w = 0;
        ct = 0;
        circlePixel = yarp::sig::PixelRgb(255,0,0);
        brightness = 1;
    }

    bool open(int w, int h) {
        this->w = w;
        this->h = h;
        return w>0 && h>0;
    }

    virtual bool open(yarp::os::Searchable& config) { 
        // extract width and height configuration, if present
        // otherwise use 128x128
        int desiredWidth = config.check("w",yarp::os::Value(128)).asInt();
        int desiredHeight = config.check("h",yarp::os::Value(128)).asInt();
        return open(desiredWidth,desiredHeight);
    }

    virtual bool close() { 
        return true; // easy
    }

    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) {
        yarp::os::Time::delay(0.1);  // simulate waiting for hardware to report
        image.resize(w,h);
        image.zero();
        yarp::sig::draw::addCrossHair(image,circlePixel,
                                      ct,h/2,h/8);
        ct = (ct+4)%w;
        return true;
    }

    virtual int height() const {
        return h;
    }

    virtual int width() const {
        return w;
    }

    virtual bool setBrightness(double v) {
        if (v>1) v = 1;
        if (v<0) v = 0;
        circlePixel = yarp::sig::PixelRgb((unsigned char)(255*v),0,0);
        brightness = v;
        return true;
    }

    virtual bool setShutter(double v) {
        return false;
    }

    virtual bool setGain(double v) {
        return false;
    }


    virtual bool setWhiteBalance(double red, double green) {
        return false;
    }

	virtual bool getWhiteBalance(double &red, double &green)
	{
		red=0;
		green=0;
		return false;
	}

    virtual double getBrightness() {
        return brightness;
    }

    virtual double getShutter() {
        return 0;
    }

    virtual double getGain() {
        return 0;
    }



    virtual bool setExposure(double v) {
        return false;
    }
    virtual bool setSharpness(double v) {
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
    virtual bool setIris(double v) {
        return false;
    }

    virtual double getExposure() {
        return 0.0;
    }
    virtual double getSharpness() {
        return 0.0;
    }
    virtual double getHue() {
        return 0.0;
    }
    virtual double getSaturation() {
        return 0.0;
    }
    virtual double getGamma() {
        return 0.0;
    }        
    virtual double getIris() {
        return 0.0;
    }
    

};

