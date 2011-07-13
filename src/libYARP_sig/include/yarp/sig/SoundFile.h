// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


//
// $Id: SoundFile.h,v 1.3 2007-03-20 11:29:32 eshuy Exp $
//
//

#ifndef YARP2SoundFile_INC
#define YARP2SoundFile_INC

#include <yarp/sig/Sound.h>

namespace yarp {
    namespace sig{
        namespace file {
            /**
             * Read a sound from file.
             * @param dest sound to read
             * @param src name of file (should be a WAV file)
             * @return true on success
             */
            bool read(Sound& dest, const char *src);

            /**
             * Write a sound to file.
             * @param src sound to write
             * @param dest name of file (will be a WAV file)
             * @return true on success
             */
            bool write(const Sound& src, const char *dest);
        }
    }
};

#endif
