/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IDL_WIRESTATE_H
#define YARP_OS_IDL_WIRESTATE_H

#include <yarp/os/api.h>

namespace yarp::os::idl {

/**
 * IDL-friendly state.
 */
class YARP_os_API WireState
{
public:
    int len;
    int code;
    bool need_ok;
    WireState* parent;

    WireState();

    bool isValid() const;
};

} // namespace yarp::os::idl

#endif // YARP_OS_IDL_WIRESTATE_H
