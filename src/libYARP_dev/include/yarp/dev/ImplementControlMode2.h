/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
    int nj;

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
