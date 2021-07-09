/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_PORTREPORT_H
#define YARP_OS_PORTREPORT_H

#include <yarp/os/api.h>

namespace yarp {
namespace os {
class PortInfo;
} // namespace os
} // namespace yarp

namespace yarp {
namespace os {

/**
 * \ingroup comm_class
 *
 * A base class for objects that want information about port status
 * changes.
 */
class YARP_os_API PortReport
{
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

#endif // YARP_OS_PORTREPORT_H
