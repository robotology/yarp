/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FRAMEGRABBER_NWC_YARP_STREAM_RECEIVER_H
#define YARP_FRAMEGRABBER_NWC_YARP_STREAM_RECEIVER_H

#include <yarp/os/Port.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/dev/ReturnValue.h>

#include "FrameGrabberMsgs.h"

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

#endif // YARP_FRAMEGRABBER_NWC_YARP_STREAM_RECEIVER_H
