/*
 * Copyright (C) 2011 Robotics Brain and Cognitive Sciences Department, Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo and Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_IMPLEMENT_CONTROL_MODE
#define YARP_IMPLEMENT_CONTROL_MODE

#include <yarp/dev/IControlMode.h>
#include <yarp/dev/api.h>

namespace yarp {
    namespace dev {
    class ImplementControlMode;
    }
}

class YARP_dev_API yarp::dev::ImplementControlMode: public IControlMode
{
    void *helper;
    yarp::dev::IControlModeRaw *raw;
public:
    bool initialize(int k, const int *amap);
    bool uninitialize();
    ImplementControlMode(IControlModeRaw *v);
    ~ImplementControlMode();
    bool setTorqueMode(int j);
    bool setImpedancePositionMode(int j);
    bool setImpedanceVelocityMode(int j);
    bool setOpenLoopMode(int j);
    bool setPositionMode(int j);
    bool setVelocityMode(int j);
    bool getControlMode(int j, int *f);
    bool getControlModes(int *modes);
};

#endif
