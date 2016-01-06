// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_WIRE
#define YARP2_WIRE

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

#endif
