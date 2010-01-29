// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/* 
 * Copyright (C) 2006 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Paul Fitzpatrick, Alessandro Scalzo
 * email: paulfitz@alum.mit.edu, alessandro.scalzo@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/


#ifndef _YARP2_REMOTEFRAMEGRABBER_
#define _YARP2_REMOTEFRAMEGRABBER_

#include <yarp/dev/ServerFrameGrabber.h>
#include <yarp/os/Network.h>
#include <yarp/os/Semaphore.h>


namespace yarp{
    namespace dev {
        class RemoteFrameGrabber;
    }
}

/**
 * @ingroup dev_impl_wrapper
 *
 * Connect to a ServerFrameGrabber.  See ServerFrameGrabber for
 * the network protocol used.
 *
 */
class yarp::dev::RemoteFrameGrabber : public IFrameGrabberImage, 
            public IFrameGrabberControls,
            public DeviceDriver {

public:
    /**
     * Constructor.
     */
    RemoteFrameGrabber() : mutex(1) {
        lastHeight = 0;
        lastWidth = 0;
    }

    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) {
        mutex.wait();
        if (reader.read(true)!=NULL) {
            image = *(reader.lastRead());
            lastHeight = image.height();
            lastWidth = image.width();
            mutex.post();
            return true;
        }
        mutex.post();
        return false;
    }

    // this is bad!
    virtual int height() const {
        return lastHeight;
    }

    virtual int width() const {
        return lastWidth;
    }

    /**
     * Configure with a set of options. These are:
     * <TABLE>
     * <TR><TD> local </TD><TD> Port name of this client. </TD></TR>
     * <TR><TD> remote </TD><TD> Port name of server to connect to. </TD></TR>
     * </TABLE>
     *
     * @param config The options to use
     * @return true iff the object could be configured.
     */
    virtual bool open(yarp::os::Searchable& config){
        remote = config.check("remote",yarp::os::Value(""),
                              "port name of real grabber").asString();
        local = config.check("local",yarp::os::Value("..."),
                             "port name to use locally").asString();
        yarp::os::ConstString carrier = 
            config.check("stream",yarp::os::Value("tcp"),
                         "carrier to use for streaming").asString();
        port.open(local);
        if (remote!="") {
            yarp::os::Network::connect(remote,local,carrier);

            // reverse connection for RPC
            // could choose to do this only on need
            yarp::os::Network::connect(local,remote);
        }
        reader.attach(port);
        return true;
    }

    virtual bool close() {
        port.close();
//        mutex.wait();   // why does it need this?
        return true;
    }

/*
#define VOCAB_BRIGHTNESS VOCAB3('b','r','i')
#define VOCAB_EXPOSURE VOCAB4('e','x','p','o')
#define VOCAB_SHARPNESS VOCAB4('s','h','a','r')
#define VOCAB_WHITE VOCAB4('w','h','i','t')
#define VOCAB_HUE VOCAB3('h','u','e')
#define VOCAB_SATURATION VOCAB4('s','a','t','u')
#define VOCAB_GAMMA VOCAB4('g','a','m','m')
#define VOCAB_SHUTTER VOCAB4('s','h','u','t')
#define VOCAB_GAIN VOCAB4('g','a','i','n')
#define VOCAB_IRIS VOCAB4('i','r','i','s')
//#define VOCAB_TEMPERATURE VOCAB4('t','e','m','p')
//#define VOCAB_WHITE_SHADING VOCAB4('s','h','a','d')
//#define VOCAB_OPTICAL_FILTER VOCAB4('f','i','l','t')
//#define VOCAB_CAPTURE_QUALITY VOCAB4('q','u','a','l')
*/

    virtual bool setBrightness(double v) {
        return setCommand(VOCAB_BRIGHTNESS,v);
    }
    virtual double getBrightness() {
        return getCommand(VOCAB_BRIGHTNESS);
    }
	virtual bool setExposure(double v) {
        return setCommand(VOCAB_EXPOSURE,v);
    }
    virtual double getExposure() {
        return getCommand(VOCAB_EXPOSURE);
    }

    virtual bool setSharpness(double v) {
        return setCommand(VOCAB_SHARPNESS,v);
    }
    virtual double getSharpness() {
        return getCommand(VOCAB_SHARPNESS);
    }

	virtual bool setWhiteBalance(double blue, double red)
	{
		return setCommand(VOCAB_WHITE, blue, red);
	}
	virtual bool getWhiteBalance(double &blue, double &red)
	{
		return getCommand(VOCAB_WHITE, blue, red);
	}

    virtual bool setHue(double v) {
        return setCommand(VOCAB_HUE,v);
    }
    virtual double getHue() {
        return getCommand(VOCAB_HUE);
    }

    virtual bool setSaturation(double v) {
        return setCommand(VOCAB_SATURATION,v);
    }
    virtual double getSaturation() {
        return getCommand(VOCAB_SATURATION);
    }

    virtual bool setGamma(double v) {
        return setCommand(VOCAB_GAMMA,v);
    }
    virtual double getGamma() {
        return getCommand(VOCAB_GAMMA);
    }

    virtual bool setShutter(double v) {
        return setCommand(VOCAB_SHUTTER,v);
    }
    virtual double getShutter() {
        return getCommand(VOCAB_SHUTTER);
    }

    virtual bool setGain(double v) {
        return setCommand(VOCAB_GAIN,v);
    }
    virtual double getGain() {
        return getCommand(VOCAB_GAIN);
    }

    virtual bool setIris(double v) {
        return setCommand(VOCAB_IRIS,v);
    }
    virtual double getIris() {
        return getCommand(VOCAB_IRIS);
    }

    /*
    virtual bool setTemperature(double v) {
        return setCommand(VOCAB_TEMPERATURE,v);
    }
    virtual double getTemperature() const {
        return getCommand(VOCAB_TEMPERATURE);
    }
    
    virtual bool setWhiteShading(double r,double g,double b) {
        return setCommand(VOCAB_WHITE_SHADING,r,g,b);
    }
    virtual bool getWhiteShading(double &r,double &g,double &b) const {
        return getCommand(VOCAB_WHITE_SHADING,r,g,b);
    }
  
    virtual bool setOpticalFilter(double v) {
        return setCommand(VOCAB_OPTICAL_FILTER,v);
    }
    virtual double getOpticalFilter() const {
        return getCommand(VOCAB_OPTICAL_FILTER);
    }

	virtual bool setCaptureQuality(double v) {
        return setCommand(VOCAB_CAPTURE_QUALITY,v);
    }
    virtual double getCaptureQuality() const {
        return getCommand(VOCAB_CAPTURE_QUALITY);
    }
    */

protected:
    yarp::os::Port port;
    yarp::os::PortReaderBuffer<yarp::sig::ImageOf<yarp::sig::PixelRgb> > reader;
    yarp::os::ConstString remote;
    yarp::os::ConstString local;
    yarp::os::Semaphore mutex;
    int lastHeight;
    int lastWidth;
    
    bool setCommand(int code, double v) {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        cmd.addDouble(v);
        port.write(cmd,response);
        return true;
    }

	bool setCommand(int code, double b, double r) {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        cmd.addDouble(b);
		cmd.addDouble(r);
        port.write(cmd,response);
        return true;
    }

    double getCommand(int code) const {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(code);
        port.write(cmd,response);
        // response should be [cmd] [name] value
        return response.get(2).asDouble();
    }

	bool getCommand(int code, double &b, double &r) const
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_GET);
        cmd.addVocab(code);
        port.write(cmd,response);
        // response should be [cmd] [name] value
        b=response.get(2).asDouble();
        r=response.get(3).asDouble();
        return true;
    }
};

#endif
