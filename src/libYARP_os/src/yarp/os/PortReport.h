/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */


#include <yarp/conf/system.h>

#if !defined(YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE)
YARP_COMPILER_WARNING("<yarp/os/PortReport.h> file is deprecated")
#endif

#ifndef YARP_OS_PORTREPORT_H
#define YARP_OS_PORTREPORT_H

#ifndef YARP_NO_DEPRECATED // Since YARP 3.4

#include <yarp/os/api.h>
namespace yarp {
namespace os {

class PortInfo;

/**
 * \ingroup comm_class
 *
 * A base class for objects that want information about port status
 * changes.
 *
 * @deprecated since YARP 3.4
 */
class YARP_os_DEPRECATED_API PortReport
{
    // HACK Issue a deprecated warning in classes derived from yarp::os::PortReport
    class YARP_DEPRECATED yarp__os__PortReport_is_deprecated {};
    yarp__os__PortReport_is_deprecated issue_warning;

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

} // namespace os
} // namespace yarp

#endif // YARP_NO_DEPRECATED

#endif // YARP_OS_PORTREPORT_H
