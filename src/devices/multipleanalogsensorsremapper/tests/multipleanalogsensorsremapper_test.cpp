/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Time.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/PolyDriverList.h>
#include <yarp/dev/IMultipleWrapper.h>

#include <yarp/dev/MultipleAnalogSensorsInterfaces.h>
#include <yarp/dev/tests/IOrientationSensorsTest.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::dev;


TEST_CASE("dev::multipleanalogsensorsremapperTest", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("multipleanalogsensorsremapper", "device");
    YARP_REQUIRE_PLUGIN("fakeIMU", "device");

    Network::setLocalMode(true);

    SECTION("Test the multipleanalogsensorsremapper device alone")
    {
        Property options;
        options.put("device", "multipleanalogsensorsremapper");
        Bottle gyrosNames;
        Bottle& gyrosList = gyrosNames.addList();
        gyrosList.addString("head");
        gyrosList.addString("torso");
        options.put("OrientationSensorsNames", gyrosNames.get(0));

        PolyDriver ddRemapper;
        REQUIRE(ddRemapper.open(options));

        yarp::os::Time::now();
        ddRemapper.close();
    }

    SECTION("Test the multipleanalogsensorsremapper device attached")
    {

        PolyDriver dd_imu1;
        PolyDriver dd_imu2;
        PolyDriver ddRemapper;
        yarp::dev::IOrientationSensors* iimu = nullptr;

        {
            Property p_cfg;
            p_cfg.put("device", "fakeIMU");
            p_cfg.put("sensorName", "fakeIMU1");
            REQUIRE(dd_imu1.open(p_cfg));
        }

        {
            Property p_cfg;
            p_cfg.put("device", "fakeIMU");
            p_cfg.put("sensorName", "fakeIMU2");
            REQUIRE(dd_imu2.open(p_cfg));
        }

        Property options;
        options.put("device", "multipleanalogsensorsremapper");
        Bottle gyrosNames;
        Bottle& gyrosList = gyrosNames.addList();
        gyrosList.addString("fakeIMU1");
        gyrosList.addString("fakeIMU2");
        options.put("OrientationSensorsNames", gyrosNames.get(0));

        REQUIRE(ddRemapper.open(options));

        yarp::dev::IMultipleWrapper* iwrap = nullptr;
        REQUIRE(ddRemapper.view(iwrap));
        PolyDriverList pdList;
        pdList.push(&dd_imu1, "fakeIMU1_key");
        pdList.push(&dd_imu2, "fakeIMU2_key");
        REQUIRE(iwrap->attachAll(pdList));

        REQUIRE(ddRemapper.view(iimu));
        yarp::dev::tests::exec_IOrientationSensors_test_1(iimu);

        yarp::os::Time::delay(0.1);

        ddRemapper.close();
    }

    Network::setLocalMode(false);
}
