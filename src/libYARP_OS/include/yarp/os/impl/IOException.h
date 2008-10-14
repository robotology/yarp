// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_IOEXCEPTION_
#define _YARP2_IOEXCEPTION_

#include <yarp/os/impl/String.h>

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
    IOException(const char *txt = NULL) {
        if (txt!=NULL) {
            desc = txt;
        }
    }

    String toString() const {
        return desc;
    }
private:
    String desc;
};

#endif

