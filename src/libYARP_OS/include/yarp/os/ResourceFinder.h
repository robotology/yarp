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
     * Request that information be printed to the console on how
     * resources are being found.  This is especially useful to
     * understand why resources are *not* found or the wrong resource
     * is picked up.
     *
     * @param verbose set/suppress printing of information
     *
     * @return true iff information will be printed
     *
     */
    bool setVerbose(bool verbose = true);

    /**
     *
     * Use a named policy.  Policy can affect the environment
     * variables that are checked, the directories that are searched,
     * and the order of search.
     *
     * For a policy <P>, YARP looks for an environment variable of 
     * that name.  If found, it tries to load the file <P>/<P>.ini
     * and use this to configure the search policy.
     *
     * An example ini file:
     *
     * \verbatim
     * style capability
     * capability_directory app
     * default_capability default
     * \endverbatim
     *
     * This would make the default search path include <P>/app/default
     * and an added context <C> would add <P>/app/<C> to the search path.
     *
     * More documentation to come as we develop this class...
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
    
    bool addContext(const char *contextName);

    bool clearContext();

    bool setContext(const char *contextName) {
        clearContext();
        return addContext(contextName);
    }

    yarp::os::ConstString findFile(const char *name);

private:
    void *implementation;
};


#endif

