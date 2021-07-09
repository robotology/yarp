/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_PORTREADERCREATOR_H
#define YARP_OS_PORTREADERCREATOR_H

#include <yarp/os/ConnectionReader.h>
#include <yarp/os/PortReader.h>

#include <string>

namespace yarp {
namespace os {

/**
 * @brief A creator for readers.
 *
 * This is used when you want a Port to create
 * a different reader for every input connection it receives.  This is
 * a very quick way to make a multi-threaded server that keeps track
 * of which input is which.  Inherit from this class, defining the
 * PortReaderCreator::create method.  Then pass an instance to
 * Port::setReaderCreator.  The create() method will be called every
 * time the Port receives a new connection, and all input coming in
 * via that connection will be channeled appropriately.
 */
class YARP_os_API PortReaderCreator
{
public:
    /**
     * Destructor.
     */
    virtual ~PortReaderCreator();

    /**
     * Factory for PortReader objects
     * @return A new PortReader object -- used to read from a connection to
     * a Port
     */
    virtual PortReader* create() const = 0;
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_PORTREADERCREATOR_H
