/*
 * Copyright (C) 2015 Istituto Italiano di Tecnologia (IIT)
 * Authors: Ali Paikan <ali.paikan@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/rtf/TestCase.h>

#include <rtf/Arguments.h>
#include <rtf/TestAssert.h>

#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>

#include <cstring>


class yarp::rtf::TestCase::Private
{
public:
    yarp::os::Network yarp;
};


yarp::rtf::TestCase::TestCase(std::string name) :
        RTF::TestCase(name),
        mPriv(new Private)
{
}

yarp::rtf::TestCase::~TestCase()
{
    delete mPriv;
}


bool yarp::rtf::TestCase::setup(int argc, char** argv)
{
    // check yarp network
    mPriv->yarp.setVerbosity(-1);
    RTF_ASSERT_ERROR_IF_FALSE(mPriv->yarp.checkNetwork(),
                        "YARP network does not seem to be available, is the yarp server accessible?");

    // loading environment properties by parsing the string value
    // from getEnvironment().
    std::string strEnv = getEnvironment();
    yarp::os::Property envprop;
    envprop.fromArguments(strEnv.c_str());
    bool useSuiteContext = envprop.check("context");

    // load the config file and update the environment if available
    // E.g., "--from mytest.ini"
    yarp::os::ResourceFinder rf;
    rf.setVerbose(false);
    if(useSuiteContext) {
        rf.setDefaultContext(envprop.find("context").asString().c_str());
    } else {
        rf.setDefaultContext("RobotTesting");
    }
    rf.configure(argc, argv);
    yarp::os::Property property;

    if(rf.check("from")) {

        std::string cfgname = rf.find("from").asString();
        RTF_ASSERT_ERROR_IF_FALSE(cfgname.size(),
                            "Empty value was set for the '--from' property");

        // loading configuration file indicated by --from
        std::string cfgfile = rf.findFileByName(cfgname.c_str());

        bool useTestContext = rf.check("context");

        // if the config file cannot be found from default context or
        // there is not any context, use the robotname environment as context
        if(!useSuiteContext && !useTestContext && !cfgfile.size() && envprop.check("robotname")) {
            rf.setDefaultContext(envprop.find("robotname").asString().c_str());
            cfgfile = rf.findFileByName(cfgname.c_str());
        }
        RTF_ASSERT_ERROR_IF_FALSE(cfgfile.size(),
                            RTF::Asserter::format("Cannot find configuration file %s", cfgfile.c_str()));
        RTF_TEST_REPORT(RTF::Asserter::format("Loading configuration from %s", cfgfile.c_str()));
        // update the properties with environment
        property.fromConfigFile(cfgfile.c_str(), envprop);
    } else {
        property.fromString(rf.toString().c_str());
    }

    return setup(property);
}

bool yarp::rtf::TestCase::setup(yarp::os::Property& property)
{
    return false;
}
