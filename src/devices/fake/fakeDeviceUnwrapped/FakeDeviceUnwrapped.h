/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FAKEDEVICEUNWRAPPED_H
#define YARP_FAKEDEVICEUNWRAPPED_H

#include <yarp/dev/testInterfaces/IFakeDeviceInterfaceTest.h>
#include <yarp/dev/PolyDriver.h>

 /**
 * @ingroup dev_impl_fake
 *
 * \brief `FakeDeviceUnwrapper`: A simple device for testing purposes.
 * It is can be wrapped on request by FakeDeviceWrapper via DeviceBundler device.
 */
class FakeDeviceUnwrapped :
    public yarp::dev::DeviceDriver,
    public yarp::dev::test::IFakeDeviceInterfaceTest
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

    //interfaces
    void testSetValue(int value) override;
    void testGetValue(int& value) override;
};

#endif
