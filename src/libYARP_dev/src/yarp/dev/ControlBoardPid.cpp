/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/ControlBoardInterfaces.h>

using namespace yarp::dev;

Pid::Pid(double p, double d, double i,
         double intm, double sc, double omax):
    kp(p),
    kd(d),
    ki(i),
    max_int(intm),
    scale(sc),
    max_output(omax),
    offset(0),
    stiction_up_val(0),
    stiction_down_val(0),
    kff(0)
{}

Pid::Pid(double p, double d, double i,
         double intm, double sc, double omax, double st_up, double st_down, double ff) :
    kp(p),
    kd(d),
    ki(i),
    max_int(intm),
    scale(sc),
    max_output(omax),
    offset(0),
    stiction_up_val(st_up),
    stiction_down_val(st_down),
    kff(ff)
{}

Pid::~Pid() = default;

Pid::Pid()
{
    clear();
}

void Pid::clear()
{
    kp = 0;
    kd = 0;
    ki = 0;
    scale = 0;
    max_int = 0;
    max_output = 0;
    offset = 0;
    stiction_up_val = 0;
    stiction_down_val = 0;
    kff = 0;
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

void Pid::setStictionValues(double up_value, double down_value)
{
    stiction_up_val=up_value;
    stiction_down_val=down_value;
}

void Pid::setKff(double ff)
{
    kff=ff;
}

bool Pid::operator==(const yarp::dev::Pid &p) const
{

    if (kp != p.kp) {
        return false;
    }

    if (ki != p.ki) {
        return false;
    }

    if (kd != p.kd) {
        return false;
    }

    if (max_output != p.max_output) {
        return false;
    }

    if (max_int != p.max_int) {
        return false;
    }

    if (kff != p.kff) {
        return false;
    }

    if (offset != p.offset) {
        return false;
    }

    if (scale != p.scale) {
        return false;
    }

    if (stiction_down_val != p.stiction_down_val) {
        return false;
    }

    if (stiction_up_val != p.stiction_up_val) {
        return false;
    }

    return true;

}
