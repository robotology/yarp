// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
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

    yarpStringPtr getHandle() {
        return impl;
    }
private:
    yarpStringPtr impl;
};

#endif
