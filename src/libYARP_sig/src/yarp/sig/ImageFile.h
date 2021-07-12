/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_IMAGEFILE_H
#define YARP_SIG_IMAGEFILE_H

#include <string>
#include <yarp/sig/Image.h>

namespace yarp {
    namespace sig{
        /**
         * \ingroup sig_class
         *
         * Image file operations.
         */
        namespace file
        {
            enum image_fileformat
                {
                    FORMAT_NULL,
                    FORMAT_ANY,
                    FORMAT_PGM,
                    FORMAT_PPM,
                    FORMAT_JPG,
                    FORMAT_NUMERIC,
                    FORMAT_NUMERIC_COMPRESSED,
                    FORMAT_PNG
                };

            // read methods
            bool YARP_sig_API read(ImageOf<PixelRgb>& dest,   const std::string& src, image_fileformat format = FORMAT_ANY);
            bool YARP_sig_API read(ImageOf<PixelBgr>& dest,   const std::string& src, image_fileformat format = FORMAT_ANY);
            bool YARP_sig_API read(ImageOf<PixelRgba>& dest,  const std::string& src, image_fileformat format = FORMAT_ANY);
            bool YARP_sig_API read(ImageOf<PixelMono>& dest,  const std::string& src, image_fileformat format = FORMAT_ANY);
            bool YARP_sig_API read(ImageOf<PixelFloat>& dest, const std::string& src, image_fileformat format = FORMAT_ANY);

            // write methods
            bool YARP_sig_API write(const ImageOf<PixelRgb>& src,   const std::string& dest, image_fileformat format = FORMAT_PPM);
            bool YARP_sig_API write(const ImageOf<PixelBgr>& src,   const std::string& dest, image_fileformat format = FORMAT_PPM);
            bool YARP_sig_API write(const ImageOf<PixelRgba>& src,  const std::string& dest, image_fileformat format = FORMAT_PPM);
            bool YARP_sig_API write(const ImageOf<PixelMono>& src,  const std::string& dest, image_fileformat format = FORMAT_PGM);
            bool YARP_sig_API write(const ImageOf<PixelFloat>& src, const std::string& dest, image_fileformat format = FORMAT_NUMERIC);
            bool YARP_sig_API write(const Image& src,               const std::string& dest, image_fileformat format = FORMAT_PPM);
        }
    }
}

#endif // YARP_SIG_IMAGEFILE_H
