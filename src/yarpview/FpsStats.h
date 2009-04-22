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

#ifndef __FPSSTATS__
#define __FPSSTATS__

class FpsStats
{
public:
    double average;
    double max;
    double min;
    unsigned int iterations;
    double now;
    double prev;
    double accDt;
    
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
        prev=0;
        now=0;
        accDt=0;
    }

    void update(double nt)
    {
        now=nt;
        if (iterations>0)
        {
            double tmp=now-prev;
            accDt+=tmp;
            if (tmp>max)
                max=tmp;
            if (tmp<min)
                min=tmp;
        }

        prev=nt;

        iterations++;
    }

    void getStats(double &av, double &m, double &M)
    {
        if (iterations<2)
        {
            av=0;
            m=0;
            M=0;
        }
        else
        {
            av=accDt/(iterations-1);
            m=min;
            M=max;
        }
    }
    
    void getStats(double &av)
    {
        if (iterations<2)
            av=0;
        else
            av=accDt/(iterations-1);
    }
};

#endif
