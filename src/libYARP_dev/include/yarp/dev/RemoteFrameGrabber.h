// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 The RobotCub Consortium
 * Authors: Paul Fitzpatrick, Alessandro Scalzo
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 */


#ifndef _YARP2_REMOTEFRAMEGRABBER_
#define _YARP2_REMOTEFRAMEGRABBER_

#include <yarp/dev/ServerFrameGrabber.h>
#include <yarp/os/Network.h>
#include <yarp/os/Semaphore.h>


namespace yarp{
    namespace dev {
        class RemoteFrameGrabber;
		class RemoteFrameGrabberDC1394;
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
        yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        cmd.addDouble(v);
        port.write(cmd);
        return true;
    }

	bool setCommand(int code, double b, double r) {
        yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_SET);
        cmd.addVocab(code);
        cmd.addDouble(b);
		cmd.addDouble(r);
        port.write(cmd);
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


class yarp::dev::RemoteFrameGrabberDC1394 : public IFrameGrabberControlsDC1394, public yarp::dev::RemoteFrameGrabber
{
public:
	RemoteFrameGrabberDC1394() : RemoteFrameGrabber(){}

	// 00
	virtual bool hasFeatureDC1394(int feature)
	{
        yarp::os::Bottle cmd,response;
        cmd.addVocab(VOCAB_DRHASFEA);
        cmd.addInt(feature);
        port.write(cmd,response);
		return bool(response.get(0).asInt());
	} 
	// 01
	virtual bool setFeatureDC1394(int feature,double value)
	{
		yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_DRSETVAL);
		cmd.addInt(feature);
		cmd.addDouble(value);
		port.write(cmd);
		return true;
	}
	// 02
	virtual double getFeatureDC1394(int feature)
	{
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETVAL);
        cmd.addInt(feature);
        port.write(cmd,response);
        return response.get(0).asDouble();
	}
	
	// 03
	virtual bool hasOnOffDC1394(int feature)	
	{
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRHASACT);
        cmd.addInt(feature);
        port.write(cmd,response);
		return bool(response.get(0).asInt());
	}    
	// 04
	virtual bool setActiveDC1394(int feature, bool onoff)
	{
		yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_DRSETACT);
		cmd.addInt(feature);
		cmd.addInt(int(onoff));
		port.write(cmd);
		return true;
	}
	// 05
	virtual bool getActiveDC1394(int feature)
	{
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETACT);
        cmd.addInt(feature);
        port.write(cmd,response);
		return bool(response.get(0).asInt());
	}

	// 06
	virtual bool hasManualDC1394(int feature)
	{
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRHASMAN);
        cmd.addInt(feature);
        port.write(cmd,response);
		return bool(response.get(0).asInt());
	}
	// 07
	virtual bool hasAutoDC1394(int feature)
	{
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRHASAUT);
        cmd.addInt(feature);
        port.write(cmd,response);
		return bool(response.get(0).asInt());
	}
	// 08
	virtual bool hasOnePushDC1394(int feature)
	{
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRHASONP);
        cmd.addInt(feature);
        port.write(cmd,response);
		return bool(response.get(0).asInt());
	}
	// 09
	virtual bool setModeDC1394(int feature, bool auto_onoff)
	{
		yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_DRSETMOD);
		cmd.addInt(feature);
		cmd.addInt(int(auto_onoff));
		port.write(cmd);
		return true;
	}
	// 10
	virtual bool getModeDC1394(int feature)
	{
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETMOD);
        cmd.addInt(feature);
        port.write(cmd,response);
		return bool(response.get(0).asInt());
	}
	// 11
	virtual bool setOnePushDC1394(int feature)
	{
		yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_DRSETONP);
		cmd.addInt(feature);
		port.write(cmd);
		return true;
	}

	// 12
	virtual unsigned int getVideoModeMaskDC1394()
	{
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETMSK);
        port.write(cmd,response);
		return response.get(0).asInt();
	}
	// 13
	virtual unsigned int getVideoModeDC1394()
	{
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETVMD);
        port.write(cmd,response);
		return response.get(0).asInt();
	}
	// 14
	virtual bool setVideoModeDC1394(int video_mode)
	{
		yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_DRSETVMD);
		cmd.addInt(video_mode);
		port.write(cmd);
		return true;		
	}

	// 15
	virtual unsigned int getFPSMaskDC1394()
	{
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETFPM);
        port.write(cmd,response);
		return response.get(0).asInt();
	}
	// 16
	virtual unsigned int getFPSDC1394()
	{
		yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETFPS);
        port.write(cmd,response);
		return response.get(0).asInt();
	}
	// 17
	virtual bool setFPSDC1394(int fps)
	{
		yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_DRSETFPS);
		cmd.addInt(fps);
        port.write(cmd);
		return true;
	}

	// 18
	virtual unsigned int getISOSpeedDC1394()
	{
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETISO);
        port.write(cmd,response);
		return response.get(0).asInt();
	}
	// 19
	virtual bool setISOSpeedDC1394(int speed)
	{
		yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_DRSETISO);
		cmd.addInt(speed);
		port.write(cmd);
		return true;
	}

	// 20
	virtual unsigned int getColorCodingMaskDC1394(unsigned int video_mode)
	{
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETCCM);
        cmd.addInt(video_mode);
        port.write(cmd,response);
		return response.get(0).asInt();
	}
	// 21
	virtual unsigned int getColorCodingDC1394()
	{
		yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETCOD);
        port.write(cmd,response);
		return response.get(0).asInt();
	}
	// 22
	virtual bool setColorCodingDC1394(int coding)
	{
		yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_DRSETCOD);
		cmd.addInt(coding);
        port.write(cmd);
		return true;
	}

	// 23
	virtual bool setWhiteBalanceDC1394(double b, double r) {
        yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_DRSETWHB);
        cmd.addDouble(b);
		cmd.addDouble(r);
        port.write(cmd);
        return true;
    }
	// 24
	virtual bool getWhiteBalanceDC1394(double &b, double &r)
    {
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETWHB);
        port.write(cmd,response);
        b=response.get(0).asDouble();
        r=response.get(1).asDouble();
        return true;
    }

	// 25
	virtual bool getFormat7MaxWindowDC1394(unsigned int &xdim,unsigned int &ydim,unsigned int &xstep,unsigned int &ystep)
	{
        yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETF7M);
        port.write(cmd,response);
        
		xdim=response.get(0).asInt();
		ydim=response.get(1).asInt();
		xstep=response.get(2).asInt();
		ystep=response.get(3).asInt();
		return true;
	}
	// 26
	virtual bool getFormat7WindowDC1394(unsigned int &xdim,unsigned int &ydim)
	{
		yarp::os::Bottle cmd, response;
        cmd.addVocab(VOCAB_DRGETWF7);
        port.write(cmd,response);
		xdim=response.get(0).asInt();
		ydim=response.get(1).asInt();
		return true;
	}
	// 27
	virtual bool setFormat7WindowDC1394(unsigned int xdim,unsigned int ydim)
	{
        yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_DRSETWF7);
		cmd.addInt(xdim);
		cmd.addInt(ydim);
        port.write(cmd);
		return true;
	}

	// 28
	virtual bool setOperationModeDC1394(bool b1394b)
	{
        yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_DRSETOPM);
        cmd.addInt(int(b1394b));
        port.write(cmd);
        return true;
	}
	// 29
	virtual bool getOperationModeDC1394()
	{
        yarp::os::Bottle cmd,response;
        cmd.addVocab(VOCAB_DRGETOPM);
        port.write(cmd,response);
        return response.get(0).asInt();
	}

	// 30
	virtual bool setTransmissionDC1394(bool bTxON)
	{
        yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_DRSETTXM);
        cmd.addInt(int(bTxON));
        port.write(cmd);
        return true;
	}
	// 31
	virtual bool getTransmissionDC1394()
	{
        yarp::os::Bottle cmd,response;
        cmd.addVocab(VOCAB_DRGETTXM);
        port.write(cmd,response);
        return response.get(0).asInt();
	}
	/*
	// 32
	virtual bool setBayerDC1394(bool bON)
	{
        yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_DRSETBAY);
        cmd.addInt(int(bON));
        port.write(cmd);
        return true;
	}
	// 33
	virtual bool getBayerDC1394()
	{
        yarp::os::Bottle cmd,response;
        cmd.addVocab(VOCAB_DRGETBAY);
        port.write(cmd,response);
        return response.get(0).asInt();
	}
	*/
	// 34
	virtual bool setBroadcastDC1394(bool onoff)
	{
		yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_DRSETBCS);
		cmd.addInt((int)onoff);
		port.write(cmd);
		return true;
	}
	// 35
	virtual bool setDefaultsDC1394()
	{
		yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_DRSETDEF);
		port.write(cmd);
		return true;
	}
	// 36
	virtual bool setResetDC1394()
	{
		yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_DRSETRST);
		port.write(cmd);
		return true;
	}
	// 37
	virtual bool setPowerDC1394(bool onoff)
	{
		yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_DRSETPWR);
		cmd.addInt((int)onoff);
		port.write(cmd);
		return true;
	}

	// 38
	virtual bool setCaptureDC1394(bool bON)
	{
        yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_DRSETCAP);
        cmd.addInt(int(bON));
        port.write(cmd);
        return true;
	}

	// 39
	virtual bool setBytesPerPacketDC1394(unsigned int bpp)
	{
        yarp::os::Bottle cmd;
        cmd.addVocab(VOCAB_DRSETBPP);
        cmd.addInt(int(bpp));
        port.write(cmd);
        return true;
	}

	// 40
	virtual unsigned int getBytesPerPacketDC1394()
	{
        yarp::os::Bottle cmd,response;
        cmd.addVocab(VOCAB_DRGETBPP);
        port.write(cmd,response);
        return (unsigned int)response.get(0).asInt();
	}


	/////////////////////////////////////////////////


	bool setBrightness(double v) {
        return setCommand(VOCAB_BRIGHTNESS,v);
    }
    double getBrightness() {
        return getCommand(VOCAB_BRIGHTNESS);
    }
	bool setExposure(double v) {
        return setCommand(VOCAB_EXPOSURE,v);
    }
    double getExposure() {
        return getCommand(VOCAB_EXPOSURE);
    }

    bool setSharpness(double v) {
        return setCommand(VOCAB_SHARPNESS,v);
    }
    double getSharpness() {
        return getCommand(VOCAB_SHARPNESS);
    }

	bool setWhiteBalance(double blue, double red)
	{
		return setCommand(VOCAB_WHITE, blue, red);
	}
	bool getWhiteBalance(double &blue, double &red)
	{
		return getCommand(VOCAB_WHITE, blue, red);
	}

    bool setHue(double v) {
        return setCommand(VOCAB_HUE,v);
    }
    double getHue() {
        return getCommand(VOCAB_HUE);
    }

    bool setSaturation(double v) {
        return setCommand(VOCAB_SATURATION,v);
    }
    double getSaturation() {
        return getCommand(VOCAB_SATURATION);
    }

    bool setGamma(double v) {
        return setCommand(VOCAB_GAMMA,v);
    }
    double getGamma() {
        return getCommand(VOCAB_GAMMA);
    }

    bool setShutter(double v) {
        return setCommand(VOCAB_SHUTTER,v);
    }
    double getShutter() {
        return getCommand(VOCAB_SHUTTER);
    }

    bool setGain(double v) {
        return setCommand(VOCAB_GAIN,v);
    }
    double getGain() {
        return getCommand(VOCAB_GAIN);
    }

    bool setIris(double v) {
        return setCommand(VOCAB_IRIS,v);
    }
    double getIris() {
        return getCommand(VOCAB_IRIS);
    }
};

#endif
