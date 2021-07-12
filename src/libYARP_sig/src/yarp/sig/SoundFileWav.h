/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_SOUNDFILE_WAV_H
#define YARP_SIG_SOUNDFILE_WAV_H

#include <yarp/sig/Sound.h>

namespace yarp {
    namespace sig{
        namespace file {
            /**
             * Read a sound from a byte array.
             * @param data sound to read
             * @param bytestream the byte array
             * @return true on success
             */
            bool YARP_sig_API read_wav_bytestream(Sound& data, const char* bytestream);

            /**
             * Read a sound from a .wav audio file.
             * @param data sound to read
             * @param filename name of file
             * @return true on success
             */
            bool YARP_sig_API read_wav_file(Sound& data, const char* filename);

            /**
             * Write a sound to a .wav file.
             * @param data sound to write
             * @param filename name of file
             * @return true on success
             */
            bool YARP_sig_API write_wav_file(const Sound& data, const char* filename);
        }
    }
}

#endif // YARP_SIG_SOUNDFILE_WAV_H
