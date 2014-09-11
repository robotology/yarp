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
#include "FpsStats.h"

#include <yarp/sig/ImageFile.h>

using namespace yarp::os;

#include "yarpview.h"

#include <sstream>

InputCallback::InputCallback()
{
    viewer=0;
    fpsData=0;
    mustDrawF=false;
}

InputCallback::~InputCallback()
{}

void InputCallback::onRead(yarp::sig::FlexImage &img)
{
   if (fpsData!=0)
   {
       fpsData->update();
   }

    if (viewer!=0)
    {
            viewer->pushImage(img);

            if (mustDrawF)
            {
                gdk_threads_enter();
                viewer->invalidateDrawArea();
                saveImageNow();
                gdk_threads_leave ();
            } else {
                gdk_threads_enter();
                saveImageNow();
                gdk_threads_leave ();
            }
       }
}

bool InputCallback::attach(ViewerResources *v)
{
    if (viewer!=0)
        return false;
    viewer=v;

    return true;
}

bool InputCallback::attach(FpsStats *v)
{
    if (fpsData!=0)
        return false;
    fpsData=v;

    return true;
}

void InputCallback::mustDraw(bool f)
{
    mustDrawF=f;
}
