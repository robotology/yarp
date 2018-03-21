/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_LOCALREADER_H
#define YARP_OS_LOCALREADER_H

#include <yarp/os/PortReader.h>

namespace yarp {
    namespace os {
        template <class T> class LocalReader;
        class PortWriter;
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
    virtual bool acceptObject(T *obj, PortWriter *wrapper) = 0;

    /**
     * Sender is no longer interested in getting callbacks.
     */
    virtual bool forgetObject(T *obj, PortWriter *wrapper) = 0;
};


#endif // YARP_OS_LOCALREADER_H

