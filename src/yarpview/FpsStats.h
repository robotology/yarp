// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author Lorenzo Natale
 * email:   lorenzo.natale@robotcub.org
 * website: www.robotcub.org
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef __FPSSTATS__
#define __FPSSTATS__

#include <yarp/os/Time.h>

class FpsStats
{
public:
    double average;
    double max;
    double min;
    unsigned int iterations;
    double t0;
    double prev;

    FpsStats()
    {
        reset();
    }

    void reset()
    {
        average=0;
        max=0;
        min=1e20;
        iterations=0;
        t0=prev=yarp::os::Time::now();
    }

    void update()
    {
        double now=yarp::os::Time::now();

        if (iterations>0)
        {
            double dt=now-prev;

            if (dt>max)
                max=dt;
            if (dt<min)
                min=dt;
        }

        prev=now;
        iterations++;
    }

    void getStats(double &av, double &m, double &M)
    {
        if (iterations>0)
        {
            av=(yarp::os::Time::now()-t0)/iterations;
            m=min;
            M=max;
        }
        else
        {
            av=0;
            m=0;
            M=0;
        }
    }

    void getStats(double &av)
    {
        if (iterations>0)
            av=(yarp::os::Time::now()-t0)/iterations;
        else
            av=0;
    }
};

#endif
