/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/robotinterface/Param.h>

#include <catch.hpp>
#include <harness.h>

TEST_CASE("robotinterface::ParamTest", "[yarp::robotinterface]")
{

    SECTION("Check yarp::robotinterface::Param")
    {
        std::string name = "name";
        std::string value = "value";
        bool isGroup = false;
        yarp::robotinterface::Param param(name, value, isGroup);

        CHECK(param.name() == name);
        CHECK(param.value() == value);
        CHECK(param.isGroup() ==  isGroup);
    }
};
