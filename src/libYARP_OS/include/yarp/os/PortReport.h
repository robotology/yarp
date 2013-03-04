// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
 *
 */
class YARP_OS_API yarp::os::PortInfo {
public:
    PortInfo() {
        tag = PORTINFO_NULL;
        incoming = false;
        created = true;
        message = "no information";
    }

    /// Possible tag values
    enum {
        /// No information.
        PORTINFO_NULL = 0,

        /// Information about an incoming or outgoing connection.
        /// SWIG currently has bug if 'c' used in this macro!
        PORTINFO_CONNECTION = VOCAB4(99/*c*/, 'o', 'n', 'n'),

        /// Unspecified information.
        PORTINFO_MISC = VOCAB4('m','i','s','c')
    };

    /// Type of information. PORTINFO_CONNECTION for information 
    /// about an incoming or outgoing connection.
    int tag;

    /// True if a connection is incoming, false if outgoing.
    bool incoming;

    /// True if a connection is created, false if destroyed.
    bool created;

    /// Name of port.
    ConstString portName;
    
    /// Name of connection source, if any.
    ConstString sourceName;

    /// Name of connection target, if any.
    ConstString targetName;
    
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
 */
class yarp::os::PortReport {
public:
    /**
     * Destructor.
     */
    virtual ~PortReport() {}

    /**
     * Callback for port event/state information.  This callback
     * must be careful about performing any operation on the port
     * it is receiving information about, since that port is
     * locked until this callback completes.  To avoid
     * deadlocks, take action based on port reports in a separate
     * thread.
     *
     * @param info Port event/state information
     */
    virtual void report(const PortInfo& info) = 0;
};


#endif
