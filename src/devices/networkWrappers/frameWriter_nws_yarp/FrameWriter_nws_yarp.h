/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FRAMEWRITER_NWS_YARP_H
#define YARP_FRAMEWRITER_NWS_YARP_H

#include <yarp/os/BufferedPort.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/RpcServer.h>
#include <yarp/os/Stamp.h>

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IFrameWriterImage.h>
#include <yarp/dev/WrapperSingle.h>

#include "FrameWriter_nws_yarp_ParamsParser.h"

class Image_InputPortProcessor : public yarp::os::BufferedPort<yarp::sig::FlexImage>
{
    std::vector<yarp::sig::FlexImage> lastData;
    std::mutex m_mutex;

public:
    Image_InputPortProcessor() = default;

    using yarp::os::BufferedPort<yarp::sig::FlexImage>::onRead;
    void onRead(yarp::sig::FlexImage& v) override
    {
        std::lock_guard<std::mutex> mylock(m_mutex);
        lastData.push_back(v);
    };

    inline void getLast(std::vector<yarp::sig::FlexImage>& img)
    {
        std::lock_guard<std::mutex> mylock(m_mutex);
        img = lastData;
        lastData.clear();
    }
};

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

class FrameWriter_nws_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::dev::WrapperSingle,
        public yarp::os::PeriodicThread,
        public FrameWriter_nws_yarp_ParamsParser
{
private:
    // Ports
    yarp::os::RpcServer rpcPort;
    Image_InputPortProcessor                     m_portProc;

    // Interfaces handled
    yarp::dev::IFrameWriterImage* iFrameWriterImage {nullptr};

public:
    FrameWriter_nws_yarp();
    FrameWriter_nws_yarp(const FrameWriter_nws_yarp&) = delete;
    FrameWriter_nws_yarp(FrameWriter_nws_yarp&&) = delete;
    FrameWriter_nws_yarp& operator=(const FrameWriter_nws_yarp&) = delete;
    FrameWriter_nws_yarp& operator=(FrameWriter_nws_yarp&&) = delete;
    ~FrameWriter_nws_yarp() override;

    // DeviceDriver
    bool close() override;
    bool open(yarp::os::Searchable& config) override;

    // IWrapper interface
    bool attach(yarp::dev::PolyDriver* poly) override;
    bool detach() override;

    //RateThread
    bool threadInit() override;
    void run() override;
};

#endif // YARP_FRAMEWRITER_NWS_YARP_H
