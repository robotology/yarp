// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author Lorenzo Natale
 * email:   lorenzo.natale@robotcub.org
 * website: www.robotcub.org
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef __IMGPORT__
#define __IMGPORT__

#include <yarp/sig/Image.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/os/Time.h>

#include <string>

#include "ViewerResources.h"

class FpsStats;

class InputCallback: public yarp::os::TypedReaderCallback<yarp::sig::FlexImage>
{
public:
    InputCallback();
    ~InputCallback();

    bool attach(ViewerResources *v);
    bool attach(FpsStats *v);

    void mustDraw(bool f);

    using yarp::os::TypedReaderCallback<yarp::sig::FlexImage>::onRead;
    virtual void onRead(yarp::sig::FlexImage &img);

    void resetStats();
    void getStats(unsigned int &it, double &av, double &max, double &min);

private:
    ViewerResources *viewer;
    FpsStats *fpsData;
    unsigned int count;
    double prev;
    double now;
    double maxDT;
    double minDT;
    double accDT;
    bool mustDrawF;
};

#endif
