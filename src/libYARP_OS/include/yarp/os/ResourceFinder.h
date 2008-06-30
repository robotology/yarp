// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_RESOURCEFINDER_
#define _YARP2_RESOURCEFINDER_

#include <yarp/os/ConstString.h>

namespace yarp {
    namespace os {
        class ResourceFinder;
    }
}

/**
 *
 * Helper class for finding config files and other external resources.
 *
 */
class yarp::os::ResourceFinder {
public:
    ResourceFinder();

    virtual ~ResourceFinder();

    /**
     *
     * Request that information be printed to standard output on how
     * resources are being found.  This is especially useful to
     * understand why resources are *not* found or the wrong resource
     * is picked up.
     *
     * @param verbose set/suppress printing of information
     *
     * @return true iff information will be printed
     *
     */
    bool setVerbose(bool verbose = true) {
        this->verbose = verbose;
        return this->verbose;
    }

    /**
     *
     * Use a named policy.  Policy can affect the environment
     * variables that are checked, the directories that are searched,
     * and the order of search.
     *
     * The policy name <P> you supply is currently used as follows:
     *  - The environment variable <P>_POLICY is checked.  In the
     *    future, if present this variable will let policy be 
     *    defined in detail.  For now, it does nothing, and the
     *    default YARP policy is used, which continues as follows.
     *  - The environment variable <P>_ROOT is checked.  If present,
     *    the directory it points to is stored, with <P>_ROOT/app/default
     *    being added to the search path.
     *  - Any directory <D> specified by addAppName are added to the 
     *    search path as <P>_ROOT/app/<D>
     *  - The current directory is added to the search path.
     *
     */
    bool configureFromPolicy(const char *policyName);

    /**
     *
     * Override policy choices (e.g. search path) from command line.
     *
     */
    bool configureFromCommandLine(int argc, char *argv[]);

    /**
     *
     * Sets up the finder.  Calls configureFromPolicy and
     * configureFromCommandLine.
     *
     */
    bool configure(const char *policyName, int argc, char *argv[]) {
        bool result = configureFromPolicy(policyName);
        if (!result) return result;
        result = configureFromCommandLine(argc,argv);
        return result;
    }
    
    bool addAppName(const char *appName);

    bool clearAppNames();

    bool setAppName(const char *appName) {
        clearAppNames();
        return addAppName(appName);
    }

    yarp::os::ConstString findFile(const char *name);

private:
    bool verbose;
    void *implementation;
};


#endif

