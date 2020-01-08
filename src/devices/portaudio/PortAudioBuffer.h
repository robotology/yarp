/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
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

#include <string>
#include <portaudio.h>
#include <yarp/dev/AudioBufferSize.h>
#include <yarp/dev/CircularAudioBuffer.h>
#include <cstdio>
/*
// Select sample format.
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


class yarp::dev::circularBuffer
{
    std::string  name;
    yarp::dev::audio_buffer_size  maxsize;
    int          start;
    int          end;
    SAMPLE       *elems;

    public:
    inline bool isFull()
    {
        return (end + 1) % maxsize.size == start;
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
        end = (end + 1) % maxsize.size;
        if (end == start)
        {
            printf ("ERROR: %s buffer ovverrun!\n", name.c_str());
            start = (start + 1) % maxsize.size; // full, overwrite
        }
    }

    inline audio_buffer_size size()
    {
        int i;
        if (end>start)
            i = end-start;
        else if (end==start)
            i = 0;
        else
            i = maxsize.size - start + end;
        return audio_buffer_size(i, maxsize.m_channels, sizeof(SAMPLE)); //not sure about this
    }

    inline SAMPLE read()
    {
        if (end == start)
        {
            printf ("ERROR: %s buffer underrun!\n", name.c_str());
        }
        SAMPLE elem = elems[start];
        start = (start + 1) % maxsize.size;
        return elem;
    }

    inline yarp::dev::audio_buffer_size getMaxSize()
    {
        return maxsize;
    }

    inline void clear()
    {
        start = 0;
        end   = 0;
    }

    circularBuffer(std::string buffer_name, yarp::dev::audio_buffer_size bufferSize);
    ~circularBuffer();

};
*/
//----------------------------------------------------------------------------------

struct circularDataBuffers
{
    yarp::dev::CircularAudioBuffer_16t* playData = nullptr;
    yarp::dev::CircularAudioBuffer_16t* recData = nullptr;
    bool canPlay = false;
    bool canRec = false;
    size_t numPlayChannels = 1;
    size_t numRecChannels = 1;
};

#endif
