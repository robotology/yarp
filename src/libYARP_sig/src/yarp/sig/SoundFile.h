/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SIG_SOUNDFILE_H
#define YARP_SIG_SOUNDFILE_H

#include <yarp/sig/Sound.h>
#include <cstdio>

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

            /**
             * Read a sound from a .mp3 audio file.
             * @param data sound to read
             * @param filename name of file
             * @return true on success
             */
            bool YARP_sig_API read_mp3(Sound& data, const char* filename);

            /**
             * Write a sound to a mp3 file.
             * @param data sound to write
             * @param filename name of file
             * @return true on success
             */
            bool YARP_sig_API write_mp3(const Sound& data, const char* filename);

            /**
             * Read a sound from a generic audio file.
             * @param data sound to read
             * @param filename name of file (supported extensions: .wav, .mp3)
             * @return true on success
             */
            bool YARP_sig_API read(Sound& data, const char * filename);

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
                FILE *fp;
                size_t index;
                char fname [255];
                struct
                {
                    int freq;
                    int channels;
                    int bits;
                    int samples;
                    size_t data_start_offset;
                } soundInfo;

                public:
                soundStreamReader()
                {
                    fp = 0;
                    index=0;
                    fname[0]=0;
                }

                ~soundStreamReader()
                {
                    if (fp)
                    {
                        close();
                        fp=0;
                    }
                }

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
