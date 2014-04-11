// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2014 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Alberto Cardellino
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __YARP_IMPLEMENT_CONTROL2_MODE__
#define __YARP_IMPLEMENT_CONTROL2_MODE__

#include <yarp/dev/IControlMode2.h>
#include <yarp/dev/api.h>

namespace yarp {
    namespace dev {
    class ImplementControlMode2;
    }
}

class YARP_dev_API yarp::dev::ImplementControlMode2: public IControlMode2
{
    void *helper;
    int *temp_int;
    int *temp_mode;

    yarp::dev::IControlMode2Raw *raw;
public:
    bool initialize(int k, const int *amap);
    bool uninitialize();
    ImplementControlMode2(IControlMode2Raw *v);

    /**
     * Destructor.
     */
    virtual  ~ImplementControlMode2();

    // Control mode
    bool setTorqueMode(int j);
    bool setImpedancePositionMode(int j);
    bool setImpedanceVelocityMode(int j);
    bool setOpenLoopMode(int j);
    bool setPositionMode(int j);
    bool setVelocityMode(int j);
    bool getControlMode(int j, int *f);
    bool getControlModes(int *modes);
    // Control Mode 2
    bool getControlModes(const int n_joint, const int *joints, int *modes);
    bool setControlMode(const int j, const int mode);
    bool setControlModes(const int n_joint, const int *joints, int *modes);
    bool setControlModes(int *modes);
};


#endif


