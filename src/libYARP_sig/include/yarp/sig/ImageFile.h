// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

//
// $Id: ImageFile.h,v 1.11 2006-11-10 10:44:44 eshuy Exp $
//
//

// paulfitz Wed May 30 18:35:14 EDT 2001
// pasa: June 2002, don't exit on open failure (gracefully returns -1, 0 if OK).// nat: May 06, adapted for YARP2

#ifndef YARP2ImageFile_INC
#define YARP2ImageFile_INC

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
                    FORMAT_NUMERIC,
                };

            // PPM/PGM format
            bool read(ImageOf<PixelRgb>& dest, const char *src);
            bool read(ImageOf<PixelBgr>& dest, const char *src);
            bool read(ImageOf<PixelMono>& dest, const char *src);

            // plain text format
            bool read(ImageOf<PixelFloat>& dest, const char *src);

            // PPM/PGM format
            bool write(const ImageOf<PixelRgb>& src, const char *dest);
            bool write(const ImageOf<PixelBgr>& src, const char *dest);
            bool write(const ImageOf<PixelMono>& src, const char *dest);

            // plain text format
            bool write(const ImageOf<PixelFloat>& src, const char *dest);

            bool write(const Image& src, const char *dest);
        }
    }
};

#endif
