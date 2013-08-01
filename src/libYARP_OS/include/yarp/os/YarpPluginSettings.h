// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_YARPPLUGINSETTINGS_
#define _YARP2_YARPPLUGINSETTINGS_

#include <yarp/os/YarpPluginSelector.h>
#include <yarp/os/SharedLibraryClass.h>

namespace yarp {
    namespace os {
        class YarpPluginSettings;
    }
}


/**
 *
 * Collect hints for finding a particular plugin.
 *
 */
class YARP_OS_API yarp::os::YarpPluginSettings {
public:

    /**
     *
     * Constructor.
     *
     */
    YarpPluginSettings() {
        verbose = false;
        selector = 0 /*NULL*/;
    }

    /**
     *
     * Set the name of the library to load and the method name to use
     * as a factory.  The library can include a complete path, or
     * be left to the operating system to find according to the regular
     * search path for libraries.  Prefixes and suffixes such as
     * "lib", ".dll", ".so", ".dylib" may be omitted.
     *
     * @param dll_name the name of the shared library to load
     * @param fn_name the name of the method within that library to use
     * as a factory
     * 
     */
    void setLibraryMethodName(const ConstString& dll_name, 
                              const ConstString& fn_name) {
        this->dll_name = dll_name;
        this->fn_name = fn_name;
    }

    /**
     *
     * Set the name of the plugin to load.  If a corresponding YARP
     * configuration file can be found, this may be a sufficient
     * hint for YARP to fill in everything else.
     *
     * @param name the name of the plugin to load
     *
     */
    void setPluginName(const ConstString& name) {
        this->name = name;
    }

    /**
     *
     * Use a selector to find a plugin or plugins.  If the name
     * of the plugin has already been set, the selector will be used to
     * increase what is known about the plugin.
     *
     * @param selector the yarp::os::YarpPluginSelector to use
     *
     */
    void setSelector(YarpPluginSelector& selector) {
        this->selector = &selector;
        if (name != "") {
            readFromSelector(name);
        }
    }

    /**
     *
     * Should messages be printed showing what searches YARP is trying out?
     *
     * @param verbose verbosity flag
     *
     */
    void setVerboseMode(bool verbose) {
        this->verbose = verbose;
    }

    /**
     *
     * Configure settings from a configuration file or other searchable
     * object.
     *
     * @param options configuration object to read from
     * @param name fallback name for plugin if not in configuration
     *
     * @return true on success
     *
     */
    bool readFromSearchable(Searchable& options, const ConstString& name) {
        ConstString iname = options.find("library").toString();
        ConstString pname = options.find("part").toString();
        if (iname=="") iname = name;
        if (pname=="") pname = name;
        this->name = iname;
        this->dll_name = iname;
        this->fn_name = pname;
        verbose = false;
        return true;
    }

    /**
     *
     * @return the name of the plugin, if set
     *
     */
    ConstString getPluginName() const { 
        return name;
    }

    /**
     *
     * @return the name of the shared library, if set
     *
     */
    ConstString getLibraryName() const { 
        return dll_name;
    }

    /**
     *
     * @return the name of the factory method, if set
     *
     */
    ConstString getMethodName() const { 
        return fn_name;
    }

    /**
     *
     * @return the selector object, if set
     *
     */
    YarpPluginSelector *getSelector() const {
        return selector;
    }

    /**
     *
     * Initialize a factory object based on the hints available.
     *
     * @param factory the factory to initialize
     *
     * @return true on success
     *
     */
    bool open(SharedLibraryFactory& factory);

    /**
     *
     * Give a human-readable report of the status of a factory
     *
     * @param factory the factory about which to report
     *
     */
    void reportStatus(SharedLibraryFactory& factory) const;

    /**
     *
     * Give a human-readable failure-to-load report, summarizing the
     * active hints.
     *
     */
    void reportFailure() const;

private:
    ConstString name;
    ConstString dll_name;
    ConstString fn_name;
    ConstString fn_ext;
    YarpPluginSelector *selector;
    bool verbose;

    bool subopen(SharedLibraryFactory& factory, const ConstString& dll_name,
                 const ConstString& fn_name);

    bool open(SharedLibraryFactory& factory, const ConstString& dll_name,
              const ConstString& fn_name);

    bool readFromSelector(const ConstString& name) {
        if (!selector) return false;
        Bottle plugins = selector->getSelectedPlugins();
        Bottle group = plugins.findGroup(name.c_str()).tail();
        readFromSearchable(group,name);
        return true;
    }
};

#endif
