/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
        PORTINFO_CONNECTION = yarp::os::createVocab32('c', 'o', 'n', 'n'),

        /// Unspecified information.
        PORTINFO_MISC = yarp::os::createVocab32('m', 'i', 's', 'c')
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
