// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <yarp/dev/ControlBoardInterfaces.h>

using namespace yarp::dev;

Pid::Pid(double kp, double kd, double ki, 
        double int_max, double scale, double out_max)
{

}
Pid::~Pid()
{

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
