/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/WrapperSingle.h>
#include <yarp/dev/IDeviceDriverParams.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;
using namespace yarp::os;

TEST_CASE("dev::TestDeviceWGP", "[yarp::dev]")
{
    YARP_REQUIRE_PLUGIN("testDeviceWGP2", "device");

    Network::setLocalMode(true);

    SECTION("Checking TestDeviceWGP2 help")
    {
        PolyDriver dd;
        Property p_cfg;
        p_cfg.put("device", "testDeviceWGP2");
        p_cfg.put("help","");
        REQUIRE(dd.open(p_cfg)==false);
    }

    SECTION("Checking TestDeviceWGP2 from string with parenthesis")
    {
        PolyDriver dd;
        std::string open_string = "\
            device testDeviceWGP2\n\
            param_vec9 (\"sa1\" \"sa2\" \"sa3\")\n\
            param_vec8 (1.0 2.0 3.0)\n\
            param_vec7 (1 2 3)\n\
            param_vec6 (sa1 sa2 sa3)\n\
            param_vec5 (1.0 2.0 3.0)\n\
            param_vec4 (1 2 3)\n\
            ";
        yarp::os::Property p;
        p.fromConfig(open_string.c_str());
        REQUIRE(dd.open(p));
    }

    SECTION("Checking TestDeviceWGP2 from string without parenthesis (deprecated, it will fail!)")
    {
        PolyDriver dd;
        std::string open_string = "\
            device testDeviceWGP2\n\
            param_vec9 \"sa1\" \"sa2\" \"sa3\"\n\
            param_vec8 1.0 2.0 3.0\n\
            param_vec7 1 2 3\n\
            param_vec6 sa1 sa2 sa3\n\
            param_vec5 1.0 2.0 3.0\n\
            param_vec4 1 2 3\n\
            ";
        yarp::os::Property p;
        p.fromConfig(open_string.c_str());
        REQUIRE(dd.open(p)==false);
    }

    SECTION("Checking TestDeviceWGP2 device")
    {
        INFO("from c code");
        PolyDriver dd;

        ////////"Checking opening polydriver with no attached device"
        {
            Property p_cfg;
            p_cfg.put("device",    "testDeviceWGP2");

            yarp::os::Value val7; val7.fromString("(1 2 3)");
            p_cfg.put("param_vec7", val7);
            yarp::os::Value val8; val8.fromString("(1.0 2.0 3.0)");
            p_cfg.put("param_vec8", val8);
            yarp::os::Value val9; val9.fromString("(sa1 sa2 sa3)");
            p_cfg.put("param_vec9", val9);

            REQUIRE(dd.open(p_cfg));

            yarp::dev::IDeviceDriverParams* idevparams = nullptr;
            REQUIRE (dd.view(idevparams));
            REQUIRE (idevparams!=nullptr);

            std::string ss1 = idevparams->getDeviceClassName();
            CHECK (ss1 == "TestDeviceWGP2");
            std::string ss2 = idevparams->getDeviceName();
            CHECK (ss2 == "testDeviceWGP2");

            std::vector<std::string> vv = idevparams->getListOfParams();
            CHECK(vv.empty() == false);

            std::string param_str;
            bool getpv = idevparams->getParamValue("param_vec9", param_str);
            CHECK(getpv == false);
            CHECK(param_str.empty());
        }

        yarp::os::Time::delay(1.0);

        //"Close all polydrivers and check"
        {
            CHECK(dd.close());
        }
    }

    Network::setLocalMode(false);
}
