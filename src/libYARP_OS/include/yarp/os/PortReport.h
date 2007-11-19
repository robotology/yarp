// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_PORTREPORT_
#define _YARP2_PORTREPORT_

#include <yarp/os/ConstString.h>

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
