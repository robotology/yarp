/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/os/Carriers.h>
#include <yarp/os/YarpPluginSettings.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os;
TEST_CASE("os::ListPluginsTest", "[yarp::os]")
{
    NetworkBase::setLocalMode(true);

    SECTION("Checking generic plugin methods")
    {
#ifdef YARP_IS_STATIC
        //The following code is copied from command: yarp plugin --all
        YarpPluginSelector selector;
        selector.scan();
        Bottle lst = selector.getSelectedPlugins();
        CHECK(lst.size() == 0);
#else
        //The following code is copied from command: yarp plugin --all
        YarpPluginSelector selector;
        selector.scan();
        Bottle lst = selector.getSelectedPlugins();
        CHECK (lst.size()!=0);
        for (size_t i = 0; i < lst.size(); i++)
        {
            Value& options = lst.get(i);
            std::string name = options.check("name", Value("untitled")).asString();
            std::string type = options.check("type", Value("unknown type")).asString();
            yarp::os::YarpPluginSettings settings;
            bool b;
            b = settings.setSelector(selector);
            //CHECK(b);
            b = settings.readFromSearchable(options, name);
            //CHECK(b);
            SharedLibraryFactory lib;
            b = settings.open(lib);
            CHECK(b);
        }
#endif
    }

    NetworkBase::setLocalMode(false);
}
