/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_IFRAMEGRABBERIMAGE_H
#define YARP_DEV_IFRAMEGRABBERIMAGE_H

#include <yarp/dev/api.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/Vector.h>

typedef enum {
    YARP_CROP_RECT = 0,             // Rectangular region of interest style, requires the two corner as a parameter
    YARP_CROP_LIST                  // Unordered list of points, the returned image will be a nx1 image with n the
                                    // number of points required by user (size of input vector), with the corresponding
                                    // pixel color.
} cropType_id_t;

namespace yarp {
namespace dev {

/**
 * @ingroup dev_iface_media
 *
 * Read a YARP-format image from a device.
 */
class YARP_dev_API IFrameGrabberImage
{
public:
    /**
     * Destructor.
     */
    virtual ~IFrameGrabberImage();

    /**
     * Get an rgb image from the frame grabber, if required
     * demosaicking/color reconstruction is applied
     *
     * @param image the image to be filled
     * @return true/false upon success/failure
     */
    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) = 0;

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
     * @param cropType enum specifying how the crop shall be generated
     * @param vertices the input coordinate (u,v) required by the cropType
     * @param image the image to be filled
     * @return true/false upon success/failure
     */
    virtual bool getImageCrop(cropType_id_t cropType,
                              yarp::sig::VectorOf<std::pair<int, int>> vertices,
                              yarp::sig::ImageOf<yarp::sig::PixelRgb>& image);

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

#endif // YARP_DEV_IFRAMEGRABBERIMAGE_H
