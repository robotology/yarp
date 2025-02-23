/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "StreamReceiver.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>


namespace {
YARP_LOG_COMPONENT(FRAMEGRABBER_NWC_YARP, "yarp.devices.frameGrabber_nwc_yarp")
} // namespace

using namespace yarp::dev;
using namespace yarp::sig;

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

// BEGIN Template instances
template bool StreamReceiver::lastImage<ImageOf<PixelRgb>>(ImageOf<PixelRgb>& image);
template bool StreamReceiver::lastImage<ImageOf<PixelMono>>(ImageOf<PixelMono>& image);
template bool StreamReceiver::lastImage<ImageOf<PixelFloat>>(ImageOf<PixelFloat>& image);
template bool StreamReceiver::lastImage<FlexImage>(FlexImage& image);
// END Template instances
