/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_PORTREADER_H
#define YARP_OS_PORTREADER_H

#include <yarp/os/ConnectionReader.h>
#include <yarp/os/Type.h>

// Defined in this file:
namespace yarp { namespace os { class PortReader; }}


namespace yarp {
namespace os {

/**
 * Interface implemented by all objects that can read themselves from
 * the network, such as Bottle objects.
 *
 * @see Port, PortWriter
 */
class YARP_OS_API PortReader
{
public:

    /**
     * Destructor.
     */
    virtual ~PortReader();

    /**
     * Read this object from a network connection.
     *
     * Override this for your particular class.
     *
     * @param reader an interface to the network connection for reading
     * @return true iff the object is successfully read
     */
    virtual bool read(ConnectionReader& reader) = 0;

    virtual Type getReadType();
};

} // namespace os
} // namespace yarp


#endif // YARP_OS_PORTREADER_H
