/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * Basic implementations of debayering functions. Used to convert Bayer images received
 * in a YARP port. Prototype implementation, in the future we should replace these functions
 * with better implementations, like the ones in the Bayer Carrier. If we decide to implement
 * debayering by chaining carriers this code could be removed completely.
 */

#ifndef YARP_SIG_IMPL_DEBAYER_H
#define YARP_SIG_IMPL_DEBAYER_H

#include <yarp/sig/Image.h>

inline bool isBayer8(int v)
{
    if ((v == VOCAB_PIXEL_ENCODING_BAYER_GRBG8) || (v == VOCAB_PIXEL_ENCODING_BAYER_BGGR8) || (v == VOCAB_PIXEL_ENCODING_BAYER_GBRG8) || (v == VOCAB_PIXEL_ENCODING_BAYER_RGGB8)) {
        return true;
    } else {
        return false;
    }
}

inline bool isBayer16(int v)
{
    if ((v == VOCAB_PIXEL_ENCODING_BAYER_GRBG16) || (v == VOCAB_PIXEL_ENCODING_BAYER_BGGR16) || (v == VOCAB_PIXEL_ENCODING_BAYER_GBRG16) || (v == VOCAB_PIXEL_ENCODING_BAYER_RGGB16)) {
        return true;
    } else {
        return false;
    }
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

#endif // YARP_SIG_IMPL_DEBAYER_H
