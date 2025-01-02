/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_LAYERED_IMAGE_H
#define YARP_LAYERED_IMAGE_H

#include <yarp/os/Portable.h>
#include <yarp/sig/Image.h>
#include <vector>

namespace yarp::sig {
class LayeredImage;
class ImageLayer;
} // namespace yarp::sig

/**
 * \ingroup sig_class
 *
 * A single layer of a layered image
 */
class YARP_sig_API yarp::sig::ImageLayer
{
    public:
    yarp::sig::FlexImage layer;
    bool enable=true;
    class colorkey_s
    {
        public:
        int  value;
        bool enable;

        //default constructor
        colorkey_s() : enable(true), value(0) {}

        void setValueAsPixelRgb(yarp::sig::PixelRgb v)
        {
            value = *reinterpret_cast<int*>(&v);
        }
        yarp::sig::PixelRgb getValueAsPixelRgb()
        {
            return *reinterpret_cast<yarp::sig::PixelRgb*>(&value);
        }
    };
    colorkey_s colorkey;

    class alpha_s
    {
        public:
        bool enable;
        float value;

        // default constructor
        alpha_s() : enable(true), value(1.0) {}
    };
    alpha_s alpha;

    bool can_be_compressed = true;
    int offset_x=0;
    int offset_y=0;

    ImageLayer(const yarp::sig::FlexImage& img, bool ena = true, colorkey_s ckey = colorkey_s(), alpha_s alph = alpha_s(), bool compress = true, int off_x = 0, int off_y = 0)
    {
        layer = img;
        enable = ena;
        colorkey = ckey;
        alpha = alph;
        can_be_compressed = compress;
        offset_x = off_x;
        offset_y = off_y;
    }
};

/**
 * \ingroup sig_class
 *
 * A Layered Image, composed by a background and multiple layers
 */
class YARP_sig_API yarp::sig::LayeredImage : public yarp::os::Portable
{
public:
    //internal data
    yarp::sig::FlexImage background;
    std::vector<yarp::sig::ImageLayer> layers;

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
    ~LayeredImage() override;

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

    /**
     * Read a LayeredImage from a connection.
     * @return true iff LayeredImage was read correctly
     */
    bool read(yarp::os::ConnectionReader& connection) override;

    /**
     * Write a LayeredImage to a connection.
     * @return true iff LayeredImage was written correctly
     */
    bool write(yarp::os::ConnectionWriter& connection) const override;

    yarp::os::Type getReadType() const override {
        return yarp::os::Type::byName("yarp/layeredimage");
    }
};

#endif // YARP_LAYERED_IMAGE_H
