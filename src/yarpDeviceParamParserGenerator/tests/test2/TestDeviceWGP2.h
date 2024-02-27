/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/DeviceDriver.h>
#include "TestDeviceWGP2_ParamsParser.h"

#include <string>

/**
* @ingroup dev_impl_media
*
* \brief `TestDeviceWGP2` : A test device driver, for Continuous Integration purposes.
*
* Parameters required by this device are shown in class: TestDeviceWGP_ParamsParser
*/

class TestDeviceWGP2 :
        public yarp::dev::DeviceDriver,
        public TestDeviceWGP2_ParamsParser
{
public:
    TestDeviceWGP2();
    TestDeviceWGP2(const TestDeviceWGP2&) = delete;
    TestDeviceWGP2(TestDeviceWGP2&&) = delete;
    TestDeviceWGP2& operator=(const TestDeviceWGP2&) = delete;
    TestDeviceWGP2& operator=(TestDeviceWGP2&&) = delete;
    ~TestDeviceWGP2() override;

    // Device Driver interface
    bool open(yarp::os::Searchable &config) override;
    bool close() override;

    private:
    bool do_test();
};
