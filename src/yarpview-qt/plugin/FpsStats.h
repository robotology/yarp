// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

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

#ifndef FPSSTATS
#define FPSSTATS

#include <yarp/os/Time.h>

/*! \class FpsStats
    \brief Class used for calculate the fps
*/
class FpsStats
{
public:
    double average;
    double max;
    double min;
    unsigned int iterations;
    double t0;
    double prev;
    
    FpsStats(){
        reset();
    }

    /*! \brief This function must be called each time you want to reset the acquiring data period
        Typically if you want to track fps in temporal range of 3 seconds for example, you have to
        call this function every 3 seconds in order to reset the statistics
    */
    void reset(){
        average=0;
        max=0;
        min=1e20;
        iterations=0;
        t0=prev=yarp::os::Time::now();
    }

    /*! \brief This function must be called each time you want to track a frame.
        Typically you call this function each time you receive a frame if you want to track the receive
        Fps, or every time you display a frame if you want to track the display Fps
    */
    void update(){
        double now=yarp::os::Time::now();

        if (iterations>0){
            double dt=now-prev;

            if (dt>max)
                max=dt;
            if (dt<min)
                min=dt;
        }

        prev=now;
        iterations++;
    }
    /*! \brief This function returns the Minimun, Maximum and Average values
        \param av the average value
        \param m the minimum value
        \param M the maximum value
    */
    void getStats(double &av, double &m, double &M)
    {
        if (iterations>0){
            av=(yarp::os::Time::now()-t0)/iterations;
            m=min;
            M=max;
        }
        else{
            av=0;
            m=0;
            M=0;
        }
    }
    
    /*! \brief This function returns the Average value
        \param av the average value
    */
    void getStats(double &av){
        if (iterations>0)
            av=(yarp::os::Time::now()-t0)/iterations;
        else
            av=0;
    }
};

#endif
