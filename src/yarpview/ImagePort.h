// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

// Dec 2008, Lorenzo Natale

#ifndef __IMGPORT__
#define __IMGPORT__

#include <yarp/sig/Image.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/os/Time.h>

#include "ViewerResources.h"

class FpsStats;

class InputCallback: public yarp::os::TypedReaderCallback<yarp::sig::FlexImage>
{
public:
    InputCallback();
    ~InputCallback();

    bool attach(ViewerResources *v);
    bool attach(FpsStats *v);

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
};

#endif