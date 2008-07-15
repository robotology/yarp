// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#ifndef _YARP2_SERVERFRAMEGRABBER_
#define _YARP2_SERVERFRAMEGRABBER_

#include <stdio.h>

#include <yarp/dev/DataSource.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/AudioGrabberInterfaces.h>
#include <yarp/dev/AudioVisualInterfaces.h>
#include <yarp/dev/ServiceInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/RateThread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>

namespace yarp {
    namespace dev {
        class ServerFrameGrabber;
    }
}

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
 
#define VOCAB_SET VOCAB3('s','e','t')
#define VOCAB_GET VOCAB3('g','e','t')
#define VOCAB_IS VOCAB2('i','s')
#define VOCAB_WIDTH VOCAB1('w')
#define VOCAB_HEIGHT VOCAB1('h')

/**
 * @ingroup dev_impl_wrapper
 *
 * Export a frame grabber to the network.  Provides the 
 * IFrameGrabberImage, IFrameGrabberControls, and IAudioGrabberSound
 * interfaces.  The corresponding client is a RemoteFrameGrabber.
 *
 * The network interface is a single Port.
 * Images are streamed out from that Port -- RemoteFrameGrabber
 * uses this stream to provide the IFrameGrabberImage interface.
 * The IFrameGrabberControls functionality is provided via RPC.
 *
 * Here's a command-line example:
 * \verbatim
 [terminal A] yarpdev --device test_grabber --width 8 --height 8 --name /grabber --framerate 30
 [terminal B] yarp read /read
 [terminal C] yarp connect /grabber /read
 [terminal C] echo "[get] [gain]" | yarp rpc /grabber
 \endverbatim
 * The yarpdev line starts a TestFrameGrabber wrapped in a ServerFrameGrabber.
 * Parameters are:
 * --width, --height set the size of the frame in pixels
 * --name portname set the name of the output port
 * --framerate set the frequency (Hz) at which images will be read and boradcast to 
 * the network; if the parameter is not set images are provided at the maximum speed
 * supported by the device. Notice that the maximum frame rate is determined by
 * the device.
 * 
 * After the "yarp connect" line, image descriptions will show up in 
 * terminal B (you could view them with the yarpview application).
 * The "yarp rpc" command should query the gain (0.0 for the test grabber).
 *
 * <TABLE>
 * <TR><TD> Command (text form) </TD><TD> Response </TD><TD> Code equivalent </TD></TR>
 * <TR><TD> [set] [bri] 1.0 </TD><TD> none </TD><TD> setBrightness() </TD></TR>
 * <TR><TD> [set] [gain] 1.0 </TD><TD> none </TD><TD> setGain() </TD></TR>
 * <TR><TD> [set] [shut] 1.0 </TD><TD> none </TD><TD> setShutter() </TD></TR>
 * <TR><TD> [get] [bri] </TD><TD> [is] [bri] 1.0 </TD><TD> getBrightness() </TD></TR>
 * <TR><TD> [get] [gain] </TD><TD> [is] [gain] 1.0 </TD><TD> getGain() </TD></TR>
 * <TR><TD> [get] [shut] </TD><TD> [is] [shut] 1.0 </TD><TD> getShutter() </TD></TR>
 * </TABLE>
 *
 */
class yarp::dev::ServerFrameGrabber : public DeviceDriver, 
            public DeviceResponder,
            public IFrameGrabberImage,
            public IAudioGrabberSound,
            public IAudioVisualGrabber,
            public IFrameGrabberControls,
            public IService,
            public DataSource<yarp::sig::ImageOf<yarp::sig::PixelRgb> >,
            public DataSource<yarp::sig::Sound>,
            public DataSource<ImageRgbSound>,
            public DataSource2<yarp::sig::ImageOf<yarp::sig::PixelRgb>,yarp::sig::Sound>
{
private:
    yarp::os::Port p;
    yarp::os::Port *p2;
    yarp::os::RateThreadWrapper thread;
    PolyDriver poly;
    IFrameGrabberImage *fgImage;
    IAudioGrabberSound *fgSound;
    IAudioVisualGrabber *fgAv;
    IFrameGrabberControls *fgCtrl;
	IPreciselyTimed *fgTimed;
    bool spoke; // location of this variable tickles bug on Solaris/gcc3.2
    bool canDrop;
    bool addStamp;
    bool active;
    bool singleThreaded;
public:
    /**
     * Constructor.
     */
    ServerFrameGrabber();
    
    virtual bool close() {
        if (!active) {
            return false;
        }
        active = false;
        thread.stop();
        if (p2!=NULL) {
            delete p2;
            p2 = NULL;
        }
        return true;
    }
    
    /**
     * Configure with a set of options. These are:
     * <TABLE>
     * <TR><TD> subdevice </TD><TD> Common name of device to wrap (e.g. "test_grabber"). </TD></TR>
     * <TR><TD> name </TD><TD> Port name to assign to this server (default /grabber). </TD></TR>
     * </TABLE>
     *
     * @param config The options to use
     * @return true iff the object could be configured.
     */
    virtual bool open(yarp::os::Searchable& config);

    //virtual bool read(ConnectionReader& connection);

    virtual bool respond(const yarp::os::Bottle& command, 
                         yarp::os::Bottle& reply);

    bool getDatum(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) {
        return getImage(image);
    }
    
    virtual bool getDatum(yarp::sig::Sound& sound) {
        return getSound(sound);
    }

    virtual bool getDatum(ImageRgbSound& imageSound) {
        return getDatum(imageSound.head,imageSound.body);
    }
    
    virtual bool getDatum(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image,
                          yarp::sig::Sound& sound) {
        return getAudioVisual(image,sound);
    }

    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) {
        if (fgImage==NULL) { return false; }
        return fgImage->getImage(image);
    }
    
    virtual bool getSound(yarp::sig::Sound& sound) {
        if (fgSound==NULL) { return false; }
        return fgSound->getSound(sound);
    }
    
    virtual bool getAudioVisual(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image,
                                yarp::sig::Sound& sound) {
        if (fgAv==NULL) { return false; }
        return fgAv->getAudioVisual(image,sound);
    }

    virtual int height() const {
        if (fgImage==NULL) { return 0; }
        return fgImage->height();
    }

    virtual int width() const {
        if (fgImage==NULL) { return 0; }
        return fgImage->width();
    }

// set
	virtual bool setBrightness(double v) {
        if (fgCtrl==NULL) { return false; }
        return fgCtrl->setBrightness(v);
    }
    virtual bool setExposure(double v)
    {
        if (fgCtrl==NULL) { return false; }
        return fgCtrl->setExposure(v);
    }	
	virtual bool setSharpness(double v) {
        if (fgCtrl==NULL) { return false; }
        return fgCtrl->setSharpness(v);
    }
	virtual bool setWhiteBalance(double blue, double red) {
        if (fgCtrl==NULL) { return false; }
        return fgCtrl->setWhiteBalance(blue,red);
    }
	virtual bool setHue(double v) {
        if (fgCtrl==NULL) { return false; }
        return fgCtrl->setHue(v);
    }	
	virtual bool setSaturation(double v) {
        if (fgCtrl==NULL) { return false; }
        return fgCtrl->setSaturation(v);
    }
	virtual bool setGamma(double v) {
        if (fgCtrl==NULL) { return false; }
        return fgCtrl->setGamma(v);
    }
    virtual bool setShutter(double v) {
        if (fgCtrl==NULL) { return false; }
        return fgCtrl->setShutter(v);
    }
    virtual bool setGain(double v) {
        if (fgCtrl==NULL) { return false; }
        return fgCtrl->setGain(v);
    }
    virtual bool setIris(double v) {
        if (fgCtrl==NULL) { return false; }
        return fgCtrl->setIris(v);
    }
    
    /*
    virtual bool setTemperature(double v) {
        if (fgCtrl==NULL) { return false; }
        return fgCtrl->setTemperature(v);
    }
    virtual bool setWhiteShading(double r,double g,double b) {
        if (fgCtrl==NULL) { return false; }
        return fgCtrl->setWhiteShading(r,g,b);
    }
    virtual bool setOpticalFilter(double v) {
        if (fgCtrl==NULL) { return false; }
        return fgCtrl->setOpticalFilter(v);
    }
    virtual bool setCaptureQuality(double v) {
        if (fgCtrl==NULL) { return false; }
        return fgCtrl->setCaptureQuality(v);
    }
    */

// get

	virtual double getBrightness() const {
        if (fgCtrl==NULL) { return 0.0; }
        return fgCtrl->getBrightness();
    }
	virtual double getExposure() const {
        if (fgCtrl==NULL) { return false; }
        return fgCtrl->getExposure();
    }
	virtual double getSharpness() const {
        if (fgCtrl==NULL) { return 0.0; }
        return fgCtrl->getSharpness();
    }
    virtual bool getWhiteBalance(double &blue, double &red) const {
        if (fgCtrl==NULL) { return false; }
        return fgCtrl->getWhiteBalance(blue,red);
    }
	virtual double getHue() const {
        if (fgCtrl==NULL) { return 0.0; }
        return fgCtrl->getHue();
    }	
	virtual double getSaturation() const {
        if (fgCtrl==NULL) { return 0.0; }
        return fgCtrl->getSaturation();
    }
	virtual double getGamma() const {
        if (fgCtrl==NULL) { return 0.0; }
        return fgCtrl->getGamma();
    }
    virtual double getShutter() const {
        if (fgCtrl==NULL) { return 0.0; }
        return fgCtrl->getShutter();
    }
    virtual double getGain() const {
        if (fgCtrl==NULL) { return 0.0; }
        return fgCtrl->getGain();
    }
    virtual double getIris() const {
        if (fgCtrl==NULL) { return 0.0; }
        return fgCtrl->getIris();
    }
    
    /*
    virtual double getTemperature() const {
        if (fgCtrl==NULL) { return 0.0; }
        return fgCtrl->getTemperature();
    }
    virtual bool getWhiteShading(double &r, double &g, double &b) const {
        if (fgCtrl==NULL) { return false; }
        return fgCtrl->getWhiteShading(r,g,b);
    }
    virtual double getOpticalFilter() const {
        if (fgCtrl==NULL) { return 0.0; }
        return fgCtrl->getOpticalFilter();
    }
    virtual double getCaptureQuality() const {
        if (fgCtrl==NULL) { return false; }
        return fgCtrl->getCaptureQuality();
    }
    */    

    virtual bool startService();

    virtual bool stopService() {
        return close();
    }

    virtual bool updateService();
};

#endif
