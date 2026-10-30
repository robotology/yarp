/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/manager/kbase.h>
#include <yarp/manager/utility.h>
#include <yarp/manager/xmlapploader.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

#include <set>
#include <string>

using namespace yarp::manager;

namespace {

// Loads all the applications in the test directory and puts together the
// application called name
struct LoadedApp
{
    XmlAppLoader loader {MANAGER_TEST_APPS_DIR, nullptr};
    KnowledgeBase kb;
    bool ok {false};

    explicit LoadedApp(const char* name)
    {
        ok = kb.createFrom(nullptr, &loader, nullptr) && kb.reasolveDependency(name);
    }

    Module* module(const std::string& name)
    {
        for (Module* mod : kb.getSelModules()) {
            if (name == mod->getName()) {
                return mod;
            }
        }
        return nullptr;
    }

    std::set<std::string> resourcePorts()
    {
        std::set<std::string> ports;
        Application* app = kb.getApplication();
        for (int i = 0; i < app->resourcesCount(); i++) {
            ports.insert(app->getResourceAt(i).getPort());
        }
        return ports;
    }
};

std::string allErrors()
{
    std::string msgs;
    ErrorLogger* logger = ErrorLogger::Instance();
    while (const char* err = logger->getLastError()) {
        msgs += std::string(err) + "\n";
    }
    return msgs;
}

} // namespace

TEST_CASE("manager::AppParamsTest", "[yarp::manager]")
{
    ErrorLogger::Instance()->clear();

    SECTION("Default values are used in every kind of field of a single application")
    {
        LoadedApp app("ParamSingle");
        INFO(allErrors());
        REQUIRE(app.ok);

        Module* dev = app.module("yarpdev");
        REQUIRE(dev);
        CHECK(std::string(dev->getParam()) == "--robot icubSim --from robot.ini");
        CHECK(std::string(dev->getHost()) == "localhost");
        CHECK(std::string(dev->getWorkDir()) == "/tmp");
        CHECK(std::string(dev->getEnvironment()) == "ROBOT=icubSim");
        CHECK(std::string(dev->getBasePrefix()) == "/icubSim");

        Module* plain = app.module("plain");
        REQUIRE(plain);
        CHECK(std::string(plain->getParam()) == "--from robot.ini --no-params");

        ModuleInterface& imod = app.kb.getApplication()->getImoduleAt(0);
        REQUIRE(imod.portmapCount() == 1);
        CHECK(std::string(imod.getPortmapAt(0).oldPort()) == "/old");
        CHECK(std::string(imod.getPortmapAt(0).newPort()) == "/icubSim/new");

        const CnnContainer& cnns = app.kb.getSelConnection();
        REQUIRE(cnns.size() == 1);
        Connection cnn = cnns[0];
        CHECK(std::string(cnn.from()) == "/icubSim/out");
        CHECK(std::string(cnn.to()) == "/icubSim/in");
        CHECK(std::string(cnn.carrier()) == "tcp");

        std::set<std::string> ports = app.resourcePorts();
        CHECK(ports.count("/icubSim/app_dep") == 1);
        CHECK(ports.count("/icubSim/mod_dep") == 1);
        CHECK(ports.count("/icubSim/in") == 1);

        REQUIRE(app.kb.getApplication()->arbitratorCount() == 1);
        Arbitrator& arb = app.kb.getApplication()->getArbitratorAt(0);
        CHECK(std::string(arb.getPort()) == "/icubSim/arb");
        CHECK(std::string(arb.getRule("c1")) == "~c2");
    }

    SECTION("The prefix of an imported application can use parameters")
    {
        LoadedApp app("ParamPrefix");
        INFO(allErrors());
        REQUIRE(app.ok);

        const CnnContainer& cnns = app.kb.getSelConnection();
        REQUIRE(cnns.size() == 1);
        Connection cnn = cnns[0];
        CHECK(std::string(cnn.from()) == "/robot/out");
        CHECK(std::string(cnn.to()) == "/robot/in");
    }

    SECTION("Errors stop the application from loading")
    {
        auto check = [](const char* appName, const std::string& expected) {
            ErrorLogger::Instance()->clear();
            LoadedApp app(appName);
            CHECK_FALSE(app.ok);
            std::string errors = allErrors();
            INFO(errors);
            CHECK(errors.find(expected) != std::string::npos);
        };

        check("ParamMissing", "parameter 'robot' has no value");
        check("ParamConflict", "parameter 'robot' has two different default values");
        check("ParamMalformed", "unterminated '$PAR{'");
        check("ParamChildError", "parameter 'robot' has no value");
    }

    ErrorLogger::Instance()->clear();
}
