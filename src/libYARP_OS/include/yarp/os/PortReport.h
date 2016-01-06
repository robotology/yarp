// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_PORTREPORT
#define YARP2_PORTREPORT

#include <yarp/os/api.h>

namespace yarp {
    namespace os {
        class PortInfo;
        class PortReport;
    }
}


/**
 * \ingroup comm_class
 *
 * A base class for objects that want information about port status
 * changes.
 */
class YARP_OS_API yarp::os::PortReport {
public:
    /**
     * Destructor.
     */
    virtual ~PortReport();

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
