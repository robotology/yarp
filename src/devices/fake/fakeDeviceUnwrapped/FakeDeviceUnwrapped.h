/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FAKEDEVICEUNWRAPPED_H
#define YARP_FAKEDEVICEUNWRAPPED_H

#include <yarp/dev/testInterfaces/IFakeDeviceInterfaceTest1.h>
#include <yarp/dev/testInterfaces/IFakeDeviceInterfaceTest2.h>
#include <yarp/dev/PolyDriver.h>

 /**
 * @ingroup dev_impl_fake
 *
 * \brief `FakeDeviceUnwrapper`: A simple device for testing purposes.
 * It is can be wrapped on request by FakeDeviceWrapper via DeviceBundler device.
 */
class FakeDeviceUnwrapped :
    public yarp::dev::DeviceDriver,
    public yarp::dev::test::IFakeDeviceInterfaceTest1,
    public yarp::dev::test::IFakeDeviceInterfaceTest2
{
protected:
    int m_value = {0};

public:
    FakeDeviceUnwrapped() = default;
    FakeDeviceUnwrapped(const FakeDeviceUnwrapped&) = delete;
    FakeDeviceUnwrapped(FakeDeviceUnwrapped&&) = delete;
    FakeDeviceUnwrapped& operator=(const FakeDeviceUnwrapped&) = delete;
    FakeDeviceUnwrapped& operator=(FakeDeviceUnwrapped&&) = delete;

    ~FakeDeviceUnwrapped() override = default;

    // yarp::dev::DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //IFakeDeviceInterfaceTest1
    yarp::dev::ReturnValue doSomething() override;

    //IFakeDeviceInterfaceTest2
    yarp::dev::ReturnValue testSetValue(int value) override;
    yarp::dev::ReturnValue testGetValue(int& value) override;
};

#endif
