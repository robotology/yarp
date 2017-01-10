/*
*  Copyright: (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
* Authors: Nicolo' Genesio <nicolo.genesio@iit.it>
* CopyPolicy: Released under the terms of the GNU GPL v2.0.
*/

#include <cstdio>
#include <ctime>
#include <rtf/dll/Plugin.h>
#include "YarpPluginFixture.h"
#include <yarp/os/Property.h>
#include <yarp/os/LogStream.h>
#include <yarp/dev/Drivers.h>
#include <yarp/os/YarpPlugin.h>
#include <yarp/os/ResourceFinder.h>

using namespace std;
using namespace RTF;
using namespace yarp::os;
using namespace yarp::dev;

PREPARE_FIXTURE_PLUGIN(YarpPluginFixture)

bool YarpPluginFixture::scanPlugins(ConstString name){
    YarpPluginSelector selector;
    selector.scan();
    Bottle lst=selector.getSelectedPlugins();
    bool res=false;
    for (int i=0; i<lst.size(); i++) {
        Value& options = lst.get(i);
        if(name == options.check("name",Value("untitled")).asString())
            res=true;
    }
    return res;
}

bool YarpPluginFixture::setup(int argc, char** argv) {
    if(argc<=1)
    {
        yError()<<"Please specify --devices and/or --plugins parameters";
        return false;
    }
    yInfo()<<"YarpPluginFixture: setupping fixture...";
    bool resDev=true, resPlug=true;
    Property prop;
    prop.fromCommand(argc, argv, false);
    if(prop.check("devices"))
    {
        devices = prop.findGroup("devices");
        if(devices.isNull())
        {
            yError()<<"YarpPluginFixture: not found devices parameter";
            resDev=false;
        }
        for(int i=1;i<devices.size();i++)
        {
            if(Drivers::factory().find(devices.get(i).asString().c_str())==NULL)
            {
                yError()<<"YarpPluginFixture: Unable to find"<<devices.get(i).asString()<<"among the available devices";
                resDev=false;
            }
        }
    }
    else
    {
        yWarning()<<"YarpPluginFixture: missing 'devices' param. Probably not required skipping this check. Trying with 'plugins' param...";
    }

    if(prop.check("plugins"))
    {
        plugins = prop.findGroup("plugins");
        if(plugins.isNull())
        {
            yError()<<"YarpPluginFixture: not found plugins parameter";
            resPlug=false;
        }
        for(int i=1;i<plugins.size();i++)
        {
            if(!scanPlugins(plugins.get(i).asString())){
                yError()<<"YarpPluginFixture: Unable to find"<<plugins.get(i).asString()<<"among the available plugins";
                resPlug=false;
            }
        }
    }
    else
    {
        yWarning()<<"YarpPluginFixture: missing 'plugins' param. Probably not required skipping this check...";
        resPlug=prop.check("devices");
    }


    return resDev && resPlug;
}

bool YarpPluginFixture::check() {
    // in this case check it is not necessary
    return true;
}

void YarpPluginFixture::tearDown() {
    yInfo()<<"YarpPluginFixture: tearing down the fixture...";
}
