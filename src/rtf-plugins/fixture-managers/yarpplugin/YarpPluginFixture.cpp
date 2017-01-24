/*
 * Copyright: (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Nicolò Genesio <nicolo.genesio@iit.it>
 * Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "YarpPluginFixture.h"

#include <cstdio>
#include <ctime>
#include <rtf/dll/Plugin.h>
#include <rtf/TestAssert.h>
#include <yarp/os/Property.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/YarpPlugin.h>
#include <yarp/os/Carriers.h>
#include <yarp/dev/Drivers.h>
#include <yarp/rtf/TestCase.h>

using namespace std;
using namespace RTF;
using namespace yarp::os;
using namespace yarp::dev;

PREPARE_FIXTURE_PLUGIN(YarpPluginFixture)

bool YarpPluginFixture::scanPortmonitor(ConstString name){
    YarpPluginSelector selector;
    selector.scan();
    Bottle lst=selector.getSelectedPlugins();
    bool res=false;
    for (int i=0; i<lst.size(); i++) {
        Value& options = lst.get(i);
        if(name == options.check("name",Value("untitled")).asString() && "portmonitor"== options.check("type",Value("untitled")).asString())
            res=true;
    }
    return res;
}

bool YarpPluginFixture::setup(int argc, char** argv) {

    if(argc<=1)
    {
        //RTF::RTF_FIXTURE_REPORT();
        RTF_ASSERT_ERROR("YarpPluginFixture: Please specify --devices and/or --portmonitor and/or --carriers parameters");
        return false;
    }
    RTF_FIXTURE_REPORT("YarpPluginFixture: setupping fixture...");
    bool resDev=false, resPlug=false, resCarr=false;
    Property prop;
    prop.fromCommand(argc, argv, false);
    if(prop.check("devices"))
    {
        devices = prop.findGroup("devices");
        if(devices.isNull())
        {
            RTF_ASSERT_ERROR("YarpPluginFixture: not found devices parameter");
        }
        resDev=true;
        for(int i=1;i<devices.size();i++)
        {
            if(Drivers::factory().find(devices.get(i).asString().c_str())==NULL)
            {
                RTF_ASSERT_ERROR("YarpPluginFixture: Unable to find "+ devices.get(i).asString() +" among the available devices");
            }
        }
    }
    else
    {
        RTF_FIXTURE_REPORT("YarpPluginFixture: missing 'devices' param. Probably not required skipping this check. Trying with 'portmonitor' param...");
    }

    if(prop.check("portmonitor"))
    {
        portmonitor = prop.findGroup("portmonitor");
        if(portmonitor.isNull())
        {
            RTF_ASSERT_ERROR("YarpPluginFixture: not found portmonitor parameter");
        }
        resPlug=true;
        for(int i=1;i<portmonitor.size();i++)
        {
            if(!scanPortmonitor(portmonitor.get(i).asString())){
                RTF_ASSERT_ERROR("YarpPluginFixture: Unable to find "+portmonitor.get(i).asString()+" among the available portmonitor");
            }
        }
    }
    else
    {
        RTF_FIXTURE_REPORT("YarpPluginFixture: missing 'portmonitor' param. Probably not required skipping this check. Trying with 'carriers' param...");
    }

    if(prop.check("carriers"))
    {
        carriers = prop.findGroup("carriers");
        if(carriers.isNull())
        {
            RTF_ASSERT_ERROR("YarpPluginFixture: not found carriers parameter");
        }
        Bottle lst=Carriers::listCarriers();
        resCarr=true;
        for(int i=1;i<carriers.size();i++)
        {
            if(lst.find(carriers.get(i).asString()).isNull()){
                RTF_ASSERT_ERROR("YarpPluginFixture: Unable to find "+carriers.get(i).asString()+" among the available carriers");
            }
        }
    }
    else
    {
        RTF_FIXTURE_REPORT("YarpPluginFixture: missing 'carriers' param. Probably not required skipping this check...");
    }

    return resDev || resPlug || resCarr;
}

bool YarpPluginFixture::check() {
    // in this case check it is not necessary
    return true;
}

void YarpPluginFixture::tearDown() {
}

