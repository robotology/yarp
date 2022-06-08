/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERAXISINFO_H
#define YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERAXISINFO_H

#include <yarp/dev/IAxisInfo.h>

#include "ControlBoardWrapperCommon.h"

class ControlBoardWrapperAxisInfo :
        virtual public ControlBoardWrapperCommon,
        public yarp::dev::IAxisInfo
{
public:
    bool getAxisName(int j, std::string& name) override;
    bool getJointType(int j, yarp::dev::JointTypeEnum& type) override;
};

#endif // YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERAXISINFO_H
