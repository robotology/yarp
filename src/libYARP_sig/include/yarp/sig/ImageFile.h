// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #pasa, paulfitz#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: ImageFile.h,v 1.5 2006-06-14 08:39:44 eshuy Exp $
///
///

/*
  paulfitz Wed May 30 18:35:14 EDT 2001
*/

/*
  pasa: June 2002, don't exit on open failure (gracefully returns -1, 0 if OK).
  nat: May 06, adapted for YARP2
*/

#ifndef YARP2ImageFile_INC
#define YARP2ImageFile_INC

#include <yarp/sig/Image.h>

/**
 * Image file operations.
 */
namespace yarp {
    namespace sig{
        namespace file {
            enum
                {
                    FORMAT_NULL,
                    FORMAT_ANY,
                    FORMAT_PGM,
                    FORMAT_PPM,
                    FORMAT_NUMERIC,
                };
            bool read(ImageOf<PixelRgb>& dest, const char *src);
            bool read(ImageOf<PixelBgr>& dest, const char *src);
            bool read(ImageOf<PixelMono>& dest, const char *src);

            bool write(const ImageOf<PixelRgb>& src, const char *dest);
            bool write(const ImageOf<PixelBgr>& src, const char *dest);
            bool write(const ImageOf<PixelMono>& src, const char *dest);
        }
    }
};

#endif
