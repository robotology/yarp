/*
* Copyright: (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
* Authors: Nicolo' Genesio <nicolo.genesio@iit.it>
* CopyPolicy: Released under the terms of the GNU GPL v2.0.
*/


#ifndef _YARP_PLUGIN_FIXTURE_H_
#define _YARP_PLUGIN_FIXTURE_H_

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
    bool scanPlugins(yarp::os::ConstString name);
private:
    yarp::os::Bottle devices;
    yarp::os::Bottle plugins;
};

#endif //_YARP_PLUGIN_FIXTURE_H_
