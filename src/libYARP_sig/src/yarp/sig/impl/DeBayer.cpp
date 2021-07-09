/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/impl/DeBayer.h>
#include <yarp/os/Log.h>

bool deBayer_GRBG8_TO_BGR(yarp::sig::Image &source, yarp::sig::Image &dest, int pixelSize)
{
    yAssert(((pixelSize == 3) && (dest.getPixelCode() == VOCAB_PIXEL_BGR)) ||
        ((pixelSize == 4 && dest.getPixelCode() == VOCAB_PIXEL_BGRA)))

        dest.resize(source.width(), source.height());
    // perform conversion, skip borders
    for (size_t r = 0; r < source.height() - 2; r += 2)
    {
        unsigned char *destRow = dest.getRow(r);
        unsigned char *sourceRowCurrent = source.getRow(r);
        unsigned char *sourceRowNext = source.getRow(r + 1);

        //row i GRGRGR...
        for (size_t c = 0; c < dest.width() - 2; c += 2)
        {
            //source is on G pixel
            destRow[0] = sourceRowNext[0];       //blue
            destRow[1] = sourceRowCurrent[0];    //green
            destRow[2] = sourceRowCurrent[1];    //red

            //jump a pixel in destination
            destRow += pixelSize;
            sourceRowCurrent++;
            sourceRowNext++;

            //source is now on R pixel
            destRow[0] = sourceRowNext[0];         //blue
            destRow[1] = sourceRowCurrent[1];     //green
            destRow[2] = sourceRowCurrent[0];     //red

            destRow += pixelSize;
            sourceRowCurrent++;
            sourceRowNext++;
        }

        destRow = dest.getRow(r + 1);
        sourceRowCurrent = source.getRow(r + 1);
        sourceRowNext = source.getRow(r + 2);

        //row is now BGBGBG...
        for (size_t c = 0; c < dest.width() - 2; c += 2)
        {
            //source is on B pixel
            destRow[0] = sourceRowCurrent[0];   //blue
            destRow[1] = sourceRowCurrent[1];   //green
            destRow[2] = sourceRowNext[1];;     //red

            //jump a pixel in destination
            destRow += pixelSize;
            sourceRowCurrent++;
            sourceRowNext++;

            //source is now on G pixel
            destRow[0] = sourceRowCurrent[1];    //blue
            destRow[1] = sourceRowCurrent[0];    //green
            destRow[2] = sourceRowNext[0];       //red

            destRow += pixelSize;
            sourceRowCurrent++;
            sourceRowNext++;
        }
    }
    return true;
}

bool deBayer_GRBG8_TO_RGB(yarp::sig::Image &source, yarp::sig::Image &dest, int pixelSize)
{
    yAssert(((pixelSize == 3) && (dest.getPixelCode() == VOCAB_PIXEL_RGB)) ||
    ((pixelSize == 4 && dest.getPixelCode() == VOCAB_PIXEL_RGBA)))

    dest.resize(source.width(), source.height());
    // perform conversion, skip borders
    for (size_t r = 0; r < source.height() - 2; r += 2)
    {
        unsigned char *destRow = dest.getRow(r);
        unsigned char *sourceRowCurrent = source.getRow(r);
        unsigned char *sourceRowNext = source.getRow(r + 1);

        //row i GRGRGR...
        for (size_t c = 0; c < source.width() - 2; c += 2)
        {
            //source is on G pixel
            destRow[0] = sourceRowCurrent[1];    //red
            destRow[1] = sourceRowCurrent[0];    //green
            destRow[2] = sourceRowNext[0];;     //blue

            //jump a pixel in destination
            destRow += pixelSize;
            sourceRowCurrent++;
            sourceRowNext++;

            //source is now on R pixel
            destRow[0] = sourceRowCurrent[0];     //red
            destRow[1] = sourceRowCurrent[1];     //green
            destRow[2] = sourceRowNext[0];        //red

            destRow += pixelSize;
            sourceRowCurrent++;
            sourceRowNext++;
        }

        destRow = dest.getRow(r + 1);
        sourceRowCurrent = source.getRow(r + 1);
        sourceRowNext = source.getRow(r + 2);

        //row is now BGBGBG...
        for (size_t c = 0; c < dest.width() - 2; c += 2)
        {
            //source is on B pixel
            destRow[0] = sourceRowNext[1];      //red
            destRow[1] = sourceRowCurrent[1];   //green
            destRow[2] = sourceRowCurrent[0];   //blue

            //jump a pixel in destination
            destRow += pixelSize;
            sourceRowCurrent++;
            sourceRowNext++;

            //source is now on G pixel
            destRow[0] = sourceRowNext[0];     //red
            destRow[1] = sourceRowCurrent[0];  //green
            destRow[2] = sourceRowCurrent[1];  //blue

            destRow += pixelSize;
            sourceRowCurrent++;
            sourceRowNext++;
        }
    }
    return true;
}

bool deBayer_BGGR8_TO_RGB(yarp::sig::Image &source, yarp::sig::Image &dest, int pixelSize)
{
    YARP_FIXME_NOTIMPLEMENTED("convert_BGGR8_TO_RGB\n");
    return false;
}

bool deBayer_RGGB8_TO_RGB(yarp::sig::Image &source, yarp::sig::Image &dest, int pixelSize)
{
    YARP_FIXME_NOTIMPLEMENTED("convert_RGGB8_TO_RGB\n");
    return false;
}

bool deBayer_BGGR8_TO_BGR(yarp::sig::Image &source, yarp::sig::Image &dest, int pixelSize)
{
    YARP_FIXME_NOTIMPLEMENTED("convert_BGGR8_TO_BGR\n");
    return false;
}

bool deBayer_RGGB8_TO_BGR(yarp::sig::Image &source, yarp::sig::Image &dest, int pixelSize)
{
    YARP_FIXME_NOTIMPLEMENTED("convert_RGGB8_TO_BGR\n");
    return false;
}
