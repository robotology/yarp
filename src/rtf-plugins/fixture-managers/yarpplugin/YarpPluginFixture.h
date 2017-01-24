/*
 * Copyright: (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Nicolò Genesio <nicolo.genesio@iit.it>
 * Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
    if(dynamic_cast<RTF::TestSuit*>(getDispatcher()) == 0) {\
        RTF_ASSERT_ERROR("RTF_FIXTURE_REPORT cannot get any TestSuit instance from dispacher!"); }\
    RTF::Asserter::report(RTF::TestMessage("reports",\
                                            message,\
                                            RTF_SOURCEFILE(),\
                                            RTF_SOURCELINE()),\
                                            dynamic_cast<RTF::TestSuit*>(getDispatcher()))

class YarpPluginFixture : public RTF::FixtureManager {
public:
    virtual bool setup(int argc, char** argv);
    virtual bool check();
    virtual void tearDown();
    bool scanPortmonitor(yarp::os::ConstString name);
private:
    yarp::os::Bottle devices;
    yarp::os::Bottle portmonitor;
    yarp::os::Bottle carriers;
};

#endif // YARP_RTF_PLUGINS_YARPPLUGIN_YARPPLUGINFIXTURE_H
