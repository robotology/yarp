/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef PortAudioBufferh
#define PortAudioBufferh

#include <portaudio.h>
#include <cstdio>

/* Select sample format. */
#if 0
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#elif 1
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#elif 1
#define PA_SAMPLE_TYPE  paInt8
typedef char SAMPLE;
#define SAMPLE_SILENCE  (0)
#else
#define PA_SAMPLE_TYPE  paUInt8
typedef unsigned char SAMPLE;
#define SAMPLE_SILENCE  (128)
#define SAMPLE_UNSIGNED
#endif

//----------------------------------------------------------------------------------
class circularBuffer
{
    int         maxsize;
    int         start;
    int         end;
    SAMPLE      *elems;

    public:
    inline bool isFull()
    {
        return (end + 1) % maxsize == start;
    }

    inline const SAMPLE* getRawData()
    {
        return elems;
    }

    inline bool isEmpty()
    {
        return end == start;
    }

    inline void write(SAMPLE elem)
    {
        elems[end] = elem;
        end = (end + 1) % maxsize;
        if (end == start)
        {
            printf ("ERROR: buffer ovverrun!\n");
            start = (start + 1) % maxsize; // full, overwrite
        }
    }

    inline int size()
    {
        int i;
        if (end>start)
            i = end-start;
        else if (end==start)
            i = 0;
        else
            i = maxsize - start + end;
        return i;
    }

    inline SAMPLE read()
    {
        if (end == start)
        {
            printf ("ERROR: buffer underrun!\n");
        }
        SAMPLE elem = elems[start];
        start = (start + 1) % maxsize;
        return elem;
    }

    inline unsigned int getMaxSize()
    {
        return maxsize;
    }

    inline void clear()
    {
        start = 0;
        end   = 0;
    }

    circularBuffer(int bufferSize);
    ~circularBuffer();

};

//----------------------------------------------------------------------------------

struct circularDataBuffers
{
    circularBuffer*     playData;
    circularBuffer*     recData;
    bool                canPlay;
    bool                canRec;
    int                 numChannels;
    circularDataBuffers ()
    {
        numChannels=1;
        playData=0;
        recData=0;
        canPlay=false;
        canRec=false;
    }
};

#endif
