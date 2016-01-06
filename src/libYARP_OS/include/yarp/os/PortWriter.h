// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_PORTWRITER
#define YARP2_PORTWRITER

#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/Type.h>

namespace yarp {
    namespace os {
        class PortWriter;
    }
}

/**
 * Interface implemented by all objects that can write themselves to
 * the network, such as Bottle objects.
 * @see Port, PortReader
 */
class YARP_OS_API yarp::os::PortWriter {
public:
    /**
     * Destructor.
     */
    virtual ~PortWriter();

    /**
     * Write this object to a network connection.
     * Override this for your particular class.
     * Be aware that
     * depending on the nature of the connections a port has, and what
     * protocol they use, and how efficient the YARP implementation is,
     * this method may be called once, twice, or many times, as the
     * result of a single call to Port::write
     * @param writer an interface to the network connection for writing
     * @return true iff the object is successfully written
     */
    virtual bool write(ConnectionWriter& writer) = 0;

    /**
     * This is called when the port has finished all writing operations.
     */
    virtual void onCompletion();

    /**
     * This is called when the port is about to begin writing operations.
     * After this point, the write method may be called zero, once, or
     * many times by YARP depending on the mix of formats and protocols
     * in use.
     */
    virtual void onCommencement();

    virtual Type getWriteType() { return Type::anon(); }
};

#endif
