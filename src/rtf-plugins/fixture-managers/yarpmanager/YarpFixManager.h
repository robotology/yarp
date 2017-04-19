/*
 * Copyright (C) 2015 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_RTF_PLUGINS_YARPMANAGER_YARPFIXMANAGER_H
#define YARP_RTF_PLUGINS_YARPMANAGER_YARPFIXMANAGER_H

#include <yarp/os/Network.h>
#include <yarp/manager/manager.h>
#include <rtf/FixtureManager.h>
#include <rtf/TestSuit.h>
#include <rtf/TestAssert.h>

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

class YarpFixManager : public RTF::FixtureManager,
        yarp::manager::Manager {
public:

    YarpFixManager();
    virtual ~YarpFixManager();

    virtual bool setup(int argc, char** argv);

    virtual void tearDown();

protected:
    //virtual void onExecutableStart(void* which);
    //virtual void onExecutableStop(void* which);
    //virtual void onExecutableDied(void* which);
    //virtual void onCnnStablished(void* which);
    virtual void onExecutableFailed(void* which);
    virtual void onCnnFailed(void* which);

private:
    bool initialized;
    yarp::os::Network yarp;
    std::string fixtureName;
};

#endif // YARP_RTF_PLUGINS_YARPMANAGER_YARPFIXMANAGER_H
