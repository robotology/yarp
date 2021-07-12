/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FRAMEGRABBER_NWS_YARP_H
#define YARP_FRAMEGRABBER_NWS_YARP_H

#include <yarp/os/BufferedPort.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/RpcServer.h>
#include <yarp/os/Stamp.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IFrameGrabberControls.h>
#include <yarp/dev/IFrameGrabberControlsDC1394.h>
#include <yarp/dev/IFrameGrabberImage.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/dev/IRgbVisualParams.h>
#include <yarp/dev/WrapperSingle.h>

#include <yarp/proto/framegrabber/FrameGrabberControlsDC1394_Responder.h>
#include <yarp/proto/framegrabber/FrameGrabberControls_Responder.h>
#include <yarp/proto/framegrabber/FrameGrabberOf_Responder.h>
#include <yarp/proto/framegrabber/RgbVisualParams_Responder.h>


/**
 * @ingroup dev_impl_nws_yarp
 *
 * \brief `frameGrabber_nws_yarp`: A YARP NWS for camera devices.
 *
 * This device is paired with its client called `frameGrabber_nwc_yarp` to
 * receive the data streams and perform remote operations.
 *
 * It is also possible to read the images without the client connecting to
 * the streaming Port.
 *
 * \section frameGrabber_nws_yarp_device_parameters Description of input parameters
 *
 * Parameters required by this device are:
 * | Parameter name | SubParameter            | Type    | Units          | Default Value | Required | Description                                                                      | Notes |
 * |:--------------:|:-----------------------:|:-------:|:--------------:|:-------------:|:--------:|:--------------------------------------------------------------------------------:|:-----:|
 * | period         |      -                  | double  | s              |   0.03        | No       | refresh period (in s) of the broadcasted values through yarp ports               | default 0.03s |
 * | name           |      -                  | string  | -              |   /grabber    | No       | Prefix name of the ports opened by the FrameGrabber_nws_yarp                     | Required suffix like '/rpc' will be added by the device |
 * | capabilities   |      -                  | string  | -              |   COLOR       | No       | two capabilities supported, COLOR and RAW respectively for rgb and raw streaming | - |
 * | subdevice      |      -                  | string  | -              |   -           | No       | name of the subdevice to use as a data source                                    | when used, parameters for the subdevice must be provided as well; when not used, the device should be 'attached' |
// FIXME DRDANZ no_drop?
 *
 * Some example of configuration files:
 *
 * Example of configuration file using .ini format.
 *
 * \code{.unparsed}
 * device frameGrabber_nws_yarp
 * capabilities COLOR        # not necessary to specify 'COLOR', this is the default value if capabilities is omitted
 * period 30
 * subdevice fakeFrameGrabber
 * \endcode
 *
 * Example of configuration file using .xml format.
 *
 * \code{.xml}
 *
 * <device name="grabber" type="fakeFrameGrabber">
 *   <param name="mode"> grid </param>
 *   <param name="timestamp"> 1 </param>
 *   <param name="freq"> 30 </param>
 * </device>
 *
 * <device name="wrapper" type="frameGrabber_nws_yarp">
 *  <param name="period"> 30 </param>
 *  <param name="name"> /grabber </param>
 *  <param name="capabilities"> COLOR </param>
 *
 *  <action phase="startup" level="10" type="attach">
 *    <param name="device"> grabber </param>
 *  </action>
 *  <action phase="shutdown" level="5" type="detach" />
 * </device>
 * \endcode
 */
class FrameGrabber_nws_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::dev::WrapperSingle,
        public yarp::os::PeriodicThread,
        public yarp::dev::DeviceResponder
{
private:
    // Ports
    yarp::os::RpcServer rpcPort;
    yarp::os::BufferedPort<yarp::sig::FlexImage> pImg;

    // Subdevice
    yarp::dev::PolyDriver* subdevice {nullptr};
    bool isSubdeviceOwned {false};

    // Interfaces handled
    yarp::dev::IRgbVisualParams* iRgbVisualParams {nullptr};
    yarp::dev::IFrameGrabberImage* iFrameGrabberImage {nullptr};
    yarp::dev::IFrameGrabberImageRaw* iFrameGrabberImageRaw {nullptr};
    yarp::dev::IFrameGrabberControls* iFrameGrabberControls {nullptr};
    yarp::dev::IFrameGrabberControlsDC1394* iFrameGrabberControlsDC1394 {nullptr};
    yarp::dev::IPreciselyTimed* iPreciselyTimed {nullptr};

    // Responders
    yarp::proto::framegrabber::FrameGrabberOf_Responder<yarp::sig::ImageOf<yarp::sig::PixelRgb>> frameGrabberImage_Responder;
    yarp::proto::framegrabber::FrameGrabberOf_Responder<yarp::sig::ImageOf<yarp::sig::PixelMono>, VOCAB_FRAMEGRABBER_IMAGERAW> frameGrabberImageRaw_Responder;
    yarp::proto::framegrabber::RgbVisualParams_Responder rgbVisualParams_Responder;
    yarp::proto::framegrabber::FrameGrabberControls_Responder frameGrabberControls_Responder;
    yarp::proto::framegrabber::FrameGrabberControlsDC1394_Responder frameGrabberControlsDC1394_Responder;

    // Images
    yarp::sig::ImageOf<yarp::sig::PixelRgb>* img {nullptr};
    yarp::sig::ImageOf<yarp::sig::PixelMono>* img_Raw {nullptr};

    // Internal state
    bool active {false};
    yarp::os::Stamp stamp;

    // Options
    static constexpr double DEFAULT_THREAD_PERIOD = 0.03; // seconds
    double period {DEFAULT_THREAD_PERIOD};
    bool noDrop {true}; // FIXME DRDANZ

    enum Capabilities
    {
        COLOR,
        RAW,
    };
    Capabilities cap {COLOR};

public:
    FrameGrabber_nws_yarp();
    FrameGrabber_nws_yarp(const FrameGrabber_nws_yarp&) = delete;
    FrameGrabber_nws_yarp(FrameGrabber_nws_yarp&&) = delete;
    FrameGrabber_nws_yarp& operator=(const FrameGrabber_nws_yarp&) = delete;
    FrameGrabber_nws_yarp& operator=(FrameGrabber_nws_yarp&&) = delete;
    ~FrameGrabber_nws_yarp() override;

    // DeviceDriver
    bool close() override;
    bool open(yarp::os::Searchable& config) override;

    // IWrapper interface
    bool attach(yarp::dev::PolyDriver* poly) override;
    bool detach() override;

    //RateThread
    bool threadInit() override;
    void run() override;

    // DeviceResponder
    bool respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply) override;
};

#endif // YARP_FRAMEGRABBER_NWS_YARP_H
