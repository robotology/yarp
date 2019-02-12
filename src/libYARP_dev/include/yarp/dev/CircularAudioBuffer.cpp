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

#include "PortAudioBuffer.h"

#include <cstdio>
#include <cstdlib>
#include <portaudio.h>

using namespace yarp::dev;

circularBuffer::circularBuffer(std::string buffer_name, yarp::dev::audio_buffer_size bufferSize)
{
    name = buffer_name;
    maxsize = bufferSize;
    maxsize.size += 1;
    start = 0;
    end   = 0;
    elems = (SAMPLE *) calloc(maxsize.size, sizeof(SAMPLE));
}

circularBuffer::~circularBuffer()
{
    free(elems);
}

