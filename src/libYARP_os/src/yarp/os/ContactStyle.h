/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_CONTACTSTYLE_H
#define YARP_OS_CONTACTSTYLE_H

#include <yarp/os/api.h>

#include <string>

namespace yarp {
namespace os {

/**
 * \ingroup comm_class
 *
 * Preferences for how to communicate with a contact.
 * All fields have sensible defaults.
 */
class YARP_os_API ContactStyle
{
public:
    /**
     * Ask recipient to treat message as administrative.
     * All YARP ports support a basic API that this gives
     * access to.
     */
    bool admin;

    /**
     * Suppress all outputs and warnings.
     */
    bool quiet;

    /**
     * Allow output on success.
     */
    bool verboseOnSuccess;

    /**
     * Set a timeout for communication (in units of seconds,
     * fractional seconds allowed).
     */
    double timeout;

    /**
     * Request that communication be made using a particular
     * carrier.
     */
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) carrier;

    /**
     * Specify whether you expect a reply to a message.
     */
    bool expectReply;

    /**
     * Specify whether a requested connection should be persistent.
     */
    bool persistent;

    /**
     * Persistence types, controlling the lifetime of a persistent
     * connection.
     */
    enum PersistenceType
    {
        OPENENDED = 0,
        END_WITH_FROM_PORT = 1,
        END_WITH_TO_PORT = 2,
        NO_PERSISTENCE = 3
    };

    /**
     * Specify kind of persistence to use.  Ignored if the persistent
     * flag is inactive.
     */
    PersistenceType persistenceType;

    /**
     * Constructor.  Sets all options to reasonable defaults.
     */
    explicit ContactStyle();
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_CONTACTSTYLE_H
