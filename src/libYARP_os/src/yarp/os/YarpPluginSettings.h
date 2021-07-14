/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_YARPPLUGINSETTINGS_H
#define YARP_OS_YARPPLUGINSETTINGS_H

#include <yarp/os/api.h>

#include <yarp/os/Log.h>
#include <yarp/os/SharedLibraryClass.h>
#include <yarp/os/YarpPluginSelector.h>

namespace yarp {
namespace os {

/**
 * Collect hints for finding a particular plugin.
 */
class YARP_os_API YarpPluginSettings
{
public:
    /**
     * Constructor.
     */
    YarpPluginSettings() = default;

    /**
     * Set the name of the library to load and the method name to use
     * as a factory.  The library can include a complete path, or
     * be left to the operating system to find according to the regular
     * search path for libraries.  Prefixes and suffixes such as
     * "lib", ".dll", ".so", ".dylib" may be omitted.
     *
     * @param dll_name the name of the shared library to load
     * @param fn_name the name of the method within that library to use
     * as a factory
     */
    void setLibraryMethodName(const std::string& dll_name,
                              const std::string& fn_name)
    {
        this->dll_name = dll_name;
        this->fn_name = fn_name;
    }

    /**
     * Set the name of the plugin to load.  If a corresponding YARP
     * configuration file can be found, this may be a sufficient
     * hint for YARP to fill in everything else.
     *
     * @param name the name of the plugin to load
     */
    void setPluginName(const std::string& name)
    {
        this->name = name;
    }

    /**
     * Set the information about the class and the base class
     * constructed by this plugin.
     *
     * @param class_name the name of the class
     * @param baseclass_name the name of the base class
     */
    void setClassInfo(const std::string& class_name,
                      const std::string& baseclass_name)
    {
        this->class_name = class_name;
        this->baseclass_name = baseclass_name;
    }


    /**
     * Use a selector to find a plugin or plugins.  If the name
     * of the plugin has already been set, the selector will be used to
     * increase what is known about the plugin.
     *
     * @param selector the yarp::os::YarpPluginSelector to use
     *
     * @return true if a section about the plugin was found
     */
    bool setSelector(YarpPluginSelector& selector)
    {
        this->selector = &selector;
        if (name != "") {
            return readFromSelector(name);
        }
        return false;
    }

#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
    /**
     * Should messages be printed showing what searches YARP is trying out?
     *
     * @param verbose verbosity flag
     * @deprecated since YARP 3.4
     */
    YARP_DEPRECATED_MSG("Use log components instead")
    void setVerboseMode(bool verbose);
#endif // YARP_NO_DEPRECATED

    /**
     * Configure settings from a configuration file or other searchable
     * object.
     *
     * @param options configuration object to read from
     * @param name fallback name for plugin if not in configuration
     *
     * @return true on success
     */
    bool readFromSearchable(Searchable& options, const std::string& name)
    {
        std::string iname = options.find("library").toString();
        std::string pname = options.find("part").toString();

        if (iname == "") {
            iname = name;
        }
        if (pname == "") {
            pname = name;
        }

        if (this->name == "") {
            this->name = iname;
        }
        this->dll_name = iname;
        this->fn_name = pname;
        this->wrapper_name = options.find("wrapper").toString();
        return true;
    }

    /**
     * @return the name of the plugin, if set
     */
    std::string getPluginName() const
    {
        return name;
    }

    /**
     * @return the name of the shared library, if set
     */
    std::string getLibraryName() const
    {
        return dll_name;
    }

    /**
     * @return the name of the factory method, if set
     */
    std::string getMethodName() const
    {
        return fn_name;
    }

    /**
     * @return the selector object, if set
     */
    YarpPluginSelector* getSelector() const
    {
        return selector;
    }

    /**
     * @return the name of the wrapper, if set
     */
    std::string getWrapperName() const
    {
        return wrapper_name;
    }

    /**
     * @return the name of the class, if set
     */
    std::string getClassName() const
    {
        return class_name;
    }

    /**
     * @return the name of the base class, if set
     */
    std::string getBaseClassName() const
    {
        return class_name;
    }

    /**
     * Initialize a factory object based on the hints available.
     *
     * @param factory the factory to initialize
     *
     * @return true on success
     */
    bool open(SharedLibraryFactory& factory);

    /**
     * Give a human-readable report of the status of a factory
     *
     * @param factory the factory about which to report
     */
    void reportStatus(SharedLibraryFactory& factory) const;

    /**
     * Give a human-readable failure-to-load report, summarizing the
     * active hints.
     */
    void reportFailure() const;

private:
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) name;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) dll_name;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) fn_name;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) wrapper_name {"unknown"};
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) class_name;
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) baseclass_name;
    YarpPluginSelector* selector {nullptr};

    bool subopen(SharedLibraryFactory& factory,
                 const std::string& dll_name,
                 const std::string& fn_name);

    bool open(SharedLibraryFactory& factory,
              const std::string& dll_name,
              const std::string& fn_name);

    bool readFromSelector(const std::string& name);
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_YARPPLUGINSELECTOR_H
