// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_PORTREPORT_
#define _YARP2_PORTREPORT_

#include <yarp/os/ConstString.h>
#include <yarp/os/Vocab.h>

namespace yarp {
    namespace os {
        class PortInfo;
        class PortReport;
    }
}


/**
 * \ingroup comm_class
 *
 * Information about a port connection or event.
 * This class is under development - it is just a stub for now.
 *
 */
class yarp::os::PortInfo {
public:
    PortInfo() {
        tag = PORTINFO_NULL;
        incoming = false;
        message = "no information";
    }

    /// Possible tag values
    enum {
        /// No information.
        PORTINFO_NULL = 0,

        /// Information about an incoming or outgoing connection.
        PORTINFO_CONNECTION = VOCAB4('c','o','n','n'),

        /// Unspecified information.
        PORTINFO_MISC = VOCAB4('m','i','s','c'),
    };

    /// Type of information. PORTINFO_CONNECTION for information 
    /// about an incoming or outgoing connection.
    int tag;

    /// True if a connection is incoming, false if outgoing.
    bool incoming;

    /// Names of relevant ports, may be empty if irrelevant
    ConstString portName, sourceName, targetName;
    
    /// Name of protocol type, if releveant.
    ConstString carrierName;

    /// A human-readable description of contents.
    ConstString message;
};


/**
 * \ingroup comm_class
 *
 * A base class for objects that want information about port status
 * changes.
 *
 */
class yarp::os::PortReport {
public:
    /**
     * Destructor.
     */
    virtual ~PortReport() {}

    /**
     * Callback for port event/state information.
     * @param info Port event/state information
     */
    virtual void report(const PortInfo& info) = 0;
};


#endif
