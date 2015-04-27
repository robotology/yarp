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
#include <TestCase.h>
#include <TestAssert.h>
#include <Arguments.h>
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

        // load the config file and update the environment if available
        // E.g., "--from mytest.ini"
        yarp::os::ResourceFinder rf;
        rf.setVerbose(false);
        rf.setDefaultContext("RobotTesting");
        rf.configure(argc, argv);
        yarp::os::Property property;

        std::string strEnv = getEnvironment();
        if(rf.check("from") && strEnv.size()) {

            std::string cfgname = rf.find("from").asString();
            RTF_ASSERT_ERROR_IF(cfgname.size(),
                                "Empty value was set for the '--from' property");

            // loading environment properties by parsing the string value
            // from getEnvironment().
            yarp::os::Property envprop;
            char* szenv = new char[strEnv.size()+1];
            strcpy(szenv, strEnv.c_str());
            int argc = 0;
            char** argv = new char*[128]; // maximum 128
            RTF::Arguments::parse(szenv, &argc, argv);
            argv[argc]=0;
            envprop.fromCommand(argc, argv, false);
            delete [] szenv;
            delete [] argv;

            // loading configuration file indicated by --from
            std::string cfgfile = rf.findFileByName(cfgname.c_str());

            // if the config file cannot be found from default context or
            // there is not any context, use the robotname environment as context
            if(!cfgfile.size() && envprop.check("robotname")) {
                rf.setContext(envprop.find("robotname").asString().c_str());
                cfgfile = rf.findFileByName(cfgname.c_str());
            }
            RTF_ASSERT_ERROR_IF(cfgfile.size(),
                                RTF::Asserter::format("Cannot find configuratio file %s", cfgfile.c_str()));

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
