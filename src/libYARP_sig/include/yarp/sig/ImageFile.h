/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SIG_IMAGEFILE_H
#define YARP_SIG_IMAGEFILE_H

#include <yarp/os/ConstString.h>
#include <yarp/sig/Image.h>

namespace yarp {
    namespace sig{
        /**
         * \ingroup sig_class
         *
         * Image file operations.
         */
        namespace file {
            enum
                {
                    FORMAT_NULL,
                    FORMAT_ANY,
                    FORMAT_PGM,
                    FORMAT_PPM,
                    FORMAT_NUMERIC
                };

            // PPM/PGM format
            bool YARP_sig_API read(ImageOf<PixelRgb>& dest,
                                   const yarp::os::ConstString& src);
            bool YARP_sig_API read(ImageOf<PixelBgr>& dest,
                                   const yarp::os::ConstString& src);
            bool YARP_sig_API read(ImageOf<PixelRgba>& dest,
                                   const yarp::os::ConstString& src);
            bool YARP_sig_API read(ImageOf<PixelMono>& dest,
                                   const yarp::os::ConstString& src);

            bool YARP_sig_API read(ImageOf<PixelFloat>& dest,
                                   const yarp::os::ConstString& src);

            // PPM/PGM format
            bool YARP_sig_API write(const ImageOf<PixelRgb>& src,
                                    const yarp::os::ConstString& dest);
            bool YARP_sig_API write(const ImageOf<PixelBgr>& src,
                                    const yarp::os::ConstString& dest);
            bool YARP_sig_API write(const ImageOf<PixelRgba>& src,
                                    const yarp::os::ConstString& dest);
            bool YARP_sig_API write(const ImageOf<PixelMono>& src,
                                    const yarp::os::ConstString& dest);

            bool YARP_sig_API write(const ImageOf<PixelFloat>& src,
                                    const yarp::os::ConstString& dest);

            bool YARP_sig_API write(const Image& src,
                                    const yarp::os::ConstString& dest);
        }
    }
}

#endif // YARP_SIG_IMAGEFILE_H
