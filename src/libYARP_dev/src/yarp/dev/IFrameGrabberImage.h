/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
class YARP_dev_API IFrameGrabberImageBase
{
public:
    /**
     * Destructor.
     */
    virtual ~IFrameGrabberImageBase();

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

template <typename ImageType>
class IFrameGrabberOf :
        public IFrameGrabberImageBase
{
public:
    /**
     * @brief Get an image from the frame grabber.
     *
     * @param image the image to be filled
     * @return true/false upon success/failure
     */
    virtual bool getImage(ImageType& image) = 0;

    /**
     * @brief Get a crop of the image from the frame grabber.
     *
     * Note: this is not configuring the camera sensor to acquire a crop
     *       of the image, nor to generate a cropped version of the streaming.
     *       Instead, the full image is acquired and then a crop is created from
     *       it. If the hardware supports it, it is possible to override this
     *       method and request a hardware crop.
     *
     * @param cropType enum specifying how the crop shall be generated
     * @param vertices the input coordinate (u,v) required by the cropType
     * @param image the image to be filled
     * @return true/false upon success/failure
     */
    virtual bool getImageCrop(cropType_id_t cropType,
                              yarp::sig::VectorOf<std::pair<int, int>> vertices,
                              ImageType& image);
};

using IFrameGrabberImage = IFrameGrabberOf<yarp::sig::ImageOf<yarp::sig::PixelRgb>>;
using IFrameGrabberImageRaw = IFrameGrabberOf<yarp::sig::ImageOf<yarp::sig::PixelMono>>;

YARP_dev_EXTERN template class YARP_dev_API IFrameGrabberOf<yarp::sig::ImageOf<yarp::sig::PixelRgb>>;
YARP_dev_EXTERN template class YARP_dev_API IFrameGrabberOf<yarp::sig::ImageOf<yarp::sig::PixelMono>>;
YARP_dev_EXTERN template class YARP_dev_API IFrameGrabberOf<yarp::sig::ImageOf<yarp::sig::PixelFloat>>;
YARP_dev_EXTERN template class YARP_dev_API IFrameGrabberOf<yarp::sig::FlexImage>;

} // namespace dev
} // namespace yarp

#endif // YARP_DEV_IFRAMEGRABBERIMAGE_H
