// -*- mode:C++ { } tab-width:4 { } c-basic-offset:4 { } indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <rtf/dll/Plugin.h>
#include <yarp/manager/utility.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>

#include "YarpFixManager.h"

using namespace RTF;
using namespace yarp::os;
using namespace yarp::manager;

PREPARE_FIXTURE_PLUGIN(YarpFixManager)

YarpFixManager::YarpFixManager()
    : initialized(false) { }

YarpFixManager::~YarpFixManager() {
        tearDown();
}


bool YarpFixManager::setup(int argc, char** argv) {
    RTF_FIXTURE_REPORT("yarpmanager is setuping the fixture...");
    bool ret;
    if(!initialized) {
        // check yarp network
        yarp.setVerbosity(-1);
        RTF_ASSERT_ERROR_IF(yarp.checkNetwork(),
                            "YARP network does not seem to be available, is the yarp server accessible?");

        // load the config file and update the environment if available
        // E.g., "--application myapp.xml"
        yarp::os::ResourceFinder rf;
        rf.setVerbose(false);
        rf.setDefaultContext("RobotTesting");
        rf.configure(argc, argv, false);

        RTF_ASSERT_ERROR_IF(rf.check("fixture"),
                            "No application xml file is set (add --fixture yourfixture.xml)");

        fixtureName = rf.find("fixture").asString();
        std::string appfile = rf.findFileByName(std::string("fixtures/"+fixtureName).c_str());
        RTF_ASSERT_ERROR_IF(appfile.size(),
                            RTF::Asserter::format("yarpmanager cannot find apllication file %s. Is it in the 'fixtures' folder?",
                                                  fixtureName.c_str()));

        // enable restricted mode to ensure all the modules
        // is running and enable watchdog to monitor the modules.
        enableWatchDog();
        enableAutoConnect();
        enableRestrictedMode();

        // load the fixture (application xml)
        char szAppName[255];
        ret = addApplication(appfile.c_str(), szAppName, 254);
        RTF_ASSERT_ERROR_IF(ret,
                            "yarpmanager (addApplication) cannot setup the fixture because " +
                            std::string(getLogger()->getFormatedErrorString()));

        ret = loadApplication(szAppName);
        RTF_ASSERT_ERROR_IF(ret,
                            "yarpmanager (loadApplication) cannot setup the fixture because " +
                            std::string(getLogger()->getFormatedErrorString()));
        initialized = true;
    }

    //run the modules and connect
    ret = run();
    RTF_ASSERT_ERROR_IF(ret,
                        "yarpmanager (run) cannot setup the fixture because " +
                        std::string(getLogger()->getFormatedErrorString()));
    return true;
}

void YarpFixManager::tearDown() {
    RTF_FIXTURE_REPORT("yarpmanager is tearing down the fixture...");
    bool ret = stop();
    if(!ret)
        ret = kill();
    const char* szerror = getLogger()->getLastError();
    RTF_ASSERT_ERROR_IF(ret,
                        "yarpmanager cannot teardown the fixture because " +
                        std::string((szerror) ? szerror : ""));
}


void YarpFixManager::onExecutableFailed(void* which) {
    Executable* exe = (Executable*) which;
    RTF_ASSERT_ERROR_IF(exe!=NULL, "Executable is null!");
    TestMessage msg(Asserter::format("Fixture %s collapsed", fixtureName.c_str()),
                    Asserter::format("Module %s is failed!", exe->getCommand()),
                    RTF_SOURCEFILE(), RTF_SOURCELINE());
    getDispatcher()->fixtureCollapsed(msg);
}

void YarpFixManager::onCnnFailed(void* which) {
    Connection* cnn = (Connection*) which;
    RTF_ASSERT_ERROR_IF(cnn!=NULL, "Connection is null!");
    TestMessage msg(Asserter::format("Fixture %s collapsed", fixtureName.c_str()),
                    Asserter::format("Connection %s - %s is failed!",
                                     cnn->from(), cnn->to()),
                    RTF_SOURCEFILE(), RTF_SOURCELINE());
    getDispatcher()->fixtureCollapsed(msg);
}
