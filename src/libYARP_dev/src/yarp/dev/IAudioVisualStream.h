/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @ingroup dev_iface_media
 *
 * For streams capable of holding different kinds of content,
 * check what they actually have.
 *
 */

#ifndef YARP_DEV_IAUDIOVISUALSTREAM_H
#define YARP_DEV_IAUDIOVISUALSTREAM_H

#include <yarp/dev/api.h>

#include <yarp/dev/ReturnValue.h>

namespace yarp::dev {

class YARP_dev_API IAudioVisualStream
{
public:
    /**
     * Destructor.
     */
    virtual ~IAudioVisualStream();

    virtual yarp::dev::ReturnValue hasAudio(bool& has) = 0;

    virtual yarp::dev::ReturnValue hasVideo(bool& has) = 0;

    virtual yarp::dev::ReturnValue hasRawVideo(bool& has) = 0;
};

} // namespace yarp::dev

#endif // YARP_DEV_IAUDIOVISUALSTREAM_H
