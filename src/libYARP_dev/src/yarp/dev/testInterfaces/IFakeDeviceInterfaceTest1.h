/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IFAKEDEVICENTERFACETEST1_H
#define YARP_DEV_IFAKEDEVICENTERFACETEST1_H

#include <yarp/dev/api.h>
#include <yarp/dev/ReturnValue.h>

namespace yarp::dev::test {

/**
 * @ingroup dev_iface_other
 *
 * A fake interface for testing,debugging and development purposes
 */
class YARP_dev_API IFakeDeviceInterfaceTest1
{
public:
    /**
     * Destructor.
     */
    virtual ~IFakeDeviceInterfaceTest1();

    /**
     * A method for testing purposes
     */
    virtual yarp::dev::ReturnValue doSomething() = 0;

    /**
     * A method for testing purposes, which implementation is not implemented
     */
    virtual yarp::dev::ReturnValue doSomethingUnimplemented() = 0;
};

} // namespace yarp::dev::test

#endif // YARP_DEV_IFAKEDEVICENTERFACETEST1_H
