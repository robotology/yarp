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
TEST_CASE("os::ListPluginsOnlyTest", "[yarp::os]")
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
        Bottle search_path = selector.getSearchPath();
        CHECK (lst.size()!=0);
        for (size_t i = 0; i < lst.size(); i++)
        {
            Value& options = lst.get(i);
            std::string name = options.check("name", Value("untitled")).asString();
            std::string type = options.check("type", Value("unknown type")).asString();
            yarp::os::YarpPluginSettings settings;
            bool b;
            b = settings.setSelector(selector);
            yarp::os::YarpPluginSelector* selector2 = settings.getSelector();
            //CHECK(b);
            b = settings.readFromSearchable(options, name);
            //CHECK(b);

            //Here I am not intentionally opening the plugin in order
            //to have a 'pure' valgrind test where leaky dynamic libraries are not involved.
            //Check also the test ListPluginsAndOpenTest.
            std::string baseClassName = settings.getBaseClassName();
            std::string ClassName = settings.getClassName();
            std::string LibraryName = settings.getLibraryName();
            std::string MethodName = settings.getMethodName();
            std::string PluginName = settings.getPluginName();
            std::string WrapperName = settings.getWrapperName();
        }
#endif
    }

    NetworkBase::setLocalMode(false);
}
