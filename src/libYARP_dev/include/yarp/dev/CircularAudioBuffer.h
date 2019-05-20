/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef yarp_dev_circular_audioBufferh
#define yarp_dev_circular_audioBufferh

#include <string>
#include <yarp/dev/AudioBufferSize.h>
#include <cstdio>
#include <yarp/os/Log.h>

//----------------------------------------------------------------------------------

namespace yarp {
    namespace dev {

template <typename SAMPLE>
class CircularAudioBuffer
{
    std::string  name;
    yarp::dev::AudioBufferSize  maxsize;
    size_t       start;
    size_t       end;
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
            printf ("ERROR: %s buffer overrun!\n", name.c_str());
            start = (start + 1) % maxsize.size; // full, overwrite
        }
    }

    inline AudioBufferSize size()
    {
        size_t i;
        if (end>start)
            i = end-start;
        else if (end==start)
            i = 0;
        else
            i = maxsize.size - start + end;
        return AudioBufferSize(i/maxsize.m_channels, maxsize.m_channels, sizeof(SAMPLE));
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

    inline yarp::dev::AudioBufferSize getMaxSize()
    {
        return maxsize;
    }

    inline void clear()
    {
        start = 0;
        end   = 0;
    }

    CircularAudioBuffer(std::string buffer_name, yarp::dev::AudioBufferSize bufferSize)
    {
        static_assert (std::is_same<unsigned char, SAMPLE>::value ||
                       std::is_same<unsigned short int, SAMPLE>::value ||
                       std::is_same<unsigned int, SAMPLE>::value,
                        "CircularAudioBuffer can be specialized only as <unsigned char>, <unsigned short int>, <unsigned int>");
        
        yAssert(bufferSize.m_depth == sizeof(SAMPLE));

        name = buffer_name;
        maxsize = bufferSize;
        maxsize.size += 1;
        start = 0;
        end = 0;
        elems = (SAMPLE *)calloc(maxsize.size, sizeof(SAMPLE));
    }

    ~CircularAudioBuffer()
    {
        free(elems);
    }

};

typedef yarp::dev::CircularAudioBuffer<unsigned char> CircularAudioBuffer_8t;
typedef yarp::dev::CircularAudioBuffer<unsigned short int> CircularAudioBuffer_16t;
typedef yarp::dev::CircularAudioBuffer<unsigned int> CircularAudioBuffer_32t;

    }
}

#endif
