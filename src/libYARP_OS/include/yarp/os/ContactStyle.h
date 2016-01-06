// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_CONTACTSTYLE
#define YARP2_CONTACTSTYLE

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

#endif
