// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


//
// $Id: SoundFile.h,v 1.2 2006-10-24 19:51:03 eshuy Exp $
//
//

#ifndef YARP2SoundFile_INC
#define YARP2SoundFile_INC

#include <yarp/sig/Sound.h>

namespace yarp {
    namespace sig{
        /**
         * \ingroup sig_class
         *
         * Sound file operations.
         */
        namespace file {
            bool read(Sound& dest, const char *src);
            bool write(const Sound& src, const char *dest);
        }
    }
};

#endif
