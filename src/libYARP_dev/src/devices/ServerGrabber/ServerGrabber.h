/*
 * Copyright (C) 2017 iCub Facility
 * Authors: Nicolo' Genesio
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARP_DEV_SERVERGRABBER_H
#define YARP_DEV_SERVERGRABBER_H

#include <stdio.h>

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
#include <yarp/os/RateThread.h>
#include <yarp/dev/Wrapper.h>
namespace yarp {
    namespace dev {
        class ServerGrabber;
        namespace DC1394 {
        class DC1394Parser;
        }
    }
}

typedef enum {
//    AV,
    COLOR,
    RAW,
} Capabilities;

typedef struct
{
    bool spoke; // location of this variable tickles bug on Solaris/gcc3.2
    bool canDrop;
    bool addStamp;
    bool active;
    bool singleThreaded;
    bool twoCameras;
    bool split;
    bool hasAudio;
    Capabilities cap;

} Configuration;

#define DEFAULT_THREAD_PERIOD   30 //ms

class yarp::dev::DC1394::DC1394Parser:    public DeviceResponder
{
private:
    yarp::dev::IFrameGrabberControlsDC1394  *fgCtrl_DC1394;

public:
    DC1394Parser();
    virtual ~DC1394Parser() {};
    bool configure(yarp::dev::IFrameGrabberControlsDC1394 *interface);
    virtual bool respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response);
};

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
 * The yarpdev line starts a TestFrameGrabber wrapped in a ServerGrabber.
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
class YARP_dev_API yarp::dev::ServerGrabber : public DeviceDriver,
            public DeviceResponder,
            public yarp::dev::IWrapper,
            public yarp::dev::IMultipleWrapper,
            public yarp::os::RateThread
{
private:
    int period;
    int count, count2;
    yarp::dev::Implement_RgbVisualParams_Parser  rgbParser;
    yarp::dev::Implement_RgbVisualParams_Parser  rgbParser2;
    yarp::dev::IRgbVisualParams* rgbVis_p;
    yarp::dev::IRgbVisualParams* rgbVis_p2;
    yarp::os::ConstString rpcPort_Name;
    yarp::os::Port rpcPort;
    yarp::os::ConstString pImg_Name;
    yarp::os::ConstString pImg2_Name;
    yarp::os::BufferedPort<yarp::sig::FlexImage> pImg;
    yarp::os::BufferedPort<yarp::sig::FlexImage> pImg2;
    yarp::os::Port *p2;//audio
    yarp::dev::PolyDriver* poly; //subDeviceOwned convert to pointer
    yarp::dev::PolyDriver* poly2;
    yarp::dev::IFrameGrabberImage *fgImage;
    yarp::dev::IFrameGrabberImage *fgImage2;
    yarp::dev::IFrameGrabberImageRaw *fgImageRaw;
    yarp::dev::IFrameGrabberImageRaw *fgImageRaw2;
    //yarp::sig::FlexImage  doubleImage, doubleImage2;
//    IAudioVisualGrabber *fgAv; //TODO: manage the AV
    yarp::dev::IFrameGrabberControls2 *fgCtrl;
    yarp::dev::IFrameGrabberControls2 *fgCtrl2;
    yarp::dev::IFrameGrabberControlsDC1394* fgCtrl_DC1394;
    yarp::dev::IFrameGrabberControlsDC1394* fgCtrl2_DC1394;
    yarp::dev::IPreciselyTimed *fgTimed;
    yarp::dev::FrameGrabberControls2_Parser ifgCtrl_Parser;
    yarp::dev::FrameGrabberControls2_Parser ifgCtrl2_Parser;
    yarp::dev::DC1394::DC1394Parser ifgCtrl_DC1394_Parser;
    yarp::dev::DC1394::DC1394Parser ifgCtrl2_DC1394_Parser;
    Configuration param;
    yarp::sig::ImageOf<yarp::sig::PixelRgb>* img;
    yarp::sig::ImageOf<yarp::sig::PixelRgb>* img2;
    yarp::sig::ImageOf<yarp::sig::PixelMono>* img_Raw;
    yarp::sig::ImageOf<yarp::sig::PixelMono>* img2_Raw;

    // Open the wrapper only, the attach method needs to be called before using it
    // Typical usage: yarprobotinterface
    bool openDeferredAttach(yarp::os::Searchable& prop);

    // If a subdevice parameter is given, the wrapper will open it(or them) and attach to immediatly.
    // Typical usage: simulator or command line
    bool                           isSubdeviceOwned;
    bool                           openAndAttachSubDevice(yarp::os::Searchable& prop);

public:
    /**
     * Constructor.
     */
    ServerGrabber();

    //DeviceDriver
    virtual bool close();
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

    //DeviceResponder
    virtual bool respond(const yarp::os::Bottle& command,
                         yarp::os::Bottle& reply);
    // IMultipleWrapper interface
    bool        attachAll(const PolyDriverList &device2attach);

    bool        detachAll();

    // IWrapper interface
    bool        attach(PolyDriver *poly);

    bool        detach();

    //RateThread
    bool threadInit();

    void threadRelease();

    void run();
protected:
    bool fromConfig(yarp::os::Searchable &config);

    bool initialize_YARP(yarp::os::Searchable &params);

    void shallowCopyImages(const yarp::sig::FlexImage& src, yarp::sig::FlexImage& dest);
};

#endif // YARP_DEV_SERVERGRABBER_H
