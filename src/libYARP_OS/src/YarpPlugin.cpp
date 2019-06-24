/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/YarpPlugin.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/NameClient.h>

#include <cstdio>
#include <cstdlib>

using namespace yarp::os;
using namespace yarp::os::impl;

bool YarpPluginSettings::open(SharedLibraryFactory& factory,
                              const std::string& dll_name,
                              const std::string& fn_name)
{
    return subopen(factory, dll_name, fn_name);
}

bool YarpPluginSettings::subopen(SharedLibraryFactory& factory,
                                 const std::string& dll_name,
                                 const std::string& fn_name)
{
    YARP_SPRINTF2(impl::Logger::get(), debug, "Trying plugin [dll: %s] [fn: %s]", dll_name.c_str(), fn_name.c_str());
    bool ok = factory.open(dll_name.c_str(), fn_name.c_str());
    if (verbose) {
        fprintf(stderr, "Trying to find library '%s' containing function '%s' -- %s\n", dll_name.c_str(), fn_name.c_str(), ok ? "found" : "fail");
    }
    if (ok) {
        YARP_SPRINTF2(impl::Logger::get(), debug, "Found plugin [dll: %s] [fn: %s]", dll_name.c_str(), fn_name.c_str());
        this->dll_name = dll_name;
        this->fn_name = fn_name;
    } else if (verbose || (factory.getStatus() != SharedLibraryFactory::STATUS_LIBRARY_NOT_FOUND)) {
        fprintf(stderr, "error while opening %s:\n  %s\n", dll_name.c_str(), factory.getError().c_str());
    }
    return ok;
}

bool YarpPluginSettings::open(SharedLibraryFactory& factory)
{
    YARP_SPRINTF3(impl::Logger::get(), debug, "Plugin [name: %s] [dll: %s] [fn: %s]", name.c_str(), dll_name.c_str(), fn_name.c_str());
    if (selector != nullptr && !name.empty()) {
        // we may have a YARP-specific search path available,
        // and a proper name for the DLL
        Bottle paths = selector->getSearchPath();
        for (size_t i = 0; i < paths.size(); i++) {
            Searchable& options = paths.get(i);
            std::string path = options.find("path").asString();
            std::string ext = options.find("extension").asString();
            std::string basename = (dll_name.find('.') != std::string::npos) ? name : dll_name;
            std::string fn = (fn_name.empty()) ? name : fn_name;

            std::string fullpath;

#if defined(_MSC_VER) && !defined(NDEBUG)
            // MSVC DEBUG build: try debug name before basic name
            fullpath = std::string(path).append("/").append(basename).append("d").append(ext);
            if (subopen(factory, fullpath, fn))
                return true;
#endif // defined(_MSC_VER) && !defined(NDEBUG)

            // Basic name
            fullpath = std::string(path).append("/").append(basename).append(ext);
            if (subopen(factory, fullpath, fn)) {
                return true;
            }

#if defined(_MSC_VER) && defined(NDEBUG)
            // MSVC RELEASE build: try debug name after basic name
            fullpath = std::string(path).append("/").append(basename).append("d").append(ext);
            if (subopen(factory, fullpath, fn))
                return true;
#endif // defined(_MSC_VER) && defined(NDEBUG)


#ifdef CMAKE_INTDIR
                // On multi-config system, try to find the plugin in the
                // current config subdirectory

#    if defined(_MSC_VER) && !defined(NDEBUG)
            // MSVC DEBUG build: try debug name before basic name
            fullpath = std::string(path).append("/" CMAKE_INTDIR "/").append(dll_name).append("d").append(ext);
            if (subopen(factory, fullpath, fn))
                return true;
#    endif // defined(_MSC_VER) && !defined(NDEBUG)

            // Basic name
            fullpath = std::string(path).append("/" CMAKE_INTDIR "/").append(dll_name).append(ext);
            if (subopen(factory, fullpath, fn))
                return true;

#    if defined(_MSC_VER) && defined(NDEBUG)
            // MSVC RELEASE build: try debug name after basic name
            fullpath = std::string(path).append("/" CMAKE_INTDIR "/").append(dll_name).append("d").append(ext);
            if (subopen(factory, fullpath, fn))
                return true;
#    endif // defined(_MSC_VER) && defined(NDEBUG)

#endif // CMAKE_INTDIR
        }
    }
    if (!dll_name.empty() || !fn_name.empty()) {
        return open(factory, dll_name, fn_name);
    }
    return factory.open((std::string("yarp_") + name).c_str(),
                        (fn_name.empty()) ? name.c_str() : fn_name.c_str());
}

void YarpPluginSettings::reportStatus(SharedLibraryFactory& factory) const
{
    int problem = factory.getStatus();
    if (problem == 0) {
        return;
    }
    switch (problem) {
    case SharedLibraryFactory::STATUS_LIBRARY_NOT_LOADED:
        if (verbose) {
            fprintf(stderr, "Cannot load plugin from shared library (%s)\n", dll_name.c_str());
            fprintf(stderr, "(%s)\n", factory.getError().c_str());
        }
        break;
    case SharedLibraryFactory::STATUS_LIBRARY_NOT_FOUND:
        fprintf(stderr, "Cannot load plugin from shared library (%s)\n", dll_name.c_str());
        fprintf(stderr, "(%s)\n", factory.getError().c_str());
        break;
    case SharedLibraryFactory::STATUS_FACTORY_NOT_FOUND:
        fprintf(stderr, "cannot find YARP hook in shared library (%s:%s)\n", dll_name.c_str(), fn_name.c_str());
        fprintf(stderr, "(%s)\n", factory.getError().c_str());
        break;
    case SharedLibraryFactory::STATUS_FACTORY_NOT_FUNCTIONAL:
        fprintf(stderr, "YARP hook in shared library misbehaved (%s:%s)\n", dll_name.c_str(), fn_name.c_str());
        fprintf(stderr, "(the library may be too old/new and need to be recompiled to match YARP version)\n");
        fprintf(stderr, "(%s)\n", factory.getError().c_str());
        break;
    default:
        fprintf(stderr, "Unknown error (%s:%s)\n", dll_name.c_str(), fn_name.c_str());
        fprintf(stderr, "(%s)\n", factory.getError().c_str());
        break;
    }
}

void YarpPluginSettings::reportFailure() const
{
    fprintf(stderr, "Failed to create %s from shared library %s\n", fn_name.c_str(), dll_name.c_str());
}


void YarpPluginSelector::scan()
{
    // This method needs to be accessed by one thread only
    LockGuard guard(mutex);

    // If it was scanned in the last 5 seconds, there is no need to scan again
    bool need_scan = true;
    if (config.check("last_update_time")) {
        if (SystemClock::nowSystem() - config.find("last_update_time").asFloat64() < 5) {
            need_scan = false;
        }
    }
    if (!need_scan) {
        return;
    }

    YARP_SPRINTF0(Logger::get(),
                  debug,
                  "Scanning. I'm scanning. I hope you like scanning too.");

    // Search plugins directories
    ResourceFinder& rf = ResourceFinder::getResourceFinderSingleton();
    if (!rf.isConfigured()) {
        rf.configure(0, nullptr);
    }
    rf.setQuiet(true);
    Bottle plugin_paths = rf.findPaths("plugins");
    if (plugin_paths.size() == 0) {
        plugin_paths = rf.findPaths("share/yarp/plugins");
    }

    // Search .ini files in plugins directories
    config.clear();
    if (plugin_paths.size() > 0) {
        for (size_t i = 0; i < plugin_paths.size(); i++) {
            std::string target = plugin_paths.get(i).asString();
            YARP_SPRINTF1(Logger::get(),
                          debug,
                          "Loading configuration files related to plugins from %s.",
                          target.c_str());
            config.fromConfigDir(target, "inifile", false);
        }
    } else {
        YARP_SPRINTF0(Logger::get(),
                      debug,
                      "Plugin directory not found");
    }

    // Read the .ini files and populate the lists
    plugins.clear();
    search_path.clear();
    Bottle inilst = config.findGroup("inifile").tail();
    for (size_t i = 0; i < inilst.size(); i++) {
        std::string inifile = inilst.get(i).asString();
        Bottle inigroup = config.findGroup(inifile);
        Bottle lst = inigroup.findGroup("plugin").tail();
        for (size_t i = 0; i < lst.size(); i++) {
            std::string plugin_name = lst.get(i).asString();
            Bottle group = inigroup.findGroup(plugin_name);
            group.add(Value::makeValue(std::string("(inifile \"") + inifile + "\")"));
            if (select(group)) {
                plugins.addList() = group;
            }
        }
        lst = inigroup.findGroup("search").tail();
        for (size_t i = 0; i < lst.size(); i++) {
            std::string search_name = lst.get(i).asString();
            Bottle group = inigroup.findGroup(search_name);
            search_path.addList() = group;
        }
    }

    // Update last_update_time
    config.put("last_update_time", SystemClock::nowSystem());
}
