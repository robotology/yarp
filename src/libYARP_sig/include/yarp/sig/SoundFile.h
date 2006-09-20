// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

//
// $Id: SoundFile.h,v 1.1 2006-09-20 17:48:17 eshuy Exp $
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
