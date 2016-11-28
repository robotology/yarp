/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_IOEXCEPTION
#define YARP2_IOEXCEPTION

#include <yarp/os/ConstString.h>

namespace yarp {
    namespace os {
        namespace impl {
            class IOException;
        }
    }
}

/**
 * General communication error.
 */
class yarp::os::impl::IOException {
public:
    IOException(const char *txt = YARP_NULLPTR) {
        if (txt!=YARP_NULLPTR) {
            desc = txt;
        }
    }

    ConstString toString() const {
        return desc;
    }
private:
    ConstString desc;
};

#endif

