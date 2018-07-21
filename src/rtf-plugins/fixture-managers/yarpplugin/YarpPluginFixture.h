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
    virtual bool setup(int argc, char** argv) override;
    virtual bool check() override;
    virtual void tearDown() override;
private:
    yarp::os::Bottle devices;
    yarp::os::Bottle plugins;
    yarp::os::Bottle portmonitors;
    yarp::os::Bottle carriers;
    bool scanPlugins(std::string name, std::string type="");
};

#endif // YARP_RTF_PLUGINS_YARPPLUGIN_YARPPLUGINFIXTURE_H
