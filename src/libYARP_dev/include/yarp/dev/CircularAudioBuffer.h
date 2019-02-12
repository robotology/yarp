/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
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
