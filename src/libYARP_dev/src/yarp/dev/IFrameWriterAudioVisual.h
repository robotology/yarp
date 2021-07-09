/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IFRAMEWRITERAUDIOVISUAL_H
#define YARP_DEV_IFRAMEWRITERAUDIOVISUAL_H

#include <yarp/dev/api.h>

#include <yarp/sig/Image.h>
#include <yarp/sig/Sound.h>

namespace yarp {
namespace dev {

/**
 * @ingroup dev_iface_media
 *
 * Write a YARP-format image and sound to a device.
 */
class YARP_dev_API IFrameWriterAudioVisual
{
public:
    /**
     * Destructor.
     */
    virtual ~IFrameWriterAudioVisual();

    /**
     * Write an image and sound
     *
     * @param image the image to be written
     * @param sound the sound to be written
     * @return true/false upon success/failure
     */
    virtual bool putAudioVisual(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image,
                                yarp::sig::Sound& sound) = 0;
};

} // namespace dev
} // namespace yarp

#endif // YARP_DEV_IFRAMEWRITERAUDIOVISUAL_H
