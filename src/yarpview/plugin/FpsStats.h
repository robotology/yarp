/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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
    /*! \brief This function returns the Minimum, Maximum and Average values
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
