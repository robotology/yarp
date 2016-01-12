/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_WIRE_H
#define YARP_OS_WIRE_H

#include <yarp/os/WireLink.h>

namespace yarp {
    namespace os {
        class Wire;
    }
}

/**
 *
 * Base class for IDL client/server.
 *
 */
class yarp::os::Wire : public PortReader {
private:
    WireLink _yarp_link;
public:
    /**
     *
     * Get YARP state associated with this object.
     *
     * @return state object.
     *
     */
    WireLink& yarp() { return _yarp_link; }
};

#endif // YARP_OS_WIRE_H
