// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_LOCALREADER_
#define _YARP2_LOCALREADER_

#include <yarp/os/PortReader.h>

namespace yarp {
    namespace os {
        template <class T> class LocalReader;
    }
}

/**
 *
 * Under development.
 *
 */
template <class T>
class yarp::os::LocalReader {
public:

    virtual ~LocalReader() {}

    /**
     * Send object obj; expect callback to wrapper
     * when obj is no longer in use.
     */
    virtual bool acceptObject(T *obj,
                              PortWriter *wrapper) = 0;

    /**
     * Sender is no longer interested in getting callbacks.
     */
    virtual bool forgetObject(T *obj,
                              PortWriter *wrapper) = 0;
};


#endif

