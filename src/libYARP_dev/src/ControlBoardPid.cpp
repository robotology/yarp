// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Lorenzo Natale, Giorgio Metta
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/dev/ControlBoardInterfaces.h>

using namespace yarp::dev;

Pid::Pid(double p, double d, double i, 
         double intm, double sc, double omax)
{
    kp=p;
    kd=d;
    ki=i;
    scale=sc;
    max_int=intm;
    max_output=omax;
    offset=0;
}

Pid::~Pid()
{

}

Pid::Pid()
{
    kp=0;
    kd=0;
    ki=0;
    scale=0;
    max_int=0;
    max_output=0;
    offset=0;
}

void Pid::setKp(double p)
{
    kp=p;
}

void Pid::setKi(double i)
{
    ki=i;
}

void Pid::setKd(double d)
{
    kd=d;
}

void Pid::setMaxInt(double m)
{
    max_int=m;
}

void Pid::setScale(double sc)
{
    scale=sc;
}

void Pid::setMaxOut(double m)
{
    max_output=m;
}

void Pid::setOffset(double o)
{
    offset=o;
}
