/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FrameGrabber_nwc_yarp.h"

#include <yarp/os/Network.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <yarp/proto/framegrabber/CameraVocabs.h>


namespace {
YARP_LOG_COMPONENT(FRAMEGRABBER_NWC_YARP, "yarp.devices.frameGrabber_nwc_yarp")
} // namespace



// BEGIN StreamReceiver
int StreamReceiver::lastHeight() const
{
    return m_lastHeight;
}

int StreamReceiver::lastWidth() const
{
    return m_lastWidth;
}

yarp::os::Stamp StreamReceiver::lastStamp() const
{
    return m_lastStamp;
}

template <typename ImageType>
bool StreamReceiver::lastImage(ImageType& image)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    bool ret {false};
    if (!port.isOpen()) {
        image.zero();
        ret = false;
    } else if (reader.check()) {
        ret = image.copy(*(reader.read(true)));
        reader.getEnvelope(m_lastStamp);
        m_lastHeight = image.height();
        m_lastWidth = image.width();
    } else {
        ret = image.copy(*(reader.lastRead()));
    }

    return ret;
}

bool StreamReceiver::open(const std::string& local,
               const std::string& remote,
               const std::string& carrier)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!port.open(local)) {
        yCError(FRAMEGRABBER_NWC_YARP) << "Failed to open " << local << "port.";
    }

    if (!remote.empty()) {
        yCInfo(FRAMEGRABBER_NWC_YARP) << "Connecting" << port.getName() << "to" << remote;
        if (!yarp::os::NetworkBase::connect(remote, port.getName(), carrier)) {
            yCError(FRAMEGRABBER_NWC_YARP) << "Failed to connect" << local << "to" << remote;
            return false;
        }
    } else {
        yCInfo(FRAMEGRABBER_NWC_YARP) << "No remote specified. Waiting for connection";
    }

    reader.attach(port);

    return true;
}

bool StreamReceiver::close()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_lastStamp = {0, 0.0};
    m_lastHeight = 0;
    m_lastWidth = 0;

    if (!port.isOpen()) {
        return true;
    }
    port.interrupt();
    port.close();

    return true;
}
// END StreamReceiver


// BEGIN FrameGrabberOf_ForwarderWithStream
template <typename ImageType,
          yarp::conf::vocab32_t IfVocab,
          yarp::conf::vocab32_t ImgVocab>
FrameGrabberOf_ForwarderWithStream<ImageType, IfVocab, ImgVocab>::FrameGrabberOf_ForwarderWithStream(yarp::os::Port& rpcPort) :
        yarp::proto::framegrabber::FrameGrabberOf_Forwarder<ImageType, IfVocab, ImgVocab>(rpcPort)
{
}

template <typename ImageType,
          yarp::conf::vocab32_t IfVocab,
          yarp::conf::vocab32_t ImgVocab>
int FrameGrabberOf_ForwarderWithStream<ImageType, IfVocab, ImgVocab>::height() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_streamReceiver) {
        return yarp::proto::framegrabber::FrameGrabberOf_Forwarder<ImageType, IfVocab, ImgVocab>::height();
    }

    return m_streamReceiver->lastHeight();
}

template <typename ImageType,
          yarp::conf::vocab32_t IfVocab,
          yarp::conf::vocab32_t ImgVocab>
int FrameGrabberOf_ForwarderWithStream<ImageType, IfVocab, ImgVocab>::width() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_streamReceiver) {
        return yarp::proto::framegrabber::FrameGrabberOf_Forwarder<ImageType, IfVocab, ImgVocab>::width();
    }

    return m_streamReceiver->lastWidth();
}

template <typename ImageType,
          yarp::conf::vocab32_t IfVocab,
          yarp::conf::vocab32_t ImgVocab>
bool FrameGrabberOf_ForwarderWithStream<ImageType, IfVocab, ImgVocab>::getImage(ImageType& image)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_streamReceiver) {
        return yarp::proto::framegrabber::FrameGrabberOf_Forwarder<ImageType, IfVocab, ImgVocab>::getImage(image);
    }

    return m_streamReceiver->lastImage(image);
}

template <typename ImageType,
          yarp::conf::vocab32_t IfVocab,
          yarp::conf::vocab32_t ImgVocab>
bool FrameGrabberOf_ForwarderWithStream<ImageType, IfVocab, ImgVocab>::getImageCrop(cropType_id_t cropType,
                                                                                    yarp::sig::VectorOf<std::pair<int, int>> vertices,
                                                                                    ImageType& image)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if(!m_streamReceiver) {
        return yarp::proto::framegrabber::FrameGrabberOf_Forwarder<ImageType, IfVocab, ImgVocab>::getImageCrop(cropType, vertices, image);
    }

    // yarp::dev::IFrameGrabberOf<ImageType>::getImageCrop() calls getImage()
    // internally, and crops the returned image
    return yarp::dev::IFrameGrabberOf<ImageType>::getImageCrop(cropType, vertices, image);
}


template <typename ImageType,
          yarp::conf::vocab32_t IfVocab,
          yarp::conf::vocab32_t ImgVocab>
void FrameGrabberOf_ForwarderWithStream<ImageType, IfVocab, ImgVocab>::setStreamReceiver(StreamReceiver* streamReceiver)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_streamReceiver = streamReceiver;
}
// END FrameGrabberOf_ForwarderWithStream



// BEGIN FrameGrabber_nwc_yarp
FrameGrabber_nwc_yarp::FrameGrabber_nwc_yarp() :
        FrameGrabberOf_ForwarderWithStream<yarp::sig::ImageOf<yarp::sig::PixelRgb>>(rpcPort),
        FrameGrabberOf_ForwarderWithStream<yarp::sig::ImageOf<yarp::sig::PixelMono>, VOCAB_FRAMEGRABBER_IMAGERAW>(rpcPort),
        FrameGrabberOf_ForwarderWithStream<yarp::sig::ImageOf<yarp::sig::PixelFloat>>(rpcPort),
        FrameGrabberOf_ForwarderWithStream<yarp::sig::FlexImage>(rpcPort),
        yarp::proto::framegrabber::FrameGrabberControls_Forwarder(rpcPort),
        yarp::proto::framegrabber::FrameGrabberControlsDC1394_Forwarder(rpcPort),
        yarp::proto::framegrabber::RgbVisualParams_Forwarder(rpcPort)
{
}


bool FrameGrabber_nwc_yarp::open(yarp::os::Searchable& config)
{
    std::string remote = config.check("remote", yarp::os::Value(""), "port name of real grabber").asString();
    std::string local = config.check("local", yarp::os::Value("..."), "port name to use locally").asString();
    std::string carrier = config.check("stream", yarp::os::Value("tcp"), "carrier to use for streaming").asString();
    bool no_stream = config.check("no_stream");

    if (!no_stream) {
        if (!streamReceiver.open(local, remote, carrier)) {
            return false;
        }
    }

    std::string rpc_local = local + "/rpc_client";
    std::string rpc_remote = remote + "/rpc";
    if (!rpcPort.open(rpc_local)) {
        yCError(FRAMEGRABBER_NWC_YARP) << "Failed to open " << rpc_local << "port.";
    }

    if (!remote.empty()) {
        yCInfo(FRAMEGRABBER_NWC_YARP) << "Connecting" << rpcPort.getName() << "to" << rpc_remote;
        if (!yarp::os::NetworkBase::connect(rpcPort.getName(), rpc_remote)) {
            yCError(FRAMEGRABBER_NWC_YARP) << "Failed to connect" << rpcPort.getName() << "to" << rpc_remote;
            return false;
        }
    } else {
        yCInfo(FRAMEGRABBER_NWC_YARP) << "No remote specified. Waiting for connection";
    }

    return true;
}

bool FrameGrabber_nwc_yarp::close()
{
    rpcPort.interrupt();
    rpcPort.close();

    streamReceiver.close();

    return true;
}


yarp::os::Stamp FrameGrabber_nwc_yarp::getLastInputStamp()
{
    return streamReceiver.lastStamp();
}

// END FrameGrabber_nwc_yarp
