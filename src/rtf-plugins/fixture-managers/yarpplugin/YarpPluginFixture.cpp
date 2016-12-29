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

using namespace std;
using namespace RTF;
using namespace yarp::os;
using namespace yarp::dev;

PREPARE_FIXTURE_PLUGIN(YarpPluginFixture)

bool YarpPluginFixture::setup(int argc, char** argv) {
    yInfo()<<"YarpPluginFixture: setupping fixture...";
    // do the setup here

    Property prop;
    prop.fromCommand(argc, argv, false);
    if(!prop.check("devices")) {
        yError()<<"YarpPluginFixture: missing 'devices' param.";
        return false;
    }
    devices = prop.findGroup("devices");
    if(devices.isNull())
    {
        yError()<<"YarpPluginFixture: not found devices parameter";
        return false;
    }
    bool res=true;

    for(int i=1;i<devices.size();i++)
    {
        if(Drivers::factory().find(devices.get(i).asString().c_str())==NULL)
        {
            yError()<<"Unable to find"<<devices.get(i).asString()<<"amonge the available devices";
            res=false;
        }
    }

    return res;
}

bool YarpPluginFixture::check() {
    // in this case check it is not necessary
    return true;
}

void YarpPluginFixture::tearDown() {
    yInfo()<<"YarpPluginFixture: tearing down the fixture...";
}
