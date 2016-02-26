// -*- mode:C++ { } tab-width:4 { } c-basic-offset:4 { } indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2015 iCub Facility
 * Authors: Ali Paikan
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARPTESTCASE_H_
#define _YARPTESTCASE_H_

#include <string>
#include <cstring>
#include <rtf/TestCase.h>
#include <rtf/TestAssert.h>
#include <rtf/Arguments.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>

/**
 * @brief The YarpTestCase is a helper class to facilitate
 * laoding the tests settings which are developed for YARP/iCub.
 * The class simply looks for test configuration file given using "--from"
 * paramter to the test case and loads it into a yarp::os::Property object.
 * If any environment property is given using "testrunner -e" or using
 * <environment></environment> tag within suit XML file, that will be used to
 * updated the properties from the main config file.
 * Please see the example folder for how to develope a simple test plugin for iCub/Yarp.
 */
class YarpTestCase : public RTF::TestCase {
public:
    YarpTestCase(std::string name)
        : RTF::TestCase(name) { }

    bool setup(int argc, char** argv) {
        // check yarp network
        yarp.setVerbosity(-1);
        RTF_ASSERT_ERROR_IF(yarp.checkNetwork(),
                            "YARP network does not seem to be available, is the yarp server accessible?");

        // loading environment properties by parsing the string value
        // from getEnvironment().
        std::string strEnv = getEnvironment();
        yarp::os::Property envprop;
        envprop.fromArguments(strEnv.c_str());
        bool useSuitContext = envprop.check("context");

        // load the config file and update the environment if available
        // E.g., "--from mytest.ini"
        yarp::os::ResourceFinder rf;
        rf.setVerbose(false);
        if(useSuitContext)
            rf.setDefaultContext(envprop.find("context").asString().c_str());
        else
            rf.setDefaultContext("RobotTesting");
        rf.configure(argc, argv);
        yarp::os::Property property;

        if(rf.check("from")) {

            std::string cfgname = rf.find("from").asString();
            RTF_ASSERT_ERROR_IF(cfgname.size(),
                                "Empty value was set for the '--from' property");

            // loading configuration file indicated by --from
            std::string cfgfile = rf.findFileByName(cfgname.c_str());

            bool useTestContext = rf.check("context");

            // if the config file cannot be found from default context or
            // there is not any context, use the robotname environment as context
            if(!useSuitContext && !useTestContext && !cfgfile.size() && envprop.check("robotname")) {
                rf.setContext(envprop.find("robotname").asString().c_str());
                cfgfile = rf.findFileByName(cfgname.c_str());
            }
            RTF_ASSERT_ERROR_IF(cfgfile.size(),
                                RTF::Asserter::format("Cannot find configuration file %s", cfgfile.c_str()));
            RTF_TEST_REPORT(RTF::Asserter::format("Loading configuration from %s", cfgfile.c_str()));
            // update the properties with environment
            property.fromConfigFile(cfgfile.c_str(), envprop);
        }
        else
            property.fromString(rf.toString().c_str());

        return setup(property);
    }

    virtual bool setup(yarp::os::Property& property) {return false;}

private:
    yarp::os::Network yarp;
};

#endif //_YARPTESTCASE_H_
