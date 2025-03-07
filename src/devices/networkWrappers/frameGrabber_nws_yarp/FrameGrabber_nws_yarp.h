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

#include <yarp/proto/framegrabber/FrameGrabberOf_Responder.h>

#include "FrameGrabber_nws_yarp_ParamsParser.h"

#include "IFrameGrabberControlsDC1394ServerImpl.h"
#include "IFrameGrabberControlsServerImpl.h"
#include "IRGBVisualParamsServerImpl.h"

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
 * Parameters required by this device are shown in class: FrameGrabber_nws_yarp_ParamsParser
 */

class FrameGrabber_nws_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::dev::WrapperSingle,
        public yarp::os::PeriodicThread,
        public yarp::os::PortReader,
        public FrameGrabber_nws_yarp_ParamsParser
{
private:
    // Ports
    yarp::os::RpcServer rpcPort;
    yarp::os::BufferedPort<yarp::sig::FlexImage> pImg;

    // Interfaces handled
    std::mutex m_mutex;
    yarp::dev::IRgbVisualParams* m_iRgbVisualParams {nullptr};
    yarp::dev::IFrameGrabberImage* m_iFrameGrabberImage {nullptr};
    yarp::dev::IFrameGrabberImageRaw* m_iFrameGrabberImageRaw {nullptr};
    yarp::dev::IFrameGrabberControls* m_iFrameGrabberControls {nullptr};
    yarp::dev::IFrameGrabberControlsDC1394* m_iFrameGrabberControlsDC1394 {nullptr};
    yarp::dev::IPreciselyTimed* m_iPreciselyTimed {nullptr};

    // Responders
    yarp::proto::framegrabber::FrameGrabberOf_Responder<yarp::sig::ImageOf<yarp::sig::PixelRgb>> frameGrabberImage_Responder;
    yarp::proto::framegrabber::FrameGrabberOf_Responder<yarp::sig::ImageOf<yarp::sig::PixelMono>, VOCAB_FRAMEGRABBER_IMAGERAW> frameGrabberImageRaw_Responder;
    IRGBVisualParamsMsgsRPCd*             m_RPC_RGBVisualParams         {nullptr};
    IFrameGrabberControlMsgsRPCd*         m_RPC_FrameGrabControls       {nullptr};
    IFrameGrabberControlDC1394MsgsRPCd*   m_RPC_FrameGrabControlsDC1394 {nullptr};

    // Images
    yarp::sig::ImageOf<yarp::sig::PixelRgb>* img {nullptr};
    yarp::sig::ImageOf<yarp::sig::PixelMono>* img_Raw {nullptr};

    // Internal state
    yarp::os::Stamp m_stamp;

    enum Capabilities
    {
        COLOR,
        RAW,
    };
    Capabilities m_cap {COLOR};

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

    // PortReader
    virtual bool read(yarp::os::ConnectionReader& connection) override;
};

#endif // YARP_FRAMEGRABBER_NWS_YARP_H
