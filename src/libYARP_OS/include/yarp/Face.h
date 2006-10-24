// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_FACE_
#define _YARP2_FACE_

#include <yarp/Address.h>
#include <yarp/InputProtocol.h>
#include <yarp/OutputProtocol.h>

namespace yarp {
    class Face;
}

/**
 * A way to communicate with a port.
 */
class yarp::Face {
public:
    virtual ~Face() {} // destructors must be virtual

    // all throw IOException

    virtual void open(const Address& address) = 0;
    virtual void close() = 0;
    virtual InputProtocol *read() = 0;
    virtual OutputProtocol *write(const Address& address) = 0; 
};

#endif

