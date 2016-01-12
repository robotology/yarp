// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_IOEXCEPTION
#define YARP2_IOEXCEPTION

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

