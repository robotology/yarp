/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#ifndef YARP2_SERVERFRAMEGRABBER
#define YARP2_SERVERFRAMEGRABBER

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
            public IFrameGrabberControls2,
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
    IFrameGrabberControls  *fgCtrl;
    IFrameGrabberControls2 *fgCtrl2;
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

    // IFrameGrabberControl2 interface //

    virtual bool getCameraDescription(CameraDescriptor *camera);
    virtual bool hasFeature(int feature, bool *hasFeature);
    virtual bool setFeature(int feature, double  values);
    virtual bool getFeature(int feature, double *values);
    virtual bool setFeature(int feature, double  value1, double  value2);
    virtual bool getFeature(int feature, double *value1, double *value2);
    virtual bool hasOnOff(int feature, bool *HasOnOff);
    virtual bool setActive(int feature, bool onoff);
    virtual bool getActive(int feature, bool *isActive);
    virtual bool hasAuto(int feature, bool *hasAuto);
    virtual bool hasManual(int feature, bool *hasManual);
    virtual bool hasOnePush(int feature, bool *hasOnePush);
    virtual bool setMode(int feature, FeatureMode mode);
    virtual bool getMode(int feature, FeatureMode *mode);

    /**
     * Set the requested feature to a value (saturation, brightness ... )
     * @param feature the identifier of the feature to change
     * @param value new value of the feature, from 0 to 1 as a percentage of param range
     * @return returns true on success, false on failure.
     */
    virtual bool setOnePush(int feature);

private:
    bool respondToFrameGrabberControl2(const yarp::os::Bottle& cmd, yarp::os::Bottle& response);
};

#endif
