/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
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
