/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_ROBOTTESTINGFRAMEWORK_PLUGINS_YARPPLUGIN_YARPPLUGINFIXTURE_H
#define YARP_ROBOTTESTINGFRAMEWORK_PLUGINS_YARPPLUGIN_YARPPLUGINFIXTURE_H

#include <robottestingframework/FixtureManager.h>
#include <robottestingframework/Asserter.h>
#include <yarp/os/Bottle.h>

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

class YarpPluginFixture : public robottestingframework::FixtureManager {
public:
    bool setup(int argc, char** argv) override;
    bool check() override;
    void tearDown() override;
private:
    yarp::os::Bottle devices;
    yarp::os::Bottle plugins;
    yarp::os::Bottle portmonitors;
    yarp::os::Bottle carriers;
    bool scanPlugins(std::string name, std::string type="");
};

#endif // YARP_ROBOTTESTINGFRAMEWORK_PLUGINS_YARPPLUGIN_YARPPLUGINFIXTURE_H
