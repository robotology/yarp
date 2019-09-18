/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_SERVERGRABBER_H
#define YARP_DEV_SERVERGRABBER_H

#include <cstdio>

#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/FrameGrabberControlImpl.h>
#include <yarp/dev/AudioGrabberInterfaces.h>
#include <yarp/dev/AudioVisualInterfaces.h>
#include <yarp/dev/ServiceInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>
#include <yarp/dev/IVisualParamsImpl.h>
#include <yarp/dev/Wrapper.h>
namespace yarp {
    namespace dev {
        class ServerGrabber;
        namespace DC1394 {
            class DC1394Parser;
        }
        namespace impl {
            class ServerGrabberResponder;
        }
    }
}

class YARP_dev_API yarp::dev::DC1394::DC1394Parser:    public DeviceResponder
{
private:
    yarp::dev::IFrameGrabberControlsDC1394  *fgCtrl_DC1394;

public:
    DC1394Parser();
    virtual ~DC1394Parser() {};
    bool configure(yarp::dev::IFrameGrabberControlsDC1394 *interface);
    bool respond(const yarp::os::Bottle& cmd, yarp::os::Bottle& response) override;
};

class yarp::dev::impl::ServerGrabberResponder :public DeviceResponder
{
private:
    bool left;
    yarp::dev::ServerGrabber* server;
public:
    ServerGrabberResponder(bool _left=false);
    ~ServerGrabberResponder();
    bool configure(yarp::dev::ServerGrabber* _server);
    bool respond(const os::Bottle &command, os::Bottle &reply) override;
};

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
    bool splitterMode;
    bool hasAudio;
    Capabilities cap;

} Configuration;

#define DEFAULT_THREAD_PERIOD   0.03 //s


/**
 * @ingroup dev_impl_wrapper
 *
 * \section ServerGrabber Description of input parameters
 * A Network grabber for camera devices.
 * In base of the configuration this device can handle one or two cameras.\n
 * In case of two cameras, the RGB or RAW streaming will be produced on two separated ports or on a single port with the two images
 * stitched horizontally.\n
 * Moreover it has two rpc ports that have the same name of the streaming ports + "/rpc" suffix.\n
 * The inheritance from yarp::dev::IWrapper and yarp::dev::IMultipleWrapper allows to be instantiated also through yarprobotinterface.
 * See their documentation for more details about each interface.
 *
 * This device is paired with its client called RemoteFrameGrabber to receive the data streams and perform remote operations.
 *
 *   Parameters required by this device are:
 * | Parameter name | SubParameter            | Type    | Units          | Default Value | Required                                                                                                                                  | Description                                                                                                  | Notes |
 * |:--------------:|:-----------------------:|:-------:|:--------------:|:-------------:|:-----------------------------------------------------------------------------------------------------------------------------------------:|:------------------------------------------------------------------------------------------------------------:|:-----:|
 * | period         |      -                  | int     | ms             |   30          | No                                                                                                                                        | refresh period(in ms) of the broadcasted values through yarp ports                                                                | default 30ms |
 * | name           |      -                  | string  | -              |   /grabber    | No                                                                                                                                        | Prefix name of the ports opened by the ServerGrabber                                                         | Required suffix like '/rpc' will be added by the device      |
 * | capabilities   |      -                  | string  | -              |   COLOR       | No                                                                                                                                        | two capabilities supported, COLOR and RAW respectively for rgb and raw streaming                             | - |
 * | twoCameras     |      -                  | bool    | -              |   -           | required only for instantiating this device with the yarprobotinterface                                                                   | if true ServerGrabber will open and handle two devices, if false only one                                    | =true it makes required left_config and right_config parameters, =false subdevice parameter becomes required|
 * | split          |      -                  | bool    | -              |   false       | No                                                                                                                                        | set 'true' to split the streaming of the input camera/s on two different ports                               | Useful in case of dual cameras, the image will be split vertically in the 2 output ports 'left' and 'right' |
 * | subdevice      |      -                  | string  | -              |   -           | used for opening and handling a single device, alternative to 'attach' action                                                             | name of the subdevice to use as a data source                                                                | when used, parameters for the subdevice must be provided as well  |
 * | left_config    |      -                  | string  | -              |   -           | used for opening and handling two devices, required if right_config present and/or twoCameras set to true                                 | name of the ini file containing the configuration of one of two subdevices to use as a data source           | when used, parameters for the subdevice must be provided in the file specified. This parameter is not admitted if the device is configured for working with one device. |
 * | right_config   |      -                  | string  | -              |   -           | used for opening and handling two devices, required if left_config present and/or twoCameras set to true                                  | name of the ini file containing the configuration of one of two subdevices to use as a data source           | when used, parameters for the subdevice must be provided in the file specified. This parameter is not admitted if the device is configured for working with one device. |
 * | context        |      -                  | string  | -              |   -           | No                                                                                                                                        | name of context where the yarp::os::ResourceFinder will search the ini files specified in left_config and right_config                          | - |
 *
 * Some example of configuration files:
 *
 * Example of configuration file using .ini format, for handling two cameras.
 *
 * \code{.unparsed}
 * device grabberDual
 * capabilities RAW
 * split false
 * period 30
 * twoCameras true
 * left_config  left_config.ini    #name of the config file containing the parameters for the camera device
 * right_config right_config.ini   #name of the config file containing the parameters for the camera device
 * \endcode
 *
 * Example of configuration file using .ini format, for handling one camera.
 *
 * \code{.unparsed}
 * device grabberDual
 * capabilities COLOR        # not necessary to specify 'COLOR', this is the default value if capabilities is omitted
 * period 30
 * subdevice test_grabber
 * \endcode
 *
 * Example of configuration file using .xml format, for handling two cameras.
 *
 * \code{.xml}
 * <device name="serverGrabber" type="grabberDual">
 *  <param name="period"> 30 </param>
 *  <param name="name">   /grabber  </param>
 *  <param name="capabilities">  RGB        </param>
 *  <param name="split"> false       </param>
 *  <param name="twoCameras"> true </param>
 *
 *  <action phase="startup" level="5" type="attach">
 *    <paramlist name="networks">
 *      <elem name="LEFT">  testCamera_left </elem>
 *      <elem name="RIGHT">  testCamera_right </elem>
 *    </paramlist>
 *  </action>
 *  <action phase="shutdown" level="5" type="detach" />
 * </device>
 * \endcode
 *
 * Example of configuration file using .xml format, for handling one camera.
 *
 * \code{.xml}
 * <device name="serverGrabber" type="grabberDual">
 *  <param name="period"> 30 </param>
 *  <param name="name">   /grabber  </param>
 *  <param name="capabilities">  RGB        </param>
 *  <param name="split"> false       </param>
 *  <param name="twoCameras"> true </param>
 *
 *  <action phase="startup" level="5" type="attach">
 *    <paramlist name="networks">
 *      <elem name="subdevice">  usbCamera_left_single </elem>
 *    </paramlist>
 *  </action>
 *  <action phase="shutdown" level="5" type="detach" />
 * </device>
 * \endcode
 *
 */
class YARP_dev_API yarp::dev::ServerGrabber : public DeviceDriver,
            public yarp::dev::IWrapper,
            public yarp::dev::IMultipleWrapper,
            public yarp::os::PeriodicThread
{
private:
    double period;
    int count, count2;
    yarp::dev::impl::ServerGrabberResponder* responder;
    yarp::dev::impl::ServerGrabberResponder* responder2;
    yarp::dev::Implement_RgbVisualParams_Parser  rgbParser;
    yarp::dev::Implement_RgbVisualParams_Parser  rgbParser2;
    yarp::dev::IRgbVisualParams* rgbVis_p;
    yarp::dev::IRgbVisualParams* rgbVis_p2;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) rpcPort_Name;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) rpcPort2_Name;
    yarp::os::Port rpcPort;
    yarp::os::Port rpcPort2;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) pImg_Name;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) pImg2_Name;
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
    yarp::dev::IFrameGrabberControls *fgCtrl;
    yarp::dev::IFrameGrabberControls *fgCtrl2;
    yarp::dev::IFrameGrabberControlsDC1394* fgCtrl_DC1394;
    yarp::dev::IFrameGrabberControlsDC1394* fgCtrl2_DC1394;
    yarp::dev::IPreciselyTimed *fgTimed;
    yarp::dev::FrameGrabberControls_Parser ifgCtrl_Parser;
    yarp::dev::FrameGrabberControls_Parser ifgCtrl2_Parser;
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

    // If a subdevice parameter is given, the wrapper will open it(or them) and attach to immediately.
    // Typical usage: simulator or command line
    bool                           isSubdeviceOwned;
    bool                           openAndAttachSubDevice(yarp::os::Searchable& prop);

public:
    /**
     * Constructor.
     */
    ServerGrabber();

    ~ServerGrabber();

    //DeviceDriver
    bool close() override;
    /**
     * Configure with a set of options.
     * @param config The options to use
     * @return true iff the object could be configured.
     */
    bool open(yarp::os::Searchable& config) override;

    //bool read(ConnectionReader& connection) override;

    //DeviceResponder
    bool respond(const yarp::os::Bottle& command,
                         yarp::os::Bottle& reply, bool left, bool both);
    // IMultipleWrapper interface
    bool        attachAll(const PolyDriverList &device2attach) override;

    bool        detachAll() override;

    // IWrapper interface
    bool        attach(PolyDriver *poly) override;

    bool        detach() override;

    //RateThread
    bool threadInit() override;

    void threadRelease() override;

    void run() override;
protected:

    bool fromConfig(yarp::os::Searchable &config);

    bool initialize_YARP(yarp::os::Searchable &params);

    void stopThread();

    void setupFlexImage(const yarp::sig::Image& img, yarp::sig::FlexImage& flex_i);

    void shallowCopyImages(const yarp::sig::FlexImage& src, yarp::sig::FlexImage& dest);

    void cleanUp();
};

#endif // YARP_DEV_SERVERGRABBER_H
