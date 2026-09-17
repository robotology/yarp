/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_LAYERED_IMAGE_H
#define YARP_LAYERED_IMAGE_H

#include <yarp/sig/Image.h>
#include <yarp/sig/LayeredImageData.h>

#include <vector>

namespace yarp::sig {
class LayeredImage;
} // namespace yarp::sig

/**
 * \ingroup sig_class
 *
 * A Layered Image, composed by a background and multiple layers
 */
class YARP_sig_API yarp::sig::LayeredImage : public yarp::sig::LayeredImageData
{
public:
    /**
     * Default constructor.
     * Creates an empty LayeredImage.
     */
    LayeredImage();

    /**
     * Copy constructor.
     * Clones the content of another LayeredImage.
     * @param alt the LayeredImage to clone
     */
    LayeredImage(const LayeredImage& alt);

    /**
     * @brief Move constructor.
     *
     * @param other the LayeredImage to be moved
     */
    LayeredImage(LayeredImage&& other) noexcept;

    /**
     * Assignment operator.
     * Clones the content of another LayeredImage.
     * @param alt the LayeredImage to clone
     */
    LayeredImage& operator=(const LayeredImage& alt);

    /**
     * @brief Move assignment operator.
     *
     * @param other the LayeredImage to be moved
     * @return this object
     */
    LayeredImage& operator=(LayeredImage&& other) noexcept;

    /**
     * Comparison operator.
     * Compares two LayeredImage
     * @return true if the two LayeredImages are identical
     */
    bool operator==(const LayeredImage& alt) const;

    /**
     * Destructor.
     */
    virtual ~LayeredImage();

    /**
     * Conversion operator
     */
    yarp::sig::FlexImage convert_to_flexImage();

    /**
     * Conversion operator
     */
    operator yarp::sig::FlexImage();

    /**
     * Clear the layered Image
     */
    void clear();
};

#endif // YARP_LAYERED_IMAGE_H
