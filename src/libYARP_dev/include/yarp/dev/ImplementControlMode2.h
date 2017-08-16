/*
 * Copyright (C) 2014 Istituto Italiano di Tecnologia (IIT)
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
    YARP_DEPRECATED bool setTorqueMode(int j) override;
    YARP_DEPRECATED bool setImpedancePositionMode(int j) override;
    YARP_DEPRECATED bool setImpedanceVelocityMode(int j) override;
    YARP_DEPRECATED bool setPositionMode(int j) override;
    YARP_DEPRECATED bool setVelocityMode(int j) override;
#endif // YARP_NO_DEPRECATED
    bool getControlMode(int j, int *f) override;
    bool getControlModes(int *modes) override;
    // Control Mode 2
    bool getControlModes(const int n_joint, const int *joints, int *modes) override;
    bool setControlMode(const int j, const int mode) override;
    bool setControlModes(const int n_joint, const int *joints, int *modes) override;
    bool setControlModes(int *modes) override;
};

#if defined(_MSC_VER) && !defined(YARP_NO_DEPRECATED) // since YARP 2.3.70
YARP_WARNING_POP
#endif

#endif // YARP_DEV_IMPLEMENTCONTROLMODE2_H
