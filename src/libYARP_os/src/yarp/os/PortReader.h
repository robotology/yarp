/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_PORTREADER_H
#define YARP_OS_PORTREADER_H

#include <yarp/os/api.h>

namespace yarp {
namespace os {

class ConnectionReader;
class Type;

/**
 * Interface implemented by all objects that can read themselves from
 * the network, such as Bottle objects.
 *
 * @see Port, PortWriter
 */
class YARP_os_API PortReader
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

    virtual Type getReadType() const;
};

} // namespace os
} // namespace yarp


#endif // YARP_OS_PORTREADER_H
