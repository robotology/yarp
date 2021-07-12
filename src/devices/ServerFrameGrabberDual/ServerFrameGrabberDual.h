/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_SERVERGRABBER_H
#define YARP_DEV_SERVERGRABBER_H

#include <cstdio>


#include <yarp/dev/IFrameGrabberImage.h>
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
#include <yarp/dev/IWrapper.h>
#include <yarp/dev/IMultipleWrapper.h>

#include <yarp/proto/framegrabber/FrameGrabberControls_Responder.h>
#include <yarp/proto/framegrabber/FrameGrabberControlsDC1394_Responder.h>
#include <yarp/proto/framegrabber/RgbVisualParams_Responder.h>

class ServerGrabber;


class ServerGrabberResponder :
        public yarp::dev::DeviceResponder
{
private:
    bool left{false};
    ServerGrabber* server{nullptr};
public:
    ServerGrabberResponder(bool _left = false);
    ~ServerGrabberResponder() override = default;
    bool configure(ServerGrabber* _server);
    bool respond(const yarp::os::Bottle &command, yarp::os::Bottle &reply) override;
};


enum Capabilities
{
//    AV,
    COLOR,
    RAW,
};


struct Configuration
{
    bool spoke{false}; // location of this variable tickles bug on Solaris/gcc3.2
    bool canDrop{true};
    bool addStamp{false};
    bool active{false};
    bool singleThreaded{false};
    bool twoCameras{false};
    bool split{false};
    bool splitterMode{false};
    bool hasAudio{false};
    Capabilities cap{COLOR};
};


#define DEFAULT_THREAD_PERIOD   0.03 //s


/**
 * @ingroup dev_impl_wrapper
 *
 * \brief `grabberDual`: A Network grabber for camera devices.
 *
 * In base of the configuration this device can handle one or two cameras.\n
 * In case of two cameras, the RGB or RAW streaming will be produced on two separated ports or on a single port with the two images
 * stitched horizontally.\n
 * Moreover it has two rpc ports that have the same name of the streaming ports + "/rpc" suffix.\n
 * The inheritance from yarp::dev::IWrapper and yarp::dev::IMultipleWrapper allows to be instantiated also through yarprobotinterface.
 * See their documentation for more details about each interface.
 *
 * This device is paired with its client called RemoteFrameGrabber to receive the data streams and perform remote operations.
 *
 * \section grabberDual_device_parameters Description of input parameters
 *
 * Parameters required by this device are:
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
 * subdevice fakeFrameGrabber
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
class ServerGrabber :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IWrapper,
        public yarp::dev::IMultipleWrapper,
        public yarp::os::PeriodicThread
{
private:
    double period{DEFAULT_THREAD_PERIOD};
    int count{0};
    int count2{0};
    ServerGrabberResponder* responder{nullptr};
    ServerGrabberResponder* responder2{nullptr};
    yarp::proto::framegrabber::RgbVisualParams_Responder rgbParser;
    yarp::proto::framegrabber::RgbVisualParams_Responder rgbParser2;
    yarp::dev::IRgbVisualParams* rgbVis_p{nullptr};
    yarp::dev::IRgbVisualParams* rgbVis_p2{nullptr};
    std::string rpcPort_Name;
    std::string rpcPort2_Name;
    yarp::os::Port rpcPort;
    yarp::os::Port rpcPort2;
    std::string pImg_Name;
    std::string pImg2_Name;
    yarp::os::BufferedPort<yarp::sig::FlexImage> pImg;
    yarp::os::BufferedPort<yarp::sig::FlexImage> pImg2;
    yarp::os::Port *p2{nullptr};//audio
    yarp::dev::PolyDriver* poly{nullptr}; //subDeviceOwned convert to pointer
    yarp::dev::PolyDriver* poly2{nullptr};
    yarp::dev::IFrameGrabberImage *fgImage{nullptr};
    yarp::dev::IFrameGrabberImage *fgImage2{nullptr};
    yarp::dev::IFrameGrabberImageRaw *fgImageRaw{nullptr};
    yarp::dev::IFrameGrabberImageRaw *fgImageRaw2{nullptr};
    //yarp::sig::FlexImage  doubleImage, doubleImage2;
//    IAudioVisualGrabber *fgAv{nullptr}; //TODO: manage the AV
    yarp::dev::IFrameGrabberControls *fgCtrl{nullptr};
    yarp::dev::IFrameGrabberControls *fgCtrl2{nullptr};
    yarp::dev::IFrameGrabberControlsDC1394* fgCtrl_DC1394{nullptr};
    yarp::dev::IFrameGrabberControlsDC1394* fgCtrl2_DC1394{nullptr};
    yarp::dev::IPreciselyTimed *fgTimed{nullptr};
    yarp::proto::framegrabber::FrameGrabberControls_Responder ifgCtrl_Responder;
    yarp::proto::framegrabber::FrameGrabberControls_Responder ifgCtrl2_Responder;
    yarp::proto::framegrabber::FrameGrabberControlsDC1394_Responder ifgCtrl_DC1394_Responder;
    yarp::proto::framegrabber::FrameGrabberControlsDC1394_Responder ifgCtrl2_DC1394_Responder;
    Configuration param;
    yarp::sig::ImageOf<yarp::sig::PixelRgb>* img{nullptr};
    yarp::sig::ImageOf<yarp::sig::PixelRgb>* img2{nullptr};
    yarp::sig::ImageOf<yarp::sig::PixelMono>* img_Raw{nullptr};
    yarp::sig::ImageOf<yarp::sig::PixelMono>* img2_Raw{nullptr};

    // Open the wrapper only, the attach method needs to be called before using it
    // Typical usage: yarprobotinterface
    bool openDeferredAttach(yarp::os::Searchable& prop);

    // If a subdevice parameter is given, the wrapper will open it(or them) and attach to immediately.
    // Typical usage: simulator or command line
    bool isSubdeviceOwned{false};
    bool openAndAttachSubDevice(yarp::os::Searchable& prop);

public:
    ServerGrabber();
    ServerGrabber(const ServerGrabber&) = delete;
    ServerGrabber(ServerGrabber&&) = delete;
    ServerGrabber& operator=(const ServerGrabber&) = delete;
    ServerGrabber& operator=(ServerGrabber&&) = delete;
    ~ServerGrabber() override;

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
    bool        attachAll(const yarp::dev::PolyDriverList &device2attach) override;

    bool        detachAll() override;

    // IWrapper interface
    bool        attach(yarp::dev::PolyDriver *poly) override;

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
