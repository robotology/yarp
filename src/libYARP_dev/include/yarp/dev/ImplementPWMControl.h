/*
* Copyright (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
* Authors: Marco Randazzo <marco.randazzo@iit.it>
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#ifndef YARP_DEV_IMPLEMENTPWMCONTROL_H
#define YARP_DEV_IMPLEMENTPWMCONTROL_H

#include <yarp/dev/IPWMControl.h>
#include <yarp/dev/api.h>
#include <yarp/conf/system.h>

namespace yarp {
    namespace dev {
        class ImplementPWMControl;
    }
}

class YARP_dev_API yarp::dev::ImplementPWMControl: public IPWMControl
{
    void *helper;
    yarp::dev::IPWMControlRaw *raw;
    double *dummy;
public:
    bool initialize(int k, const int *amap, const double* dutyToPWM);
    bool uninitialize();
    ImplementPWMControl(IPWMControlRaw *v);
    ~ImplementPWMControl();
    bool setRefDutyCycle(int j, double v);
    bool setRefDutyCycles(const double *v);
    bool getRefDutyCycle(int j, double *v);
    bool getRefDutyCycles(double *v);
    bool getDutyCycle(int j, double *v);
    bool getDutyCycles(double *v);

};

#endif // YARP_DEV_IMPLEMENTPWMCONTROL_H
