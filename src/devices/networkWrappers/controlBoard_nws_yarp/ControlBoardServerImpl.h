/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef YARP_DEV_CONTROLBOARDSERVERIMPL_H
#define YARP_DEV_CONTROLBOARDSERVERIMPL_H

#include "ControlBoardMsgs.h"
#include <yarp/dev/ReturnValue.h>

class ControlBoardRPCd : public ControlBoardMsgs
{
    private:
    std::mutex                     m_mutex;

    public:
    ControlBoardRPCd()
    {
    }

    yarp::dev::ReturnValue dummyTest_RPC() override;

    std::mutex* getMutex() {return &m_mutex;}
};

#endif // YARP_DEV_CONTROLBOARDSERVERIMPL_H
