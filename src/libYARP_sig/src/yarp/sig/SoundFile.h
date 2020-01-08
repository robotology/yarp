/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
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
             * Read a sound from file.
             * @param dest sound to read
             * @param src name of file (should be a WAV file)
             * @return true on success
             */
            bool YARP_sig_API read(Sound& dest, const char *src);

            /**
             * Write a sound to file.
             * @param src sound to write
             * @param dest name of file (will be a WAV file)
             * @return true on success
             */
            bool YARP_sig_API write(const Sound& src, const char *dest);

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
