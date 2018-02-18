/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARP_DEV_SERVERFRAMEGRABBER_H
#define YARP_DEV_SERVERFRAMEGRABBER_H

#include <cstdio>

#include <yarp/dev/DataSource.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/FrameGrabberControl2Impl.h>
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
#include <yarp/dev/IVisualParamsImpl.h>
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
            public IService,
            public DataSource<yarp::sig::ImageOf<yarp::sig::PixelRgb> >,
            public DataSource<yarp::sig::ImageOf<yarp::sig::PixelMono> >,
            public DataSource<yarp::sig::Sound>,
            public DataSource<ImageRgbSound>,
            public DataSource2<yarp::sig::ImageOf<yarp::sig::PixelRgb>,yarp::sig::Sound>
{
private:
    yarp::dev::Implement_RgbVisualParams_Parser  rgbParser;
    yarp::dev::IRgbVisualParams* rgbVis_p;
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

    FrameGrabberControls2_Parser ifgCtrl2_Parser;

public:
    /**
     * Constructor.
     */
    ServerFrameGrabber();

    virtual bool close() override;
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
    virtual bool open(yarp::os::Searchable& config) override;

    //virtual bool read(ConnectionReader& connection) override;

    virtual bool respond(const yarp::os::Bottle& command,
                         yarp::os::Bottle& reply) override;

    bool getDatum(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) override;

    bool getDatum(yarp::sig::ImageOf<yarp::sig::PixelMono>& image) override;

    virtual bool getDatum(yarp::sig::Sound& sound) override;

    virtual bool getDatum(ImageRgbSound& imageSound) override;

    virtual bool getDatum(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image,
                          yarp::sig::Sound& sound) override;

    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) override;

    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image);

    virtual bool getSound(yarp::sig::Sound& sound) override;

    virtual bool startRecording() override;

    virtual bool stopRecording() override;

    virtual bool getAudioVisual(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image,
                                yarp::sig::Sound& sound) override;

    virtual int height() const override;

    virtual int width() const override;

// set
    virtual bool setBrightness(double v) override;

    virtual bool setExposure(double v) override;

    virtual bool setSharpness(double v) override;

    virtual bool setWhiteBalance(double blue, double red) override;

    virtual bool setHue(double v) override;

    virtual bool setSaturation(double v) override;

    virtual bool setGamma(double v) override;

    virtual bool setShutter(double v) override;

    virtual bool setGain(double v) override;

    virtual bool setIris(double v) override;

// get

    virtual double getBrightness() override;

    virtual double getExposure() override;

    virtual double getSharpness() override;

    virtual bool getWhiteBalance(double &blue, double &red) override;

    virtual double getHue() override;

    virtual double getSaturation() override;

    virtual double getGamma() override;

    virtual double getShutter() override;

    virtual double getGain() override;

    virtual double getIris() override;

    virtual bool startService() override;

    virtual bool stopService() override;

    virtual bool updateService() override;
};

#endif // YARP_DEV_SERVERFRAMEGRABBER_H
