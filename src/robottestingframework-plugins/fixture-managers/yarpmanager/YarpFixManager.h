/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_ROBOTTESTINGFRAMEWORK_PLUGINS_YARPMANAGER_YARPFIXMANAGER_H
#define YARP_ROBOTTESTINGFRAMEWORK_PLUGINS_YARPMANAGER_YARPFIXMANAGER_H

#include <yarp/os/Network.h>
#include <yarp/manager/manager.h>
#include <robottestingframework/FixtureManager.h>
#include <robottestingframework/TestSuite.h>
#include <robottestingframework/TestAssert.h>

// define a helper macro for fixture message reporting
#define ROBOTTESTINGFRAMEWORK_FIXTURE_REPORT(message)\
    if(dynamic_cast<robottestingframework::FixtureManager*>(this) == 0) {\
        ROBOTTESTINGFRAMEWORK_ASSERT_ERROR("ROBOTTESTINGFRAMEWORK_FIXTURE_REPORT is called outside a FixtureManager!"); }\
    if(dynamic_cast<robottestingframework::TestSuite*>(getDispatcher()) == 0) {\
        ROBOTTESTINGFRAMEWORK_ASSERT_ERROR("ROBOTTESTINGFRAMEWORK_FIXTURE_REPORT cannot get any TestSuite instance from dispacher!"); }\
    robottestingframework::Asserter::report(robottestingframework::TestMessage("reports",\
                                            message,\
                                            ROBOTTESTINGFRAMEWORK_SOURCEFILE(),\
                                            ROBOTTESTINGFRAMEWORK_SOURCELINE()),\
                                            dynamic_cast<robottestingframework::TestSuite*>(getDispatcher()))

class YarpManagerPlugin : public yarp::manager::Manager
{
public:

    std::string fixtureName;
    YarpManagerPlugin(robottestingframework::FixtureEvents* disp) : dispatcher(disp)
    {
        enableWatchDog();
        enableAutoConnect();
        enableRestrictedMode();
    }

protected:
    void onExecutableFailed(void* which) override;
    void onCnnFailed(void* which) override;
private:
    robottestingframework::FixtureEvents* dispatcher;

};

class YarpFixManager : public robottestingframework::FixtureManager {
public:

    YarpFixManager();
    virtual ~YarpFixManager();

    bool setup(int argc, char** argv) override;

    void tearDown() override;

private:
    bool initialized;
    yarp::os::Network yarp;
    YarpManagerPlugin* manager {nullptr};

};

#endif // YARP_ROBOTTESTINGFRAMEWORK_PLUGINS_YARPMANAGER_YARPFIXMANAGER_H
