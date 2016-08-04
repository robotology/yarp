/*
 * Copyright (C) 2009 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author Lorenzo Natale
 * email:   lorenzo.natale@robotcub.org
 * website: www.robotcub.org
 *
 * Modified by: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
    sigHandler = NULL;
    counter = 0;
}

InputCallback::~InputCallback()
{}

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
#if QT_VERSION >= 0x050302
    int imgSize = img.getRawImageSize();
#else
    int imgSize = s.width() * s.height() * img.getPixelSize();
#endif

    // Allocate a QVideoFrame
    QVideoFrame frame(imgSize,
              s,
#if QT_VERSION >= 0x050302
              img.getRowSize(),
#else
              s.width() * img.getPixelSize(),
#endif
              QVideoFrame::Format_RGB32);

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

#if QT_VERSION >= 0x050302
    memcpy(tmpBuf,rawImg,imgSize);
#else
    for(int x =0; x < s.height(); x++) {
        memcpy(tmpBuf + x * (img.width() * img.getPixelSize()),
               rawImg + x * (img.getRowSize()),
               img.width() * img.getPixelSize());
    }
#endif

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

