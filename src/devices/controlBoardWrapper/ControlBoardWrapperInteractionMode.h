/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERINTERACTIONMODE_H
#define YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERINTERACTIONMODE_H

#include <yarp/dev/IInteractionMode.h>

#include "ControlBoardWrapperCommon.h"


class ControlBoardWrapperInteractionMode :
        virtual public ControlBoardWrapperCommon,
        public yarp::dev::IInteractionMode
{
public:
    bool getInteractionMode(int j, yarp::dev::InteractionModeEnum* mode) override;
    bool getInteractionModes(int n_joints, int* joints, yarp::dev::InteractionModeEnum* modes) override;
    bool getInteractionModes(yarp::dev::InteractionModeEnum* modes) override;
    bool setInteractionMode(int j, yarp::dev::InteractionModeEnum mode) override;
    bool setInteractionModes(int n_joints, int* joints, yarp::dev::InteractionModeEnum* modes) override;
    bool setInteractionModes(yarp::dev::InteractionModeEnum* modes) override;
};

#endif // YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERINTERACTIONMODE_H
