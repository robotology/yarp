/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CIRCULARAUDIOBUFFER_H
#define YARP_DEV_CIRCULARAUDIOBUFFER_H

#include <yarp/os/Log.h>
#include <yarp/dev/AudioBufferSize.h>
#include <cstdio>
#include <string>

#include <yarp/os/LogStream.h>

namespace yarp {
namespace dev {


template <typename SAMPLE>
class CircularAudioBuffer
{
    std::string name;
    yarp::dev::AudioBufferSize maxsize;
    size_t start;
    size_t end;
    SAMPLE *elems;

    public:
    bool isFull()
    {
        return (end + 1) % maxsize.size == start;
    }

    const SAMPLE* getRawData()
    {
        return elems;
    }

    bool isEmpty()
    {
        return end == start;
    }

    void write(SAMPLE elem)
    {
        elems[end] = elem;
        end = (end + 1) % maxsize.size;
        if (end == start)
        {
            printf ("ERROR: %s buffer overrun!\n", name.c_str());
            start = (start + 1) % maxsize.size; // full, overwrite
        }
    }

    AudioBufferSize size()
    {
        size_t i;
        if (end > start) {
            i = end-start;
        } else if (end == start) {
            i = 0;
        } else {
            i = maxsize.size - start + end;
        }
        return AudioBufferSize(i/maxsize.m_channels, maxsize.m_channels, sizeof(SAMPLE));
    }

    SAMPLE read()
    {
        if (end == start)
        {
            printf ("ERROR: %s buffer underrun!\n", name.c_str());
        }
        SAMPLE elem = elems[start];
        start = (start + 1) % maxsize.size;
        return elem;
    }

    yarp::dev::AudioBufferSize getMaxSize()
    {
        return maxsize;
    }

    void clear()
    {
        start = 0;
        end   = 0;
    }

    CircularAudioBuffer(std::string buffer_name, yarp::dev::AudioBufferSize bufferSize) :
            name{buffer_name},
            maxsize{bufferSize},
            start{0},
            end{0},
            elems{static_cast<SAMPLE*>(calloc(maxsize.size, sizeof(SAMPLE)))}
    {
        static_assert (std::is_same<unsigned char, SAMPLE>::value ||
                       std::is_same<unsigned short int, SAMPLE>::value ||
                       std::is_same<unsigned int, SAMPLE>::value,
                        "CircularAudioBuffer can be specialized only as <unsigned char>, <unsigned short int>, <unsigned int>");

        yAssert(bufferSize.m_depth == sizeof(SAMPLE));

        maxsize.size += 1;
    }

    ~CircularAudioBuffer()
    {
        free(elems);
    }

};

typedef yarp::dev::CircularAudioBuffer<unsigned char> CircularAudioBuffer_8t;
typedef yarp::dev::CircularAudioBuffer<unsigned short int> CircularAudioBuffer_16t;
typedef yarp::dev::CircularAudioBuffer<unsigned int> CircularAudioBuffer_32t;

} // namespace dev
} // namespace yarp

#endif // YARP_DEV_CIRCULARAUDIOBUFFER_H
