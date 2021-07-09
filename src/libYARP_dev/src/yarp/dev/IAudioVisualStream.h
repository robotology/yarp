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

namespace yarp {
namespace dev {

class YARP_dev_API IAudioVisualStream
{
public:
    /**
     * Destructor.
     */
    virtual ~IAudioVisualStream();

    virtual bool hasAudio() = 0;

    virtual bool hasVideo() = 0;

    virtual bool hasRawVideo() {
        return hasVideo();
    }
};

} // namespace dev
} // namespace yarp

#endif // YARP_DEV_IAUDIOVISUALSTREAM_H
