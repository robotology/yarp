/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_CONTACTSTYLE_H
#define YARP_OS_CONTACTSTYLE_H

#include <yarp/os/ConstString.h>

namespace yarp {
    namespace os {
        class ContactStyle;
    }
}

/**
 * \ingroup comm_class
 *
 * Preferences for how to communicate with a contact.
 * All fields have sensible defaults.
 *
 */
class YARP_OS_API yarp::os::ContactStyle {
public:
    /**
     *
     * Ask recipient to treat message as administrative.
     * All YARP ports support a basic API that this gives
     * access to.
     *
     */
    bool admin;

    /**
     *
     * Suppress all outputs and warnings.
     *
     */
    bool quiet;

    /**
     *
     * Allow output on success.
     *
     */
    bool verboseOnSuccess;

    /**
     *
     * Set a timeout for communication (in units of seconds,
     * fractional seconds allowed).
     *
     */
    double timeout;

    /**
     *
     * Request that communication be made using a particular
     * carrier.
     *
     */
    ConstString carrier;

    /**
     *
     * Specify whether you expect a reply to a message.
     *
     */
    bool expectReply;

    /**
     *
     * Specify whether a requested connection should be persistent.
     *
     */
    bool persistent;

    /**
     *
     * Persistence types, controlling the lifetime of a persistent
     * connection.
     *
     */
    enum PersistenceType {
        OPENENDED = 0,
        END_WITH_FROM_PORT = 1,
        END_WITH_TO_PORT = 2,
        NO_PERSISTENCE = 3
    };

    /**
     *
     * Specify kind of persistence to use.  Ignored if the persistent
     * flag is inactive.
     *
     */
    PersistenceType persistenceType;

    /**
     *
     * Constructor.  Sets all options to reasonable defaults.
     *
     */
    explicit ContactStyle();
};

#endif // YARP_OS_CONTACTSTYLE_H
