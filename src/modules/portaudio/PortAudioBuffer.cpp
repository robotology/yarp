// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "PortAudioBuffer.h"

#include <stdio.h>
#include <stdlib.h>
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


