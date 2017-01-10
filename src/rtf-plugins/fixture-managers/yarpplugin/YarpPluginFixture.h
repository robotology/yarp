/*
* Copyright: (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
* Authors: Nicolo' Genesio <nicolo.genesio@iit.it>
* CopyPolicy: Released under the terms of the GNU GPL v2.0.
*/


#ifndef _YARP_PLUGIN_FIXTURE_H_
#define _YARP_PLUGIN_FIXTURE_H_

#include <rtf/FixtureManager.h>
#include <yarp/os/Bottle.h>

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
