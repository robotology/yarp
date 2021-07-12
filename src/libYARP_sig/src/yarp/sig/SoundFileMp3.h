/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_SOUNDFILE_MP3_H
#define YARP_SIG_SOUNDFILE_MP3_H

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
            bool YARP_sig_API read_mp3_bytestream(Sound& data, const char* bytestream, size_t streamsize);

            /**
             * Read a sound from a .mp3 audio file.
             * @param data sound to read
             * @param filename name of file
             * @return true on success
             */
            bool YARP_sig_API read_mp3_file(Sound& data, const char* filename);

            /**
             * Write a sound to a mp3 file.
             * @param data sound to write
             * @param filename name of file
             * @param bitrate the compressed audio bitrate
             * @return true on success
             */
            bool YARP_sig_API write_mp3_file(const Sound& data, const char* filename, size_t bitrate=64000);
        }
    }
}

#endif // YARP_SIG_SOUNDFILE_MP3_H
