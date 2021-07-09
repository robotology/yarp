/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/idl/WireState.h>

#include <yarp/os/idl/Unwrapped.h>

using namespace yarp::os::idl;

WireState::WireState() :
        len(-1),
        code(-1),
        need_ok(false),
        parent(nullptr)
{
}

bool WireState::isValid() const
{
    return len >= 0;
}
