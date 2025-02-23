/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IFRAMEGRABBERIMAGE_H
#define YARP_DEV_IFRAMEGRABBERIMAGE_H

#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/Portable.h>

#include <yarp/sig/Image.h>
#include <yarp/sig/Vector.h>

#include <yarp/dev/api.h>
#include <yarp/dev/ReturnValue.h>

namespace yarp::dev
{
    typedef enum
    {
        YARP_CROP_RECT = 0, // Rectangular region of interest style, requires the two corner as a parameter
        YARP_CROP_LIST      // Unordered list of points, the returned image will be a nx1 image with n the
                            // number of points required by user (size of input vector), with the corresponding
                            // pixel color.
    } cropType_id_t;

    class YARP_dev_API vertex_t : public yarp::os::Portable
    {
        public:
        virtual ~vertex_t();
        vertex_t() = default;
        vertex_t(int x, int y);

        int x=0;
        int y=0;

        bool read(yarp::os::ConnectionReader& connection) override;
        bool write(yarp::os::ConnectionWriter& connection) const override;

    };
} // namespace yarp::dev

namespace yarp::dev {

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

/**
 * @ingroup dev_iface_media
 *
 * Read a YARP-format image (of a specific type) from a device.
 */
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
    virtual yarp::dev::ReturnValue getImage(ImageType& image) = 0;

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
    virtual yarp::dev::ReturnValue getImageCrop(yarp::dev::cropType_id_t cropType,
                              std::vector<vertex_t> vertices,
                              ImageType& image);
};

using IFrameGrabberImage = IFrameGrabberOf<yarp::sig::ImageOf<yarp::sig::PixelRgb>>;
using IFrameGrabberImageRaw = IFrameGrabberOf<yarp::sig::ImageOf<yarp::sig::PixelMono>>;

YARP_dev_EXTERN template class YARP_dev_API IFrameGrabberOf<yarp::sig::ImageOf<yarp::sig::PixelRgb>>;
YARP_dev_EXTERN template class YARP_dev_API IFrameGrabberOf<yarp::sig::ImageOf<yarp::sig::PixelMono>>;
YARP_dev_EXTERN template class YARP_dev_API IFrameGrabberOf<yarp::sig::ImageOf<yarp::sig::PixelFloat>>;
YARP_dev_EXTERN template class YARP_dev_API IFrameGrabberOf<yarp::sig::FlexImage>;

} // namespace yarp::dev

#endif // YARP_DEV_IFRAMEGRABBERIMAGE_H
