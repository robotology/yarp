/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FRAMEGRABBER_NWC_YARP_H
#define YARP_FRAMEGRABBER_NWC_YARP_H

#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/Port.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/dev/IPreciselyTimed.h>

#include <yarp/proto/framegrabber/FrameGrabberOf_Forwarder.h>
#include <yarp/proto/framegrabber/FrameGrabberControlsDC1394_Forwarder.h>
#include <yarp/proto/framegrabber/FrameGrabberControls_Forwarder.h>
#include <yarp/proto/framegrabber/RgbVisualParams_Forwarder.h>

#include <mutex>


class StreamReceiver
{
public:
    int lastHeight() const;
    int lastWidth() const;
    yarp::os::Stamp lastStamp() const;

    template <typename ImageType>
    bool lastImage(ImageType& image);

    bool open(const std::string& local,
              const std::string& remote,
              const std::string& carrier);
    bool close();

private:
    yarp::os::PortReaderBuffer<yarp::sig::FlexImage> reader;
    yarp::os::Port port;

    std::mutex m_mutex;
    yarp::os::Stamp m_lastStamp {0, 0.0};
    int m_lastHeight {0};
    int m_lastWidth {0};
};


template <typename ImageType,
          yarp::conf::vocab32_t IfVocab = VOCAB_FRAMEGRABBER_IMAGE,
          yarp::conf::vocab32_t ImgVocab = VOCAB_RGB_IMAGE>
class FrameGrabberOf_ForwarderWithStream:
        public yarp::proto::framegrabber::FrameGrabberOf_Forwarder<ImageType, IfVocab, ImgVocab>
{
public:
    FrameGrabberOf_ForwarderWithStream(yarp::os::Port& rpcPort);
    ~FrameGrabberOf_ForwarderWithStream() override = default;

    // Re-implement the IFrameGrabberOf methods, to use the image received from
    // the streaming port when enabled, instead of calling the requesting them
    // using RPC calls
    int height() const override;
    int width() const override;
    bool getImage(ImageType& image) override;
    bool getImageCrop(cropType_id_t cropType,
                      yarp::sig::VectorOf<std::pair<int, int>> vertices,
                      ImageType& image) override;

    void setStreamReceiver(StreamReceiver* m_streamReceiver);

private:
    mutable std::mutex m_mutex;
    StreamReceiver* m_streamReceiver {nullptr};
};


/**
 * @ingroup dev_impl_nwc_yarp
 *
 * \section frameGrabber_nws_yarp
 *
 * \brief `frameGrabber_nws_yarp`: Connect to a frameGrabber_nws_yarp.
 */
class FrameGrabber_nwc_yarp :
        public yarp::dev::DeviceDriver,
        public FrameGrabberOf_ForwarderWithStream<yarp::sig::ImageOf<yarp::sig::PixelRgb>>,
        public FrameGrabberOf_ForwarderWithStream<yarp::sig::ImageOf<yarp::sig::PixelMono>, VOCAB_FRAMEGRABBER_IMAGERAW>,
        public FrameGrabberOf_ForwarderWithStream<yarp::sig::ImageOf<yarp::sig::PixelFloat>>,
        public FrameGrabberOf_ForwarderWithStream<yarp::sig::FlexImage>,
        public yarp::proto::framegrabber::FrameGrabberControls_Forwarder,
        public yarp::proto::framegrabber::FrameGrabberControlsDC1394_Forwarder,
        public yarp::proto::framegrabber::RgbVisualParams_Forwarder,
        public yarp::dev::IPreciselyTimed
{
public:
    FrameGrabber_nwc_yarp();
    FrameGrabber_nwc_yarp(const FrameGrabber_nwc_yarp&) = delete;
    FrameGrabber_nwc_yarp(FrameGrabber_nwc_yarp&&) = delete;
    FrameGrabber_nwc_yarp& operator=(const FrameGrabber_nwc_yarp&) = delete;
    FrameGrabber_nwc_yarp& operator=(FrameGrabber_nwc_yarp&&) = delete;
    ~FrameGrabber_nwc_yarp() override = default;

    // yarp::dev::DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    // yarp::dev::IPreciselyTimed
    yarp::os::Stamp getLastInputStamp() override;

private:
    StreamReceiver streamReceiver;
    yarp::os::Port rpcPort;
};

#endif // YARP_FRAMEGRABBER_NWC_YARP_H
