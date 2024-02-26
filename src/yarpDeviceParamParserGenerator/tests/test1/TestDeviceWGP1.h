/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/DeviceDriver.h>
#include "TestDeviceWGP1_ParamsParser.h"

#include <string>

/**
* @ingroup dev_impl_media
*
* \brief `TestDeviceWGP1` : A test device driver, for Continuous Integration purposes.
*
* Parameters required by this device are shown in class: TestDeviceWGP1_ParamsParser
*/

class TestDeviceWGP1 :
        public yarp::dev::DeviceDriver,
        public TestDeviceWGP1_ParamsParser
{
public:
    TestDeviceWGP1();
    TestDeviceWGP1(const TestDeviceWGP1&) = delete;
    TestDeviceWGP1(TestDeviceWGP1&&) = delete;
    TestDeviceWGP1& operator=(const TestDeviceWGP1&) = delete;
    TestDeviceWGP1& operator=(TestDeviceWGP1&&) = delete;
    ~TestDeviceWGP1() override;

    // Device Driver interface
    bool open(yarp::os::Searchable &config) override;
    bool close() override;

    private:
    bool do_test();
};
