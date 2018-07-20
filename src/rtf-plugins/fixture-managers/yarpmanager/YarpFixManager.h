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

#ifndef YARP_RTF_PLUGINS_YARPMANAGER_YARPFIXMANAGER_H
#define YARP_RTF_PLUGINS_YARPMANAGER_YARPFIXMANAGER_H

#include <yarp/os/Network.h>
#include <yarp/manager/manager.h>
#include <rtf/FixtureManager.h>
#include <rtf/TestSuite.h>
#include <rtf/TestAssert.h>

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

class YarpManagerPlugin : public yarp::manager::Manager
{
public:

    std::string fixtureName;
    YarpManagerPlugin(RTF::FixtureEvents* disp) : dispatcher(disp)
    {
        enableWatchDog();
        enableAutoConnect();
        enableRestrictedMode();
    }

protected:
    virtual void onExecutableFailed(void* which) override;
    virtual void onCnnFailed(void* which) override;
private:
    RTF::FixtureEvents* dispatcher;

};

class YarpFixManager : public RTF::FixtureManager {
public:

    YarpFixManager();
    virtual ~YarpFixManager();

    virtual bool setup(int argc, char** argv) override;

    virtual void tearDown() override;

private:
    bool initialized;
    yarp::os::Network yarp;
    YarpManagerPlugin* manager {nullptr};

};

#endif // YARP_RTF_PLUGINS_YARPMANAGER_YARPFIXMANAGER_H
