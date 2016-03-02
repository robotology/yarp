/*
 * Copyright (C) 2007 RobotCub Consortium
 * Authors: Lorenzo Natale and Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARP2ImageFile_INC
#define YARP2ImageFile_INC

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

            // plain text format
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

            // plain text format
            bool YARP_sig_API write(const ImageOf<PixelFloat>& src,
                                    const yarp::os::ConstString& dest);

            bool YARP_sig_API write(const Image& src,
                                    const yarp::os::ConstString& dest);
        }
    }
}

#endif
