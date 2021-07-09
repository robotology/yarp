/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef PortAudioBufferh
#define PortAudioBufferh

#include <string>
#include <portaudio.h>
#include <yarp/dev/AudioBufferSize.h>
#include <yarp/dev/CircularAudioBuffer.h>
#include <cstdio>

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
