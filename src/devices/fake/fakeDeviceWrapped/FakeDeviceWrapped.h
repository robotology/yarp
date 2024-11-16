/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FAKEDEVICEWRAPPED_H
#define YARP_FAKEDEVICEWRAPPED_H

#include <yarp/dev/testInterfaces/IFakeDeviceInterfaceTest.h>
#include <yarp/dev/PolyDriver.h>

 /**
 * @ingroup dev_impl_fake
 *
 * \brief `FakeDeviceWrapped`: A simple device for testing purposes.
 * It is automatically wrapped by FakeDeviceWrapper via deviceBundler device.
 * See also FakeDeviceUnwrapped for an identical device without automatic wrapping.
 */
class FakeDeviceWrapped :
    public yarp::dev::DeviceDriver,
    public yarp::dev::test::IFakeDeviceInterfaceTest
{
protected:
    int m_value = { 0 };

public:
    FakeDeviceWrapped() = default;
    FakeDeviceWrapped(const FakeDeviceWrapped&) = delete;
    FakeDeviceWrapped(FakeDeviceWrapped&&) = delete;
    FakeDeviceWrapped& operator=(const FakeDeviceWrapped&) = delete;
    FakeDeviceWrapped& operator=(FakeDeviceWrapped&&) = delete;

    ~FakeDeviceWrapped() override = default;

    // yarp::dev::DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //interfaces
    void testSetValue(int value) override;
    void testGetValue(int& value) override;
};

#endif
