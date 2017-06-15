/*
 * Copyright (C) 2014 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Alberto Cardellino
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_DEV_IMPLEMENTCONTROLMODE2_H
#define YARP_DEV_IMPLEMENTCONTROLMODE2_H

#include <yarp/dev/IControlMode2.h>
#include <yarp/dev/api.h>

namespace yarp {
    namespace dev {
    class ImplementControlMode2;
    }
}

#if defined(_MSC_VER) && !defined(YARP_NO_DEPRECATED) // since YARP 2.3.70
// A class implementing setXxxxxMode(int) causes a warning on MSVC
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
#endif

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
#ifndef YARP_NO_DEPRECATED // since YARP 2.3.70
    YARP_DEPRECATED bool setTorqueMode(int j) YARP_OVERRIDE;
    YARP_DEPRECATED bool setImpedancePositionMode(int j) YARP_OVERRIDE;
    YARP_DEPRECATED bool setImpedanceVelocityMode(int j) YARP_OVERRIDE;
    YARP_DEPRECATED bool setPositionMode(int j) YARP_OVERRIDE;
    YARP_DEPRECATED bool setVelocityMode(int j) YARP_OVERRIDE;
#endif // YARP_NO_DEPRECATED
    bool getControlMode(int j, int *f) YARP_OVERRIDE;
    bool getControlModes(int *modes) YARP_OVERRIDE;
    // Control Mode 2
    bool getControlModes(const int n_joint, const int *joints, int *modes) YARP_OVERRIDE;
    bool setControlMode(const int j, const int mode) YARP_OVERRIDE;
    bool setControlModes(const int n_joint, const int *joints, int *modes) YARP_OVERRIDE;
    bool setControlModes(int *modes) YARP_OVERRIDE;
};

#if defined(_MSC_VER) && !defined(YARP_NO_DEPRECATED) // since YARP 2.3.70
YARP_WARNING_POP
#endif

#endif // YARP_DEV_IMPLEMENTCONTROLMODE2_H
