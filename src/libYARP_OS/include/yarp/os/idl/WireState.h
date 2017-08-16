/*
 * Copyright (C) 2012 Istituto Italiano di Tecnologia (IIT)
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
