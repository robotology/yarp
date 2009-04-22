// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/* 
 * Copyright (C) 2009 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author Lorenzo Natale
 * email:   lorenzo.natale@robotcub.org
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
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
