/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IFAKEDEVICENTERFACETEST2_H
#define YARP_DEV_IFAKEDEVICENTERFACETEST2_H

#include <yarp/dev/api.h>
#include <yarp/dev/ReturnValue.h>

namespace yarp::dev::test {

/**
 * \ingroup dev_iface_other
 *
 * \brief A test interface used internally by continuous integration.
 * See devices: fakeDeviceWrapped, fakeDeviceUnwrapped, fakeDeviceWrapper.
 */
class YARP_dev_API IFakeDeviceInterfaceTest2
{
public:
    virtual ~IFakeDeviceInterfaceTest2();

    /**
     * This method is used to set a single value inside the test device
     */
    virtual yarp::dev::ReturnValue testSetValue(int value) = 0;

    /**
     * This method is used to get a single value from a test device
     */
    virtual yarp::dev::ReturnValue testGetValue(int& value) = 0;
};

} // namespace yarp::dev::test

#endif // YARP_DEV_IFAKEDEVICENTERFACETEST_H
