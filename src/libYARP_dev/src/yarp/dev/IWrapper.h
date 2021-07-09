/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IWRAPPER_H
#define YARP_DEV_IWRAPPER_H

#include <yarp/dev/api.h>
#include <yarp/dev/PolyDriver.h>

namespace yarp {
namespace dev {

/**
 * @ingroup dev_iwrapper
 *
 * Interface for an object that can wrap/or "attach" to another. This is useful
 * for example when an object refers internally to another device, but
 * you want to decouple the creation of the two objects. In this case
 * you first creates the two objects separately then you can attach one
 * to the other.
 */
class YARP_dev_API IWrapper
{
public:
    /**
     * Destructor.
     */
    virtual ~IWrapper();

    /**
     * Attach to another object.
     * @param driver the polydriver that you want to attach to.
     * @return true/false on success failure.
     */
    virtual bool attach(PolyDriver* driver) = 0;

    /**
     * Detach the object (you must have first called attach).
     * @return true/false on success failure.
     */
    virtual bool detach() = 0;
};

} // namespace dev
} // namespace yarp

#endif // YARP_DEV_IWRAPPER_H
