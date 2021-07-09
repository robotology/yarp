/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_SOUNDFILE_H
#define YARP_SIG_SOUNDFILE_H

#include <yarp/sig/Sound.h>

namespace yarp {
    namespace sig{
        namespace file {
            /**
             * Read a sound from a generic audio file.
             * @param data sound to read
             * @param filename name of file (supported extensions: .wav, .mp3)
             * @return true on success
             */
            bool YARP_sig_API read(Sound& data, const char * filename);

            /**
             *Read a sound from a byte array.
            * @param data sound to read
            * @param bytestream the byte array
            * @return true on success
            */
            bool YARP_sig_API read_bytestream(Sound& data, const char* filename, size_t streamsize, std::string format);

            /**
             * Write a sound to file. The format is specified by the extension.
             * @param data sound to write
             * @param filename name of file (supported extensions: .wav, .mp3)
             * @return true on success
             */
            bool YARP_sig_API write(const Sound& data, const char * filename);

            class YARP_sig_API soundStreamReader
            {
                private:
                Sound  m_sound_data;
                size_t m_index=0;
                size_t m_totsize=0;

                public:
                soundStreamReader() {}
                ~soundStreamReader() {}

                bool   open(const char *filename);
                bool   close();
                size_t readBlock(Sound& dest, size_t block_size);
                bool   rewind(size_t sample_offset=0);
                size_t getIndex();
            };
        }
    }
}

#endif // YARP_SIG_SOUNDFILE_H
