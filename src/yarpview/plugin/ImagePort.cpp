/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

// ImagePort.cpp: implementation of the ImagePort class.
//
//////////////////////////////////////////////////////////////////////

#include "ImagePort.h"
#include <yarp/sig/ImageFile.h>

using namespace yarp::os;

#include <sstream>

InputCallback::InputCallback()
{
    sigHandler = nullptr;
    counter = 0;
}

InputCallback::~InputCallback() = default;

/*! \brief the function callback
    \param img the image received
*/
#ifdef YARP_LITTLE_ENDIAN
void InputCallback::onRead(yarp::sig::ImageOf<yarp::sig::PixelBgra> &img)
#else
void InputCallback::onRead(yarp::sig::ImageOf<yarp::sig::PixelRgba> &img)
#endif
{

    uchar *tmpBuf;
    QSize s = (QSize(img.width(),img.height()));
    int imgSize = img.getRawImageSize();

    // Allocate a QVideoFrame
    QVideoFrame frame(imgSize, s, img.getRowSize(), QVideoFrame::Format_RGB32);

    // Maps the buffer
    frame.map(QAbstractVideoBuffer::WriteOnly);
    // Takes the ownership of the buffer in write only mode
    tmpBuf = frame.bits();
    unsigned char *rawImg = img.getRawImage();
    //int j = 0;
    // Inverts the planes because Qt Wants an image in RGB format instead of BGR
   /* for(int i=0; i<imgSize; i++){
        tmpBuf[j+2] = rawImg[i];
        i++;
        tmpBuf[j+1] = rawImg[i];
        i++;
        tmpBuf[j] = rawImg[i];
        tmpBuf[j+3] = 0;
        j+=4;
    }*/

    if (img.topIsLowIndex()) {
        memcpy(tmpBuf, rawImg, imgSize);
    } else {
        for(int x = 0; x < s.height(); x++) {
            memcpy(tmpBuf + x * img.getRowSize(),
                   rawImg + (s.height() - x - 1) * img.getRowSize(),
                   img.getRowSize());
        }
    }

    //unmap the buffer
    frame.unmap();
    if(sigHandler){
        sigHandler->sendVideoFrame(frame);
    }

}

/*! \brief sets the signalhandler to the class
    \param handler the signal handler
*/
void InputCallback::setSignalHandler(SignalHandler *handler)
{
    sigHandler = handler;
}
