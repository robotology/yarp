/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_RTF_PLUGINS_YARPPLUGIN_YARPPLUGINFIXTURE_H
#define YARP_RTF_PLUGINS_YARPPLUGIN_YARPPLUGINFIXTURE_H

#include <rtf/FixtureManager.h>
#include <rtf/Asserter.h>
#include <yarp/os/Bottle.h>

// define a helper macro for fixture message reporting
#define RTF_FIXTURE_REPORT(message)\
    if(dynamic_cast<RTF::FixtureManager*>(this) == 0) {\
        RTF_ASSERT_ERROR("RTF_FIXTURE_REPORT is called outside a FixtureManager!"); }\
    if(dynamic_cast<RTF::TestSuite*>(getDispatcher()) == 0) {\
        RTF_ASSERT_ERROR("RTF_FIXTURE_REPORT cannot get any TestSuite instance from dispacher!"); }\
    RTF::Asserter::report(RTF::TestMessage("reports",\
                                            message,\
                                            RTF_SOURCEFILE(),\
                                            RTF_SOURCELINE()),\
                                            dynamic_cast<RTF::TestSuite*>(getDispatcher()))

class YarpPluginFixture : public RTF::FixtureManager {
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

#endif // YARP_RTF_PLUGINS_YARPPLUGIN_YARPPLUGINFIXTURE_H
