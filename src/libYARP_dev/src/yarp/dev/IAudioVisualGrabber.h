/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IAUDIOVISUALGRABBER_H
#define YARP_DEV_IAUDIOVISUALGRABBER_H

#include <yarp/dev/api.h>

#include <yarp/sig/Image.h>
#include <yarp/sig/Sound.h>

namespace yarp {
namespace dev {

/**
 * @ingroup dev_iface_media
 *
 * Read a YARP-format image and sound from a device.
 */
class YARP_dev_API IAudioVisualGrabber
{
public:
    /**
     * Destructor.
     */
    virtual ~IAudioVisualGrabber();

    /**
     * Get an image and sound
     *
     * @param image the image to be filled
     * @param sound the sound to be filled
     * @return true/false upon success/failure
     */
    virtual bool getAudioVisual(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image,
                                yarp::sig::Sound& sound) = 0;
};

} // namespace dev
} // namespace yarp

#endif // YARP_DEV_IAUDIOVISUALGRABBER_H
