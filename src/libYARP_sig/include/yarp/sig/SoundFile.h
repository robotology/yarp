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
            bool YARP_sig_API read(Sound& dest, const char *src);

            /**
             * Write a sound to file.
             * @param src sound to write
             * @param dest name of file (will be a WAV file)
             * @return true on success
             */
            bool YARP_sig_API write(const Sound& src, const char *dest);

            class soundStreamReader
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

#endif
