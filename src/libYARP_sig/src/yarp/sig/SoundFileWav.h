/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SIG_SOUNDFILE_WAV_H
#define YARP_SIG_SOUNDFILE_WAV_H

#include <yarp/sig/Sound.h>

namespace yarp {
    namespace sig{
        namespace file {
            /**
             * Read a sound from a .wav audio file.
             * @param data sound to read
             * @param filename name of file
             * @return true on success
             */
            bool YARP_sig_API read_wav(Sound& data, const char* filename);

            /**
             * Write a sound to a .wav file.
             * @param data sound to write
             * @param filename name of file
             * @return true on success
             */
            bool YARP_sig_API write_wav(const Sound& data, const char* filename);
        }
    }
}

#endif // YARP_SIG_SOUNDFILE_WAV_H
