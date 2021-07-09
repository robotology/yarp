/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_WIRE_H
#define YARP_OS_WIRE_H

#include <yarp/os/PortReader.h>
#include <yarp/os/WireLink.h>

namespace yarp {
namespace os {

/**
 * Base class for IDL client/server.
 */
class Wire : public PortReader
{
private:
    yarp::os::WireLink _yarp_link;

public:
    /**
     * Get YARP state associated with this object.
     *
     * @return state object.
     */
    yarp::os::WireLink& yarp()
    {
        return _yarp_link;
    }

    /**
     * Get YARP state associated with this object (const version).
     *
     * @return state object.
     */
    const yarp::os::WireLink& yarp() const
    {
        return _yarp_link;
    }
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_WIRE_H
