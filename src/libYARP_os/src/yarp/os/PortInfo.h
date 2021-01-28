/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_PORTINFO_H
#define YARP_OS_PORTINFO_H

#include <yarp/os/api.h>

#include <yarp/os/Vocab.h>

#include <string>


namespace yarp {
namespace os {

/**
 * \ingroup comm_class
 *
 * Information about a port connection or event.
 */
class YARP_os_API PortInfo
{
public:
    /**
     * Constructor
     */
    explicit PortInfo();

    /// Possible tag values
    enum
    {
        /// No information.
        PORTINFO_NULL = 0,

        /// Information about an incoming or outgoing connection.
        PORTINFO_CONNECTION = yarp::os::createVocab('c', 'o', 'n', 'n'),

        /// Unspecified information.
        PORTINFO_MISC = yarp::os::createVocab('m', 'i', 's', 'c')
    };

    /// Type of information. PORTINFO_CONNECTION for information
    /// about an incoming or outgoing connection.
    int tag;

    /// True if a connection is incoming, false if outgoing.
    bool incoming;

    /// True if a connection is created, false if destroyed.
    bool created;

    /// Name of port.
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) portName;

    /// Name of connection source, if any.
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) sourceName;

    /// Name of connection target, if any.
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) targetName;

    /// Name of protocol type, if releveant.
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) carrierName;

    /// A human-readable description of contents.
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) message;
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_PORTINFO_H
