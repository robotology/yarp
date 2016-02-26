/*
 * Copyright (C) 2006, 2008, 2009 RobotCub Consortium
 * Authors: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_OPEN_LOOP_IMPL
#define YARP_OPEN_LOOP_IMPL

#include <yarp/dev/IOpenLoopControl.h>
#include <yarp/dev/api.h>

namespace yarp {
    namespace dev {
        class ImplementOpenLoopControl;
    }
}

class YARP_dev_API yarp::dev::ImplementOpenLoopControl: public IOpenLoopControl
{
    void *helper;
    yarp::dev::IOpenLoopControlRaw *raw;
    double *dummy;
public:
    bool initialize(int k, const int *amap);
    bool uninitialize();
    ImplementOpenLoopControl(IOpenLoopControlRaw *v);
    ~ImplementOpenLoopControl();
    bool setRefOutput(int j, double v);
    bool setRefOutputs(const double *v);
    bool getRefOutput(int j, double *v);
    bool getRefOutputs(double *v);
    bool getOutput(int j, double *v);
    bool getOutputs(double *v);
    bool setOpenLoopMode();
};

#endif
