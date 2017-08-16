/*
 * Copyright (C) 2013 Istituto Italiano di Tecnologia (IIT)
 * Authors: Marco Randazzo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "PortAudioBuffer.h"

#include <cstdio>
#include <cstdlib>
#include <portaudio.h>


circularBuffer::circularBuffer(int bufferSize)
{
    maxsize  = bufferSize + 1;
    start = 0;
    end   = 0;
    elems = (SAMPLE *) calloc(maxsize, sizeof(SAMPLE));
}

circularBuffer::~circularBuffer()
{
    free(elems);
}


