/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERCONTROLMODE_H
#define YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERCONTROLMODE_H

#include <yarp/dev/IControlMode.h>

#include "ControlBoardWrapperCommon.h"

class ControlBoardWrapperControlMode :
        virtual public ControlBoardWrapperCommon,
        public yarp::dev::IControlMode
{
public:
    bool getControlMode(int j, int* mode) override;
    bool getControlModes(int* modes) override;
    bool getControlModes(const int n_joint, const int* joints, int* modes) override;
    bool setControlMode(const int j, const int mode) override;
    bool setControlModes(const int n_joints, const int* joints, int* modes) override;
    bool setControlModes(int* modes) override;
};

#endif // YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERCONTROLMODE_H
