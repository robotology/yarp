// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
/* 
 * Copyright (C) 2009 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author Lorenzo Natale
 * email:   lorenzo.natale@robotcub.org
 * website: www.robotcub.org
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


// ImagePort.cpp: implementation of the ImagePort class.
//
//////////////////////////////////////////////////////////////////////

#include "ImagePort.h"
//#include "FpsStats.h"

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

void InputCallback::onRead(yarp::sig::FlexImage &img)
{

    uchar *tmpBuf;
    QSize s = (QSize(img.width(),img.height()));
    int imgSize = img.getRawImageSize();
    QVideoFrame frame(img.width() * img.height() * 4,
              s,
              s.width(),
              QVideoFrame::Format_RGB32);

    frame.map(QAbstractVideoBuffer::WriteOnly);

    tmpBuf = frame.bits();

    unsigned char *rawImg = img.getRawImage();
    int j = 0;
    for(int i=0; i<imgSize; i++){

        tmpBuf[j+2] = rawImg[i];
        i++;
        tmpBuf[j+1] = rawImg[i];
        i++;
        tmpBuf[j] = rawImg[i];
        tmpBuf[j+3] = 0;
        j+=4;


    }

    frame.unmap();
    if(sigHandler){
        sigHandler->sendVideoFrame(frame);
    }

}

void InputCallback::setSignalHandler(SignalHandler *handler)
{
    sigHandler = handler;
}

