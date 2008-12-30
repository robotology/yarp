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

InputCallback::InputCallback()
{
    viewer=0;
    fpsData=0;
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

            g_idle_add(forceDraw, 0);
 //         viewer->draw(mainWindow);
 //         viewer->invalidateDrawArea();

            // fprintf(stderr, ".");
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