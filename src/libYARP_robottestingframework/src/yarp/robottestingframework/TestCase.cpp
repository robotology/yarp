/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/robottestingframework/TestCase.h>

#include <robottestingframework/Arguments.h>
#include <robottestingframework/TestAssert.h>

#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>

#include <cstring>


class yarp::robottestingframework::TestCase::Private
{
public:
    yarp::os::Network yarp;
};


yarp::robottestingframework::TestCase::TestCase(std::string name) :
        ::robottestingframework::TestCase(name),
        mPriv(new Private)
{
}

yarp::robottestingframework::TestCase::~TestCase()
{
    delete mPriv;
}


bool yarp::robottestingframework::TestCase::setup(int argc, char** argv)
{
    // check yarp network
    ROBOTTESTINGFRAMEWORK_ASSERT_ERROR_IF_FALSE(mPriv->yarp.checkNetwork(),
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
    if(useSuiteContext) {
        rf.setDefaultContext(envprop.find("context").asString().c_str());
    } else {
        rf.setDefaultContext("RobotTesting");
    }
    rf.configure(argc, argv);
    yarp::os::Property property;

    if(rf.check("from")) {

        std::string cfgname = rf.find("from").asString();
        ROBOTTESTINGFRAMEWORK_ASSERT_ERROR_IF_FALSE(cfgname.size(),
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
        ROBOTTESTINGFRAMEWORK_ASSERT_ERROR_IF_FALSE(cfgfile.size(),
                                                    ::robottestingframework::Asserter::format("Cannot find configuration file %s", cfgfile.c_str()));
        ROBOTTESTINGFRAMEWORK_TEST_REPORT(::robottestingframework::Asserter::format("Loading configuration from %s", cfgfile.c_str()));
        // update the properties with environment
        property.fromConfigFile(cfgfile.c_str(), envprop);
    } else {
        property.fromString(rf.toString().c_str());
    }

    return setup(property);
}

bool yarp::robottestingframework::TestCase::setup(yarp::os::Property& property)
{
    return false;
}
