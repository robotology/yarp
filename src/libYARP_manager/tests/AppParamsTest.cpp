/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/manager/kbase.h>
#include <yarp/manager/utility.h>
#include <yarp/manager/xmlapploader.h>
#include <yarp/manager/xmlappsaver.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

#include <filesystem>
#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace yarp::manager;

namespace {

// Loads all the applications in the test directory and puts together the
// application called name
struct LoadedApp
{
    XmlAppLoader loader;
    KnowledgeBase kb;
    bool ok {false};

    explicit LoadedApp(const char* name, const std::string& dir = MANAGER_TEST_APPS_DIR) :
            loader(dir.c_str(), nullptr)
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

    std::multiset<std::string> moduleParams(const std::string& name)
    {
        std::multiset<std::string> params;
        for (Module* mod : kb.getSelModules()) {
            if (name == mod->getName()) {
                params.insert(mod->getParam());
            }
        }
        return params;
    }

    std::set<std::string> connectionSources()
    {
        std::set<std::string> sources;
        for (Connection cnn : kb.getSelConnection()) {
            sources.insert(cnn.from());
        }
        return sources;
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

TEST_CASE("manager::AppParamsImportTest", "[yarp::manager]")
{
    ErrorLogger::Instance()->clear();

    SECTION("An application loaded on its own uses its default values")
    {
        LoadedApp app("ParamChild");
        INFO(allErrors());
        REQUIRE(app.ok);
        CHECK(app.moduleParams("child_mod") == std::multiset<std::string> {"--robot icubSim --status SUCCESS"});
    }

    SECTION("Each import of the same application gets its own values")
    {
        LoadedApp app("ParamTwice");
        INFO(allErrors());
        REQUIRE(app.ok);
        CHECK(app.moduleParams("child_mod") == std::multiset<std::string> {
            "--robot icub --status SUCCESS",
            "--robot icubSim --status SUCCESS",
            "--robot ergocub --status RUNNING"});
        CHECK(app.connectionSources() == std::set<std::string> {"/a/icub/out", "/b/icubSim/out", "/c/ergocub/out"});
    }

    SECTION("Override values can use the parameters of the importing application")
    {
        LoadedApp app("ParamFromParent");
        INFO(allErrors());
        REQUIRE(app.ok);
        CHECK(app.moduleParams("child_mod") == std::multiset<std::string> {"--robot icubSim --status RUNNING"});
    }

    SECTION("Values are passed through nested imports")
    {
        LoadedApp app("ParamNested");
        INFO(allErrors());
        REQUIRE(app.ok);
        CHECK(app.moduleParams("child_mod") == std::multiset<std::string> {"--robot icubSim --status FAILURE"});
    }

    SECTION("A parameter without default can be given by the importing application")
    {
        LoadedApp app("ParamProvided");
        INFO(allErrors());
        REQUIRE(app.ok);
        CHECK(app.moduleParams("yarpdev") == std::multiset<std::string> {"--robot icub"});
    }

    SECTION("Overriding a parameter that the imported application never uses is an error")
    {
        LoadedApp app("ParamUnused");
        CHECK_FALSE(app.ok);
        std::string errors = allErrors();
        INFO(errors);
        CHECK(errors.find("parameter 'robt' is overridden but never used") != std::string::npos);
    }

    SECTION("Malformed app-param tags prevent the file from loading")
    {
        auto check = [](const char* file, const std::string& expected) {
            ErrorLogger::Instance()->clear();
            XmlAppLoader loader((std::string(MANAGER_TEST_INVALID_APPS_DIR) + "/" + file).c_str());
            REQUIRE(loader.init());
            CHECK(loader.getNextApplication() == nullptr);
            std::string errors = allErrors();
            INFO(errors);
            CHECK(errors.find(expected) != std::string::npos);
        };

        check("ParamNoName.xml", "app-param without name");
        check("ParamDuplicate.xml", "app-param robot is set twice");
    }

    ErrorLogger::Instance()->clear();
}

TEST_CASE("manager::AppParamsSaveTest", "[yarp::manager]")
{
    namespace fs = std::filesystem;
    ErrorLogger::Instance()->clear();

    // Saves the application, reloads it next to the application it imports and
    // checks that the imported modules get the same parameters
    auto saveAndReload = [](const char* appName, bool usesParams, const std::vector<std::string>& expectedXml) {
        LoadedApp app(appName);
        INFO(allErrors());
        REQUIRE(app.ok);
        CHECK(app.kb.getApplication()->usesAppParameters() == usesParams);

        fs::path dir = fs::temp_directory_path() / ("yarp_manager_test_" + std::string(appName));
        fs::remove_all(dir);
        fs::create_directories(dir);
        fs::copy_file(fs::path(MANAGER_TEST_APPS_DIR) / "ParamChild.xml", dir / "ParamChild.xml");
        fs::path saved = dir / (std::string(appName) + ".xml");
        XmlAppSaver saver(saved.string().c_str());
        REQUIRE(app.kb.saveApplication(&saver, app.kb.getApplication()));

        std::ifstream in(saved);
        std::stringstream xml;
        xml << in.rdbuf();
        INFO(xml.str());
        for (const auto& expected : expectedXml) {
            CHECK(xml.str().find(expected) != std::string::npos);
        }

        LoadedApp reloaded(appName, dir.string());
        INFO(allErrors());
        REQUIRE(reloaded.ok);
        CHECK(reloaded.moduleParams("child_mod") == app.moduleParams("child_mod"));
        fs::remove_all(dir);
    };

    SECTION("Values given with app-param are saved")
    {
        saveAndReload("ParamTwice", false, {
            "<app-param name=\"robot\">icub</app-param>",
            "<app-param name=\"robot\">ergocub</app-param>",
            "<app-param name=\"status\">RUNNING</app-param>"});
    }

    SECTION("Values given with app-param are saved as written")
    {
        saveAndReload("ParamFromParent", true, {"<app-param name=\"status\">$PAR{mode=RUNNING}</app-param>"});
    }

    ErrorLogger::Instance()->clear();
}
