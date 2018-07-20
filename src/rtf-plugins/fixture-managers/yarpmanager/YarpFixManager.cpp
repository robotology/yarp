/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <rtf/dll/Plugin.h>
#include <yarp/manager/utility.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Log.h>

#include "YarpFixManager.h"

using namespace RTF;
using namespace yarp::os;
using namespace yarp::manager;

PREPARE_FIXTURE_PLUGIN(YarpFixManager)

YarpFixManager::YarpFixManager()
    : initialized(false) { }

YarpFixManager::~YarpFixManager()
{
    delete manager;
}


bool YarpFixManager::setup(int argc, char** argv) {
    RTF_FIXTURE_REPORT("yarpmanager is setting up the fixture...");
    bool ret;
    if(!initialized) {
        // check yarp network
        yarp.setVerbosity(-1);
        RTF_ASSERT_ERROR_IF_FALSE(yarp.checkNetwork(),
                            "YARP network does not seem to be available, is the yarp server accessible?");

        // load the config file and update the environment if available
        // E.g., "--application myapp.xml"
        yarp::os::ResourceFinder rf;
        rf.setVerbose(false);
        rf.setDefaultContext("RobotTesting");
        rf.configure(argc, argv, false);

        RTF_ASSERT_ERROR_IF_FALSE(rf.check("fixture"),
                            "No application xml file is set (add --fixture yourfixture.xml)");

        manager = new YarpManagerPlugin(getDispatcher());
        manager->fixtureName = rf.find("fixture").asString();


        std::string appfile = rf.findFileByName(std::string("fixtures/"+manager->fixtureName).c_str());
        if (appfile.empty())
        {
            yInfo("yarpmanager: trying to load fixture file from absolute path");
            appfile = rf.findFileByName(std::string(manager->fixtureName).c_str());
        }

        RTF_ASSERT_ERROR_IF_FALSE(!appfile.empty(),
                            RTF::Asserter::format("yarpmanager cannot find application file %s. Is it in the 'fixtures' folder?",
                                                  manager->fixtureName.c_str()));
        // load the fixture (application xml)
        char* szAppName = nullptr;
        ret = manager->addApplication(appfile.c_str(), &szAppName, true);
        RTF_ASSERT_ERROR_IF_FALSE(ret,
                            "yarpmanager (addApplication) cannot setup the fixture because " +
                            std::string(manager->getLogger()->getFormatedErrorString()));

        ret = manager->loadApplication(szAppName);
        RTF_ASSERT_ERROR_IF_FALSE(ret,
                            "yarpmanager (loadApplication) cannot setup the fixture because " +
                            std::string(manager->getLogger()->getFormatedErrorString()));
        initialized = true;
        if (szAppName)
        {
            delete [] szAppName;
            szAppName = nullptr;
        }
    }

    //run the modules and connect
    ret = manager->run();
    RTF_ASSERT_ERROR_IF_FALSE(ret,
                        "yarpmanager (run) cannot setup the fixture because " +
                        std::string(manager->getLogger()->getFormatedErrorString()));
    return true;
}

void YarpFixManager::tearDown() {
    RTF_FIXTURE_REPORT("yarpmanager is tearing down the fixture...");
    bool ret = manager->stop();
    if(!ret)
        ret = manager->kill();
    const char* szerror = manager->getLogger()->getLastError();
    RTF_ASSERT_ERROR_IF_FALSE(ret,
                        "yarpmanager cannot teardown the fixture because " +
                        std::string((szerror) ? szerror : ""));
}

void YarpManagerPlugin::onExecutableFailed(void* which) {
    Executable* exe = (Executable*) which;
    RTF_ASSERT_ERROR_IF_FALSE(exe!=nullptr, "Executable is null!");
    TestMessage msg(Asserter::format("Fixture %s collapsed", fixtureName.c_str()),
                    Asserter::format("Module %s is failed!", exe->getCommand()),
                    RTF_SOURCEFILE(), RTF_SOURCELINE());
    dispatcher->fixtureCollapsed(msg);
}

void YarpManagerPlugin::onCnnFailed(void* which) {
    Connection* cnn = (Connection*) which;
    RTF_ASSERT_ERROR_IF_FALSE(cnn!=nullptr, "Connection is null!");
    TestMessage msg(Asserter::format("Fixture %s collapsed", fixtureName.c_str()),
                    Asserter::format("Connection %s - %s is failed!",
                                     cnn->from(), cnn->to()),
                    RTF_SOURCEFILE(), RTF_SOURCELINE());
    dispatcher->fixtureCollapsed(msg);
}
