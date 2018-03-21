/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARPCXX_ConstString_INC
#define YARPCXX_ConstString_INC

#include <yarp/yarpcxx.h>

#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>

namespace yarpcxx {
    namespace os {
        class ConstString;
    }
}

class yarpcxx::os::ConstString {
public:
    ConstString() {
        impl = yarpStringCreate();
        YARPCXX_VALID(impl);
    }

    ~ConstString() {
        yarpStringFree(impl);
    }

    const char *c_str() {
        return yarpStringToC(getHandle());
    }

    yarpStringPtr getHandle() {
        return impl;
    }
private:
    yarpStringPtr impl;
};

#endif
