/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IDL_WIRESTATE_H
#define YARP_OS_IDL_WIRESTATE_H

#include <yarp/os/api.h>

namespace yarp {
    namespace os {
        namespace idl {
            class WireState;
        }
    }
}

/**
 *
 * IDL-friendly state.
 *
 */
class YARP_OS_API yarp::os::idl::WireState {
public:
    int len;
    int code;
    bool need_ok;
    WireState *parent;

    WireState();

    bool isValid() const;
};


#endif // YARP_OS_IDL_WIRESTATE_H
