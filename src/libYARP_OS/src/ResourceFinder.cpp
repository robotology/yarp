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

using namespace yarp::os;


class ResourceFinderHelper {
private:
    yarp::os::Bottle apps;
    yarp::os::ConstString root;
    yarp::os::ConstString policyName;
public:

    bool addAppName(const char *appName) {
        apps.addString(appName);
        return true;
    }

    bool clearAppNames() {
        apps.clear();
        return true;
    }

    yarp::os::ConstString findFile(const char *name) {
        printf("Resource Finder not useful yet, hang on\n");
        return "";
    }

    bool configureFromPolicy(const char *policyName) {
        this->policyName = policyName;
        return true;
    }

    bool configureFromCommandLine(int argc, char *argv[]) {
        printf("Resource Finder does not yet accept command line overrides\n");
        return true;
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


