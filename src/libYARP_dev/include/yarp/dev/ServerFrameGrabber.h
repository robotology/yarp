// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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

#define VOCAB_DRHASFEA VOCAB4('D','R','2','a') // 00
#define VOCAB_DRSETVAL VOCAB4('D','R','2','b') // 01
#define VOCAB_DRGETVAL VOCAB4('D','R','2','c') // 02

#define VOCAB_DRHASACT VOCAB4('D','R','2','d') // 03
#define VOCAB_DRSETACT VOCAB4('D','R','2','e') // 04
#define VOCAB_DRGETACT VOCAB4('D','R','2','f') // 05

#define VOCAB_DRHASMAN VOCAB4('D','R','2','g') // 06
#define VOCAB_DRHASAUT VOCAB4('D','R','2','h') // 07
#define VOCAB_DRHASONP VOCAB4('D','R','2','i') // 08
#define VOCAB_DRSETMOD VOCAB4('D','R','2','j') // 09
#define VOCAB_DRGETMOD VOCAB4('D','R','2','k') // 10
#define VOCAB_DRSETONP VOCAB4('D','R','2','l') // 11

// masks
#define VOCAB_DRGETMSK VOCAB4('D','R','2','m') // 12
#define VOCAB_DRGETVMD VOCAB4('D','R','2','n') // 13
#define VOCAB_DRSETVMD VOCAB4('D','R','2','o') // 14

#define VOCAB_DRGETFPM VOCAB4('D','R','2','p') // 15
#define VOCAB_DRGETFPS VOCAB4('D','R','2','q') // 16
#define VOCAB_DRSETFPS VOCAB4('D','R','2','r') // 17

#define VOCAB_DRGETISO VOCAB4('D','R','2','s') // 18
#define VOCAB_DRSETISO VOCAB4('D','R','2','t') // 19

#define VOCAB_DRGETCCM VOCAB4('D','R','2','u') // 20
#define VOCAB_DRGETCOD VOCAB4('D','R','2','v') // 21
#define VOCAB_DRSETCOD VOCAB4('D','R','2','w') // 22

#define VOCAB_DRSETWHB VOCAB4('D','R','2','x') // 23
#define VOCAB_DRGETWHB VOCAB4('D','R','2','y') // 24

#define VOCAB_DRGETF7M VOCAB4('D','R','2','z') // 25
#define VOCAB_DRGETWF7 VOCAB4('D','R','2','A') // 26
#define VOCAB_DRSETWF7 VOCAB4('D','R','2','B') // 27

#define VOCAB_DRSETOPM VOCAB4('D','R','2','C') // 28
#define VOCAB_DRGETOPM VOCAB4('D','R','2','D') // 29
#define VOCAB_DRSETTXM VOCAB4('D','R','2','E') // 30
#define VOCAB_DRGETTXM VOCAB4('D','R','2','F') // 31
//#define VOCAB_DRSETBAY VOCAB4('D','R','2','G') // 32
//#define VOCAB_DRGETBAY VOCAB4('D','R','2','H') // 33

#define VOCAB_DRSETBCS VOCAB4('D','R','2','I') // 34
#define VOCAB_DRSETDEF VOCAB4('D','R','2','J') // 35
#define VOCAB_DRSETRST VOCAB4('D','R','2','K') // 36
#define VOCAB_DRSETPWR VOCAB4('D','R','2','L') // 37

#define VOCAB_DRSETCAP VOCAB4('D','R','2','M') // 38
#define VOCAB_DRSETBPP VOCAB4('D','R','2','N') // 39
#define VOCAB_DRGETBPP VOCAB4('D','R','2','O') // 40

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
class YARP_dev_API yarp::dev::ServerFrameGrabber : public DeviceDriver,
            public DeviceResponder,
            public IFrameGrabberImage,
            public IAudioGrabberSound,
            public IAudioVisualGrabber,
            public IFrameGrabberControls,
            public IService,
            public DataSource<yarp::sig::ImageOf<yarp::sig::PixelRgb> >,
            public DataSource<yarp::sig::ImageOf<yarp::sig::PixelMono> >,
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
    IFrameGrabberImageRaw *fgImageRaw;
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

    bool getDatum(yarp::sig::ImageOf<yarp::sig::PixelMono>& image) {
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

    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image) {
        if (fgImageRaw==NULL) { return false; }
        return fgImageRaw->getImage(image);
    }

    virtual bool getSound(yarp::sig::Sound& sound) {
        if (fgSound==NULL) { return false; }
        return fgSound->getSound(sound);
    }

    virtual bool startRecording() {
        if (fgSound==NULL) { return false; }
        return fgSound->startRecording();
    }

    virtual bool stopRecording() {
        if (fgSound==NULL) { return false; }
        return fgSound->stopRecording();
    }

    virtual bool getAudioVisual(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image,
                                yarp::sig::Sound& sound) {
        if (fgAv==NULL) { return false; }
        return fgAv->getAudioVisual(image,sound);
    }

    virtual int height() const {
        if (fgImage) { return fgImage->height(); }
        if (fgImageRaw) { return fgImageRaw->height(); }
        return 0;
    }

    virtual int width() const {
        if (fgImage) { return fgImage->width(); }
        if (fgImageRaw) { return fgImageRaw->width(); }
        return 0;
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

    virtual double getBrightness() {
        if (fgCtrl==NULL) { return 0.0; }
        return fgCtrl->getBrightness();
    }
    virtual double getExposure() {
        if (fgCtrl==NULL) { return false; }
        return fgCtrl->getExposure();
    }
    virtual double getSharpness() {
        if (fgCtrl==NULL) { return 0.0; }
        return fgCtrl->getSharpness();
    }
    virtual bool getWhiteBalance(double &blue, double &red) {
        if (fgCtrl==NULL) { return false; }
        return fgCtrl->getWhiteBalance(blue,red);
    }
    virtual double getHue() {
        if (fgCtrl==NULL) { return 0.0; }
        return fgCtrl->getHue();
    }
    virtual double getSaturation() {
        if (fgCtrl==NULL) { return 0.0; }
        return fgCtrl->getSaturation();
    }
    virtual double getGamma() {
        if (fgCtrl==NULL) { return 0.0; }
        return fgCtrl->getGamma();
    }
    virtual double getShutter() {
        if (fgCtrl==NULL) { return 0.0; }
        return fgCtrl->getShutter();
    }
    virtual double getGain() {
        if (fgCtrl==NULL) { return 0.0; }
        return fgCtrl->getGain();
    }
    virtual double getIris() {
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
