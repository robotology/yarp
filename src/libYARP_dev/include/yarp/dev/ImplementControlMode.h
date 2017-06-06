/*
 * Copyright (C) 2011 Robotics Brain and Cognitive Sciences Department, Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo and Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_IMPLEMENTCONTROLMODE_H
#define YARP_DEV_IMPLEMENTCONTROLMODE_H

#include <yarp/dev/IControlMode.h>
#include <yarp/dev/api.h>

namespace yarp {
    namespace dev {
    class ImplementControlMode;
    }
}

#if defined(_MSC_VER) && !defined(YARP_NO_DEPRECATED) // since YARP 2.3.70
// A class implementing setXxxxxMode(int) causes a warning on MSVC
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
#endif

class YARP_dev_API yarp::dev::ImplementControlMode: public IControlMode
{
    void *helper;
    yarp::dev::IControlModeRaw *raw;
public:
    bool initialize(int k, const int *amap);
    bool uninitialize();
    ImplementControlMode(IControlModeRaw *v);
    ~ImplementControlMode();
#ifndef YARP_NO_DEPRECATED // since YARP 2.3.70
    YARP_DEPRECATED bool setTorqueMode(int j);
    YARP_DEPRECATED bool setImpedancePositionMode(int j);
    YARP_DEPRECATED bool setImpedanceVelocityMode(int j);
    YARP_DEPRECATED bool setPositionMode(int j);
    YARP_DEPRECATED bool setVelocityMode(int j);
#endif // YARP_NO_DEPRECATED
    bool getControlMode(int j, int *f);
    bool getControlModes(int *modes);
};

#if defined(_MSC_VER) && !defined(YARP_NO_DEPRECATED) // since YARP 2.3.70
YARP_WARNING_POP
#endif

#endif // YARP_DEV_IMPLEMENTCONTROLMODE_H
