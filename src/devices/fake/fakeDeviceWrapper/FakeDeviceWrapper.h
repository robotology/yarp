/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FAKEDEVICEWRAPPER_H
#define YARP_FAKEDEVICEWRAPPER_H

#include <yarp/dev/testInterfaces/IFakeDeviceInterfaceTest.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>

 /**
 * @ingroup dev_impl_fake
 *
 * \brief `FakeDeviceWrapper`: A simple device for testing purposes.
 * It is can wrap both FakeDeviceUnwrapped and FakeDeviceWrapped devices.
 */
class FakeDeviceWrapper :
        public yarp::dev::DeviceDriver,
        public yarp::dev::WrapperSingle,
        public yarp::dev::test::IFakeDeviceInterfaceTest
{
private:
    yarp::dev::test::IFakeDeviceInterfaceTest* iTest=nullptr;

public:
    FakeDeviceWrapper() = default;
    FakeDeviceWrapper(const FakeDeviceWrapper&) = delete;
    FakeDeviceWrapper(FakeDeviceWrapper&&) = delete;
    FakeDeviceWrapper& operator=(const FakeDeviceWrapper&) = delete;
    FakeDeviceWrapper& operator=(FakeDeviceWrapper&&) = delete;

    ~FakeDeviceWrapper() override = default;

    // yarp::dev::DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    // yarp::dev::WrapperSingle
    bool detach() override;
    bool attach(yarp::dev::PolyDriver* drv) override;

    //interfaces
    void testSetValue(int value) override;
    void testGetValue(int& value) override;
};

#endif
