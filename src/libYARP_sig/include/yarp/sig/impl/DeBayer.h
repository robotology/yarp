// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2015 Istituto Italiano di Tecnologia, iCub Facility
* Authors: Lorenzo Natale
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
* 
* Basic implementations of debayering functions. Used to convert Bayer images received
* in a YARP port. Prototype implementation, in the future we should replace these functions
* with better implementations, like the ones in the Bayer Carrier. If we decide to implement
* debayering by chaining carriers this code could be removed completely.
*/

#ifndef _YARP_IMAGE_DEBAYER_
#define _YARP_IMAGE_DEBAYER_

#include <yarp/sig/Image.h>

inline bool isBayer8(int v)
{
    if ((v == VOCAB_PIXEL_ENCODING_BAYER_GRBG8) ||
        (v == VOCAB_PIXEL_ENCODING_BAYER_BGGR8) ||
        (v == VOCAB_PIXEL_ENCODING_BAYER_GBRG8) ||
        (v == VOCAB_PIXEL_ENCODING_BAYER_RGGB8))
        return true;
    else
        return false;
}

inline bool isBayer16(int v)
{
    if ((v == VOCAB_PIXEL_ENCODING_BAYER_GRBG16) ||
        (v == VOCAB_PIXEL_ENCODING_BAYER_BGGR16) ||
        (v == VOCAB_PIXEL_ENCODING_BAYER_GBRG16) ||
        (v == VOCAB_PIXEL_ENCODING_BAYER_RGGB16))
        return true;
    else
        return false;
}

/*
* Nearest neighbor debayer implementation (warning: skip borders)
*/
bool deBayer_GRBG8_TO_RGB(yarp::sig::Image &source, yarp::sig::Image &dest, int pixelSize);

bool deBayer_BGGR8_TO_RGB(yarp::sig::Image &source, yarp::sig::Image &dest, int pixelSize);

bool deBayer_RGGB8_TO_RGB(yarp::sig::Image &source, yarp::sig::Image &dest, int pixelSize);

/*
* Nearest neighbor debayer implementation (warning: skip borders)
*/
bool deBayer_GRBG8_TO_BGR(yarp::sig::Image &source, yarp::sig::Image &dest, int pixelSize);

bool deBayer_BGGR8_TO_BGR(yarp::sig::Image &source, yarp::sig::Image &dest, int pixelSize);

inline bool deBayer_RGGB8_TO_BGR(yarp::sig::Image &source, yarp::sig::Image &dest, int pixelSize);

#endif
