// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008 Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


// ImagePort.cpp: implementation of the ImagePort class.
//
//////////////////////////////////////////////////////////////////////

#include "ImagePort.h"
#include "FpsStats.h"

#include <yarp/sig/ImageFile.h>

using namespace yarp::os;

#include "gCamView.h"

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
       fpsData->update(Time::now());
   }

    if (viewer!=0)
    {
            viewer->pushImage(img);

            if (mustDrawF)
            {
                gdk_threads_enter();
                viewer->invalidateDrawArea();
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
