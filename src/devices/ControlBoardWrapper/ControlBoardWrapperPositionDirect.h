/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERPOSITIONDIRECT_H
#define YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERPOSITIONDIRECT_H

#include <yarp/dev/IPositionDirect.h>

#include "ControlBoardWrapperCommon.h"


class ControlBoardWrapperPositionDirect :
        virtual public ControlBoardWrapperCommon,
        public yarp::dev::IPositionDirect
{
public:
    inline bool getAxes(int *ax) override { return ControlBoardWrapperCommon::getAxes(ax); }
    bool setPosition(int j, double ref) override;
    bool setPositions(const int n_joints, const int* joints, const double* dpos) override;
    bool setPositions(const double* refs) override;
    bool getRefPosition(const int joint, double* ref) override;
    bool getRefPositions(double* refs) override;
    bool getRefPositions(const int n_joint, const int* joints, double* refs) override;
};

#endif // YARP_DEV_CONTROLBOARDWRAPPER_CONTROLBOARDWRAPPERPOSITIONDIRECT_H
