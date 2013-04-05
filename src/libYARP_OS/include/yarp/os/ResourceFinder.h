// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_RESOURCEFINDER_
#define _YARP2_RESOURCEFINDER_

#include <yarp/os/ConstString.h>
#include <yarp/os/Searchable.h>
#include <yarp/os/Value.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinderOptions.h>

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
class YARP_OS_API yarp::os::ResourceFinder : public Searchable{
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
     * Request that information be suppressed from the console.  By
     * default ResourceFinder will print messages if it fails to find
     * files, for example.
     *
     * @param quiet suppress printing of information
     *
     * @return true iff information will be suppressed
     *
     */
    bool setQuiet(bool quiet = true);

    /**
     *
     * Sets up the finder.  The policyName is used to find
     * a file containing the default policy for searching
     * for resource files.  Policy can affect the environment
     * variables that are checked, the directories that are searched,
     * and the order of search.
     *
     * For a policy [P], YARP looks for an environment variable of
     * that name.  If found, it tries to load the file [P]/[P].ini
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
     * This would make the default search path include [P]/app/default
     * and an added context [C] would add [P]/app/[C] to the search path.
     *
     * Some elements of policy can be changed from the commandline.
     *
     */
    bool configure(const char *policyName, int argc, char *argv[],
                   bool skipFirstArgument = true);

    /**
     *
     * Variant of configure() that doesn't require a policy name
     * to be specified.  In this case, policy will be read from
     * an environment variable, YARP_POLICY.
     *
     */
    bool configure(int argc, char *argv[]) {
        return configure("",argc,argv,true);
    }

    bool setDefaultContext(const char *contextName) {
        clearContext();
        return addContext(contextName);
    }

    /**
     *
     * Deprecated name for setDefaultContext
     *
     */
    bool setContext(const char *contextName) {
        return setDefaultContext(contextName);
    }

    bool setDefault(const char *key, const char *val);

    bool setDefaultConfigFile(const char *fname) {
        return setDefault("from",fname);
    }

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

    /**
     *
     * Expand a partial path to a full path.  The path is specified by the
     * name of a key.  The value of the key should be set up either
     * on the command line, through a loaded config file, or
     * by a call to setDefault.
     *
     * If all else fails, findPath will try interpreting key as
     * a path - this is for backwards compatibility
     * and is behavior that will probably go away - don't
     * depend on it!
     *
     */
    yarp::os::ConstString findPath(const char *key);

    /**
     *
     * Expand a partial path to a list of paths.
     * Like findPath(key), but continues on to find all
     * instances of the path.
     * 
     * so findPaths("app") would return ["/foo/app","/bar/app",...]
     * depending on the search path in effect.
     * The first path is the list comes from the highest-priority
     * location, and would be the path returned by findPath("app")
     *
     */
    yarp::os::Bottle findPaths(const char *key);

    /**
     *
     * Find the first existing directory in the search path.
     *
     */
    yarp::os::ConstString findPath();


    /**
     *
     * Return the default "context" or "application name" used in searching
     * for configuration files.  The context is a keyword that is converted
     * into a search path in a policy-specific way.
     *
     */
    yarp::os::ConstString getContext();

    /**
     *
     * Return the path that the default context expands to, according to
     * the policy.
     *
     */
    yarp::os::ConstString getContextPath();

    /**
     *
     * Return the full stack of contexts used in searching
     * for configuration files.
     *
     */
    yarp::os::Bottle getContexts();

    // Searchable interface
    virtual bool check(const char *key);
    virtual Value& find(const char *key);
    virtual Bottle& findGroup(const char *key);
    virtual bool isNull() const;
    virtual ConstString toString() const;


    /**
     * Gets a section as a ResourceFinder object, retaining the
     * context and configuration of the current ResourceFinder.
     * This is a thin wrapper around the Searchable::findGroup method.
     * @param key The section to look for
     * @return A ResourceFinder corresponding to the named section
     *
     */
    virtual ResourceFinder findNestedResourceFinder(const char *key);


    bool isConfigured() const {
        return isConfiguredFlag;
    }

    /**
     *
     * Access a ResourceFinder singleton whose lifetime will match that of
     * the YARP library.
     *
     * @return the ResourceFinder singleton
     *
     */
    static ResourceFinder& getResourceFinderSingleton();

    using Searchable::check;
    using Searchable::findGroup;


    /* YARP 2.4 changes begin */

    /**
     *
     * Location where user data files are stored.
     * If $YARP_DATA_HOME is set, that is returned.
     * Otherwise:
     *   If $XDG_DATA_HOME is set, "yarp" is appended to it after the 
     *   OS-appropriate directory separator, and the result returned.
     *   Otherwise:
     *     On Windows
     *       %APPDATA%\yarp is returned.
     *     On Linux and all others:
     *       $HOME/.local/share is returned.
     *     (an OSX-specific case remains to be defined)
     *
     */
    static ConstString getDataHome();

    /**
     *
     * Location where user config files are stored.
     * If $YARP_CONFIG_HOME is set, that is returned.
     * Otherwise:
     *   If $XDG_CONFIG_HOME is set, "yarp" is appended to it after the 
     *   OS-appropriate directory separator, and the result returned.
     *   Otherwise:
     *     On Windows
     *       %APPDATA%\yarp\config is returned.
     *     On Linux and all others:
     *       $HOME/.config/yarp is returned.
     *     (an OSX-specific case remains to be defined)
     *
     */
    static ConstString getConfigHome();

    /**
     *
     * Locations where packaged data and config files are stored.
     * If $YARP_DATA_DIRS is set, that is returned.
     * Otherwise:
     *   If $XDG_DATA_DIRS is set, "/yarp" or "\yarp" as appropriate
     *   is appended to each path and the result returned.
     *   Otherwise:
     *     On Windows
     *       %YARP_DIR%\share\yarp
     *     On Linux and all others:
     *       /usr/local/share/yarp:/usr/share/yarp is returned.
     *     (an OSX-specific case remains to be defined)
     *
     */
    static Bottle getDataDirs();

    /**
     *
     * Locations where system administrator data and config files are stored.
     * If $YARP_CONFIG_DIRS is set, that is returned.
     * Otherwise:
     *   If $XDG_CONFIG_DIRS is set, "/yarp" or "\yarp" as appropriate
     *   is appended to each path and the result returned.
     *   Otherwise:
     *     On Windows
     *       %ALLUSERSPROFILE%\yarp
     *     On Linux and all others:
     *       /etc/yarp is returned.
     *     (an OSX-specific case remains to be defined)
     *
     */
    static Bottle getConfigDirs();

    yarp::os::Bottle findPaths(const char *key,
                               const ResourceFinderOptions& options);

    yarp::os::ConstString findPath(const char *key,
                                   const ResourceFinderOptions& options);

    yarp::os::ConstString findFile(const char *key,
                                   const ResourceFinderOptions& options);

    /* YARP 2.4 changes end */

private:

    // this might be useful, but is not in spec
    bool addContext(const char *contextName);

    // this might be useful, but is not in spec
    bool clearContext();

    void *implementation;
    bool owned;
    bool nullConfig;
    bool isConfiguredFlag;
    yarp::os::Property config;

    ResourceFinder(Searchable& data, void *implementation);
};


#endif

