/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/YarpPlugin.h>

#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/impl/LogComponent.h>
#include <yarp/os/impl/NameClient.h>

#include <cstdio>
#include <cstdlib>

using namespace yarp::os;
using namespace yarp::os::impl;

namespace {
#ifndef YARP_NO_DEPRECATED // since YARP 3.4
// The log component cannot be const because we still support setVerboseMode
YARP_OS_NON_CONST_LOG_COMPONENT(YARPPLUGINSETTINGS, "yarp.os.YarpPluginSettings")
#else
YARP_OS_LOG_COMPONENT(YARPPLUGINSETTINGS, "yarp.os.YarpPluginSettings")
#endif
} // namespace

#ifndef YARP_NO_DEPRECATED // since YARP 3.4
void YarpPluginSettings::setVerboseMode(bool verbose)
{
    YARPPLUGINSETTINGS().setMinimumPrintLevel(verbose ? yarp::os::Log::DebugType : yarp::os::Log::InfoType);
}
#endif // YARP_NO_DEPRECATED

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
    yCDebug(YARPPLUGINSETTINGS,
            "Trying plugin [dll: %s] [fn: %s]",
            dll_name.c_str(),
            fn_name.c_str());
    bool ok = factory.open(dll_name.c_str(), fn_name.c_str());
    yCDebug(YARPPLUGINSETTINGS,
            "Trying to find library '%s' containing function '%s' -- %s",
            dll_name.c_str(),
            fn_name.c_str(), ok ? "found" : "fail");
    if (ok) {
        yCDebug(YARPPLUGINSETTINGS,
                "Found plugin [dll: %s] [fn: %s]",
                dll_name.c_str(),
                fn_name.c_str());
        this->dll_name = dll_name;
        this->fn_name = fn_name;
    } else {
        if (factory.getStatus() != SharedLibraryFactory::STATUS_LIBRARY_NOT_FOUND) {
            yCError(YARPPLUGINSETTINGS,
                    "Error while opening %s:\n  %s",
                    dll_name.c_str(),
                    factory.getError().c_str());
        } else {
            yCDebug(YARPPLUGINSETTINGS,
                    "Error while opening %s:\n  %s",
                    dll_name.c_str(),
                    factory.getError().c_str());
        }
    }
    return ok;
}

bool YarpPluginSettings::open(SharedLibraryFactory& factory)
{
    yCDebug(YARPPLUGINSETTINGS,
            "Plugin [name: %s] [dll: %s] [fn: %s]",
            name.c_str(),
            dll_name.c_str(),
            fn_name.c_str());
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
        yCDebug(YARPPLUGINSETTINGS, "Cannot load plugin from shared library (%s)", dll_name.c_str());
        yCDebug(YARPPLUGINSETTINGS, "(%s)", factory.getError().c_str());
        break;
    case SharedLibraryFactory::STATUS_LIBRARY_NOT_FOUND:
        yCWarning(YARPPLUGINSETTINGS, "Cannot load plugin from shared library (%s)", dll_name.c_str());
        yCWarning(YARPPLUGINSETTINGS, "(%s)", factory.getError().c_str());
        break;
    case SharedLibraryFactory::STATUS_FACTORY_NOT_FOUND:
        yCWarning(YARPPLUGINSETTINGS, "Cannot find YARP hook in shared library (%s:%s)", dll_name.c_str(), fn_name.c_str());
        yCWarning(YARPPLUGINSETTINGS, "(%s)", factory.getError().c_str());
        break;
    case SharedLibraryFactory::STATUS_FACTORY_NOT_FUNCTIONAL:
        yCWarning(YARPPLUGINSETTINGS, "YARP hook in shared library misbehaved (%s:%s)", dll_name.c_str(), fn_name.c_str());
        yCWarning(YARPPLUGINSETTINGS, "(the library may be too old/new and need to be recompiled to match YARP version)");
        yCWarning(YARPPLUGINSETTINGS, "(%s)", factory.getError().c_str());
        break;
    default:
        yCWarning(YARPPLUGINSETTINGS, "Unknown error (%s:%s)", dll_name.c_str(), fn_name.c_str());
        yCWarning(YARPPLUGINSETTINGS, "(%s)", factory.getError().c_str());
        break;
    }
}

void YarpPluginSettings::reportFailure() const
{
    yCError(YARPPLUGINSETTINGS, "Failed to create %s from shared library %s", fn_name.c_str(), dll_name.c_str());
}

bool YarpPluginSettings::readFromSelector(const std::string& name)
{
    if (!selector)
        return false;
    Bottle plugins = selector->getSelectedPlugins();
    Bottle group = plugins.findGroup(name).tail();
    if (group.isNull()) {
        yCError(YARPPLUGINSETTINGS,
                "Cannot find \"%s\" plugin (not built in, and no .ini file found for it)"
                "Check that YARP_DATA_DIRS leads to at least one directory with plugins/%s.ini "
                "or share/yarp/plugins/%s.ini in it",
                name.c_str(),
                name.c_str(),
                name.c_str());
        return false;
    }
    return readFromSearchable(group, name);
}

void YarpPluginSelector::scan()
{
    // This method needs to be accessed by one thread only
    std::lock_guard<std::mutex> guard(mutex);

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

    yCDebug(YARPPLUGINSETTINGS, "Scanning. I'm scanning. I hope you like scanning too.");

    // Search plugins directories
    ResourceFinder& rf = ResourceFinder::getResourceFinderSingleton();
    static std::mutex rf_mutex;
    std::lock_guard<std::mutex> rf_guard(rf_mutex);

    if (!rf.isConfigured()) {
        rf.configure(0, nullptr);
    }
    Bottle plugin_paths = rf.findPaths("plugins");
    if (plugin_paths.size() == 0) {
        plugin_paths = rf.findPaths("share/yarp/plugins");
    }

    // Search .ini files in plugins directories
    config.clear();
    if (plugin_paths.size() > 0) {
        for (size_t i = 0; i < plugin_paths.size(); i++) {
            std::string target = plugin_paths.get(i).asString();
            yCDebug(YARPPLUGINSETTINGS, "Loading configuration files related to plugins from %s.",
                       target.c_str());
            config.fromConfigDir(target, "inifile", false);
        }
    } else {
        yCDebug(YARPPLUGINSETTINGS, "Plugin directory not found");
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
