/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_PORTWRITER_H
#define YARP_OS_PORTWRITER_H

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

#endif // YARP_OS_PORTWRITER_H
