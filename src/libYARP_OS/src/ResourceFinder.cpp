// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <stdio.h>

#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Bottle.h>
#include <yarp/Logger.h>
#include <yarp/String.h>

#include <ace/OS.h>

using namespace yarp::os;
using namespace yarp;

#define RTARGET stderr

class ResourceFinderHelper {
private:
    yarp::os::Bottle apps;
    yarp::os::ConstString root;
    yarp::os::ConstString policyName;
    bool verbose;
public:
    ResourceFinderHelper() {
        verbose = false;
    }

    bool addAppName(const char *appName) {
        apps.addString(appName);
        return true;
    }
    
    bool clearAppNames() {
        apps.clear();
        return true;
    }
    
    bool configureFromPolicy(const char *policyName) {
        this->policyName = policyName;
        if (verbose) {
            fprintf(RTARGET,"||| policy set to %s\n", policyName);
        }
        String rootVar = String(policyName)+"ROOT";
        const char *result = 
            ACE_OS::getenv(rootVar.c_str());
        if (result==NULL) {
            root = "";
            if (verbose) {
                fprintf(RTARGET,"||| environment variable %s not set\n", 
                        rootVar.c_str());
            }
        } else {
            root = result;
            if (verbose) {
                fprintf(RTARGET,"||| %s: %s\n", 
                        rootVar.c_str(),root.c_str());
            }
        }
        return true;
    }

    bool configureFromCommandLine(int argc, char *argv[]) {
        fprintf(RTARGET,"||| warning: not yet using command line overrides\n");
        return true;
    }

    yarp::os::ConstString findFile(const char *name) {
        printf("Resource Finder not useful yet, hang on\n");
        return "";
    }

    bool setVerbose(bool verbose) {
        this->verbose = verbose;
        return this->verbose;
    }

    bool exists(const char *fname) {
        ACE_stat s;
        int result = ACE_OS::stat(fname,&s);
        return (result==0);
	}
};

#define HELPER(x) (*((ResourceFinderHelper*)(x)))

ResourceFinder::ResourceFinder() {
    implementation = new ResourceFinderHelper;
    YARP_ASSERT(implementation!=NULL);
}

ResourceFinder::~ResourceFinder() {
    if (implementation!=NULL) {
        delete &HELPER(implementation);
        implementation = NULL;
    }
}


bool ResourceFinder::configureFromPolicy(const char *policyName) {
    return HELPER(implementation).configureFromPolicy(policyName);
}

bool ResourceFinder::configureFromCommandLine(int argc, char *argv[]) {
    return HELPER(implementation).configureFromCommandLine(argc,argv);
}

bool ResourceFinder::addAppName(const char *appName) {
    return HELPER(implementation).addAppName(appName);
}

bool ResourceFinder::clearAppNames() {
    return HELPER(implementation).clearAppNames();
}

yarp::os::ConstString ResourceFinder::findFile(const char *name) {
    return HELPER(implementation).findFile(name);
}


bool ResourceFinder::setVerbose(bool verbose) {
    return HELPER(implementation).setVerbose(verbose);
}
