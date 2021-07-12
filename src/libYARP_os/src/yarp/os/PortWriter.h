/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_PORTWRITER_H
#define YARP_OS_PORTWRITER_H

#include <yarp/os/api.h>

namespace yarp {
namespace os {

class ConnectionWriter;
class Type;

/**
 * Interface implemented by all objects that can write themselves to
 * the network, such as Bottle objects.
 * @see Port, PortReader
 */
class YARP_os_API PortWriter
{
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
    virtual bool write(ConnectionWriter& writer) const = 0;

    /**
     * This is called when the port has finished all writing operations.
     */
    virtual void onCompletion() const;

    /**
     * This is called when the port is about to begin writing operations.
     * After this point, the write method may be called zero, once, or
     * many times by YARP depending on the mix of formats and protocols
     * in use.
     */
    virtual void onCommencement() const;

    virtual yarp::os::Type getWriteType() const;
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_PORTWRITER_H
