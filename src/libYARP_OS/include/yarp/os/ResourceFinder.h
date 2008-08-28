// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_RESOURCEFINDER_
#define _YARP2_RESOURCEFINDER_

#include <yarp/os/ConstString.h>
#include <yarp/os/Searchable.h>

namespace yarp {
    namespace os {
        class ResourceFinder;
    }
}

/**
 *
 * Helper class for finding config files and other external resources.
 * This is under development at the moment.  See ideas at:
 * http://eris.liralab.it/wiki/Configuration_and_resource_files
 *
 */
class yarp::os::ResourceFinder : public Searchable{
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
     * Sets up the finder.  The policyName is used to find
     * a file containing the default policy for searching
     * for resource files.  Policy can affect the environment
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
     * Some elements of policy can be changed from the commandline.
     *
     */
    bool configure(const char *policyName, int argc, char *argv[], 
                   bool skipFirstArgument = true);
    
    bool addContext(const char *contextName);

    bool clearContext();

    bool setContext(const char *contextName) {
        clearContext();
        return addContext(contextName);
    }

    bool setDefaultContext(const char *contextName) {
        clearContext();
        return addContext(contextName);
    }

    bool setDefault(const char *key, const char *val);

    /**
     *
     * Find the full path to a file.  The file is specified by the
     * name of a key.  The value of the key should be set up either
     * on the command line, through a loaded config file, or
     * by a call to setDefault.
     *
     * If all else fails, findFile will try interpreting key as
     * a file name - this is for backwards compatibility
     * and is behavior that will probably go away - don't
     * depend on it!
     *
     */
    yarp::os::ConstString findFile(const char *key);


    // Searchable interface
    virtual bool check(const char *key);
    virtual Value& find(const char *key);
    virtual Bottle& findGroup(const char *key);
    virtual bool isNull() const;
    virtual ConstString toString() const;

private:
    void *implementation;
};


#endif

