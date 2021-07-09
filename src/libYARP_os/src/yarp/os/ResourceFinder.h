/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_RESOURCEFINDER_H
#define YARP_OS_RESOURCEFINDER_H

#include <yarp/os/Bottle.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinderOptions.h>
#include <yarp/os/Searchable.h>
#include <yarp/os/Value.h>

#include <string>

namespace yarp {
namespace os {

/**
 *
 * Helper class for finding config files and other external resources.
 *
 * More details on this class behaviour can be found in
 * \ref yarp_resource_finder_tutorials.
 *
 */
class YARP_os_API ResourceFinder : public Searchable
{
public:
    ResourceFinder();

    ResourceFinder(const ResourceFinder& alt);

    virtual ~ResourceFinder();

    const ResourceFinder& operator=(const ResourceFinder& alt);

    /**
     * Sets up the ResourceFinder.
     *
     * @return true if configuration succeeded. Configuration fails if the user
     * requests a configuration file to be read (via --from for example) and
     * that file cannot be found. If a default configuration file has been set
     * with ResourceFinder::setDefaultConfigFile, the presence or absence of
     * that file doesn't by itself contribute to  success/failure (since it is
     * perfectly valid for it to be
     * absent).
     */
    bool configure(int argc, char* argv[], bool skipFirstArgument = true);

    /**
     * Sets the context for the current ResourceFinder object.
     *
     * @param contextName The name of the context
     * @return true on success, false otherwise
     *
     * @see setDefaultContext(const char *contextName)
     */
    bool setDefaultContext(const std::string& contextName)
    {
        clearContext();
        return addContext(contextName);
    }

    /**
     * Provide a default value for a given key.
     *
     * The provided \c val will be converted to a yarp::os::Value, so also
     * string representations for lists and numerical values are accepted.
     */
    bool setDefault(const std::string& key, const std::string& val);

    /**
     * Provide a default value for a given key.
     *
     * The provided \c val will be converted to a 32-bit integer
     * yarp::os::Value.
     */
    bool setDefault(const std::string& key, std::int32_t val);

    /**
     * Provide a default value for a given key.
     *
     * The provided \c val will be converted to a 64-bit floating point
     * yarp::os::Value.
     */
    bool setDefault(const std::string& key, yarp::conf::float64_t val);

    /**
     *
     * Provide a default value for a given key
     *
     */
    bool setDefault(const std::string& key, const yarp::os::Value& val);

    /**
     *
     * Provide a default value for the configuration file
     * (can be overridden from command line with the --from argument)
     *
     */
    bool setDefaultConfigFile(const std::string& fname)
    {
        return setDefault("from", fname);
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
     * The file is searched in a hierarchy of paths as defined in
     * \ref yarp_resource_finder_tutorials.
     *
     */
    std::string findFile(const std::string& name);

    /**
     *
     * Find the full path to a file. The name of the file is provided explicitly.
     *
     * The file is searched in a hierarchy of paths as defined in
     * \ref yarp_resource_finder_tutorials.
     */
    std::string findFileByName(const std::string& name);

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
     * The path is searched in a hierarchy of paths as defined in
     * \ref yarp_resource_finder_tutorials.
     */
    std::string findPath(const std::string& name);

    /**
     *
     * Expand a partial path to a list of paths.
     * Like findPath(key), but continues on to find all
     * instances of the path.
     *
     * so findPaths("app") would return ["/foo/app", "/bar/app", ...]
     * depending on the search path in effect.
     * The first path is the list comes from the highest-priority
     * location, and would be the path returned by findPath("app")
     *
     * The path is searched in a hierarchy of paths as defined in
     * \ref yarp_resource_finder_tutorials.
     */
    yarp::os::Bottle findPaths(const std::string& name);

    /**
     *
     * Find the first existing directory in the search path.
     *
     */
    std::string findPath();


    /**
     *
     * Return the default "context" or "application name" used in searching
     * for configuration files.  The context is a keyword that is converted
     * into a search path in a policy-specific way.
     *
     */
    std::string getContext();

    /**
     *
     * Return the full stack of contexts used in searching
     * for configuration files.
     *
     */
    yarp::os::Bottle getContexts();

    // Searchable interface
    bool check(const std::string& key) const override;
    Value& find(const std::string& key) const override;
    Bottle& findGroup(const std::string& key) const override;
    bool isNull() const override;
    std::string toString() const override;


    /**
     * Gets a section as a ResourceFinder object, retaining the
     * context and configuration of the current ResourceFinder.
     * This is a thin wrapper around the Searchable::findGroup method.
     * @param key The section to look for
     * @return A ResourceFinder corresponding to the named section
     *
     */
    virtual ResourceFinder findNestedResourceFinder(const std::string& key);


    bool isConfigured() const
    {
        return m_isConfiguredFlag;
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

    /**
     * Return the path to the "user" context directory for the current context
     */
    std::string getHomeContextPath();

    /**
     * Return the path to the "user" robot directory
     */
    std::string getHomeRobotPath();

    yarp::os::Bottle findPaths(const std::string& name,
                               const ResourceFinderOptions& options);

    std::string findPath(const std::string& name,
                         const ResourceFinderOptions& options);

    std::string findFile(const std::string& name,
                         const ResourceFinderOptions& options);

    std::string findFileByName(const std::string& name,
                               const ResourceFinderOptions& options);

    bool readConfig(Property& config,
                    const std::string& key,
                    const ResourceFinderOptions& options);

#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
    /**
     * Request that information be printed to the console on how
     * resources are being found.  This is especially useful to
     * understand why resources are *not* found or the wrong resource
     * is picked up.
     *
     * @param verbose set/suppress printing of information
     *
     * @return true iff information will be printed
     * @deprecated since YARP 3.4
     */
    bool setVerbose(bool verbose = true);

    /**
     * Request that information be suppressed from the console.  By
     * default ResourceFinder will print messages if it fails to find
     * files, for example.
     *
     * @param quiet suppress printing of information
     *
     * @return true iff information will be suppressed
     * @deprecated since YARP 3.4
     */
    bool setQuiet(bool quiet = true);
#endif // YARP_NO_DEPRECATED

#ifndef YARP_NO_DEPRECATED // SINCE YARP 3.5
    /**
     * Location where user data files are stored.
     * If $YARP_DATA_HOME is set, that is returned.  We do not check
     * to see if that directory exists.
     * Otherwise:
     *   (In all the following cases, we attempt to create the directory
     *   returned if it does not already exist).
     *   If $XDG_DATA_HOME is set, "yarp" is appended to it after the
     *   OS-appropriate directory separator, and the result returned.
     *   Otherwise:
     *     On Windows
     *       %APPDATA%\yarp is returned.
     *     On Linux and all others:
     *       $HOME/.local/share is returned.
     *     (an OSX-specific case remains to be defined)
     *
     * @deprecated since YARP 3.5
     */
    YARP_DEPRECATED_MSG("Use yarp::conf::dirs::data_home() + yarp::os::mkdir_p() instead")
    static std::string getDataHome()
    {
        return getDataHomeWithPossibleCreation(true);
    }


    /**
     * Variant of getDataHome that will never create the directory
     * returned.
     *
     * @deprecated since YARP 3.5
     */
    YARP_DEPRECATED_MSG("Use yarp::conf::dirs::data_home() instead")
    static std::string getDataHomeNoCreate()
    {
        return getDataHomeWithPossibleCreation(false);
    }

    /**
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
     * @deprecated since YARP 3.5
     */
    YARP_DEPRECATED_MSG("Use yarp::conf::dirs::config_home() + yarp::os::mkdir_p() instead")
    static std::string getConfigHome()
    {
        return getConfigHomeWithPossibleCreation(true);
    }

    /**
     * Variant of getConfigHome that will never create the directory
     * returned.
     *
     * @deprecated since YARP 3.5
     */
    YARP_DEPRECATED_MSG("Use yarp::conf::dirs::config_home() instead")
    static std::string getConfigHomeNoCreate()
    {
        return getConfigHomeWithPossibleCreation(false);
    }

    /**
     * Locations where packaged data and config files are stored.
     * If $YARP_DATA_DIRS is set, that is returned.
     * Otherwise:
     *   If $XDG_DATA_DIRS is set, "/yarp" or "\yarp" as appropriate
     *   is appended to each path and the result returned.
     *   Otherwise:
     *     On Windows
     *       %ALLUSERSPROFILE%\yarp
     *     On Linux and all others:
     *       /usr/local/share/yarp:/usr/share/yarp is returned.
     *     (an OSX-specific case remains to be defined)
     *
     * @deprecated since YARP 3.5
     */
    YARP_DEPRECATED_MSG("Use yarp::conf::dirs::yarpdatadirs() instead")
    static Bottle getDataDirs();

    /**
     * Locations where system administrator data and config files are stored.
     * If $YARP_CONFIG_DIRS is set, that is returned.
     * Otherwise:
     *   If $XDG_CONFIG_DIRS is set, "/yarp" or "\yarp" as appropriate
     *   is appended to each path and the result returned.
     *   Otherwise:
     *     On Windows
     *       %ALLUSERSPROFILE%\yarp\config
     *     On Linux and all others:
     *       /etc/xdg/yarp is returned.
     *     (an OSX-specific case remains to be defined)
     *
     * @deprecated since YARP 3.5
     */
    YARP_DEPRECATED_MSG("Use yarp::conf::dirs::config_dirs() instead")
    static Bottle getConfigDirs();
#endif // YARP_NO_DEPRECATED


private:
    // this might be useful, but is not in spec
    bool addContext(const std::string& appName);

    // this might be useful, but is not in spec
    bool clearContext();

    bool m_owned;
    bool m_nullConfig;
    bool m_isConfiguredFlag;
    yarp::os::Property m_configprop;

    static std::string createIfAbsent(bool mayCreate, const std::string& path);

#ifndef YARP_NO_DEPRECATED // SINCE YARP 3.5
    static std::string getDataHomeWithPossibleCreation(bool mayCreate);
    static std::string getConfigHomeWithPossibleCreation(bool mayCreate);
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS
private:
    class Private;
    Private* mPriv;
    ResourceFinder(Searchable& data, Private* altPriv);
#endif // DOXYGEN_SHOULD_SKIP_THIS
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_RESOURCEFINDER_H
