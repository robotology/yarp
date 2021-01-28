/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
