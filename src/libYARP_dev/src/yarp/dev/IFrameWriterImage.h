/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_IFRAMEWRITERIMAGE_H
#define YARP_DEV_IFRAMEWRITERIMAGE_H

#include <yarp/dev/api.h>
#include <yarp/sig/Image.h>

namespace yarp {
namespace dev {

/**
 * @ingroup dev_iface_media
 *
 * Read a YARP-format image to a device.
 */
class YARP_dev_API IFrameWriterImage
{
public:
    /**
     * Destructor.
     */
    virtual ~IFrameWriterImage();

    /**
     * Write an image to the device.
     *
     * @param image the image to write
     * @return true/false upon success/failure
     */
    virtual bool putImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) = 0;
};

} // namespace dev
} // namespace yarp

#endif // YARP_DEV_IFRAMEGRABBER_H
