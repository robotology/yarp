/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_IFRAMEGRABBERIMAGERAW_H
#define YARP_DEV_IFRAMEGRABBERIMAGERAW_H

#include <yarp/dev/api.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/Vector.h>

#include <yarp/dev/IFrameGrabberImage.h> // FIXME For cropType_id_t

namespace yarp {
namespace dev {

/**
 * @ingroup dev_iface_media
 *
 * Read a YARP-format image from a device.
 */
class YARP_dev_API IFrameGrabberImageRaw
{
public:
    /**
     * Destructor.
     */
    virtual ~IFrameGrabberImageRaw();
    /**
     * Get a raw image from the frame grabber
     *
     * @param image the image to be filled
     * @return true/false upon success/failure
     */
    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image) = 0;

    /**
     * Get a crop of the rgb image from the frame grabber, if required
     * demosaicking/color reconstruction is applied
     *
     * Note: this is not configuring the camera sensor to acquire a crop
     *       of the image, nor to generate a cropped version of the streaming.
     *       Instead, the full image is acquired and then a crop is created from
     *       it. The crop is meant to be created by the image producer upon user
     *       request via RPC call.
     *
     * @param cropType enum specifying how the crop shall be generated, defined at FrameGrabberInterfaces.h
     * @param vertices the input coordinate (u,v) required by the cropType
     * @param image the image to be filled
     * @return true/false upon success/failure
     */
    virtual bool getImageCrop(cropType_id_t cropType, yarp::sig::VectorOf<std::pair<int, int>> vertices, yarp::sig::ImageOf<yarp::sig::PixelMono>& image)
    {
        return false;
    };

    /**
     * Return the height of each frame.
     * @return image height
     */
    virtual int height() const = 0;

    /**
     * Return the width of each frame.
     * @return image width
     */
    virtual int width() const = 0;
};


} // namespace dev
} // namespace yarp

#endif // YARP_DEV_IFRAMEGRABBERIMAGERAW_H
