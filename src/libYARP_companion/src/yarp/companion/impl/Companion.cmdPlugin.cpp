/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Property.h>
#include <yarp/os/SharedLibraryClassApi.h>
#include <yarp/os/SharedLibraryFactory.h>
#include <yarp/os/Value.h>
#include <yarp/os/YarpPluginSettings.h>
#include <yarp/os/YarpPluginSelector.h>
#include <yarp/os/impl/PlatformSignal.h>

using yarp::companion::impl::Companion;

using yarp::os::Bottle;
using yarp::os::Property;
using yarp::os::SharedLibraryClassApi;
using yarp::os::SharedLibraryFactory;
using yarp::os::Value;
using yarp::os::YarpPluginSettings;
using yarp::os::YarpPluginSelector;


namespace {

void plugin_signal_handler(int)
{
   // prevent infinite recursion if say_hi() causes another segfault
    yarp::os::impl::signal(SIGSEGV, SIG_DFL);
    yarp::os::impl::signal(SIGABRT, SIG_DFL);
    throw std::exception();
}

bool plugin_test(YarpPluginSettings& settings)
{
    SharedLibraryFactory lib;
    settings.open(lib);
    if (!lib.isValid()) {
        yCError(COMPANION, "    Cannot find or load shared library");
        return false;
    } else {
        const SharedLibraryClassApi& api = lib.getApi();
        char className[256] = "unknown";
        api.getClassName(className, sizeof(className));
        char baseClassName[256] = "unknown";
        api.getBaseClassName(baseClassName, sizeof(baseClassName));
        yCInfo(COMPANION, "  * library:        %s", lib.getName().c_str());
        yCInfo(COMPANION, "  * system version: %d", (int)api.systemVersion);
        yCInfo(COMPANION, "  * class name:     %s", className);
        yCInfo(COMPANION, "  * base class:     %s", baseClassName);

        bool ok = true;
        yarp::os::impl::signal(SIGSEGV, plugin_signal_handler);
        yarp::os::impl::signal(SIGABRT, plugin_signal_handler);
        try {
            void* tmp = api.create();
            api.destroy(tmp);
        } catch (...) {
            yCWarning(COMPANION);
            yCWarning(COMPANION, "  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
            yCWarning(COMPANION, "  X                                                     X");
            yCWarning(COMPANION, "  X                       WARNING                       X");
            yCWarning(COMPANION, "  X                                                     X");
            yCWarning(COMPANION, "  X            === This plugin is BROKEN ===            X");
            yCWarning(COMPANION, "  X                                                     X");
            yCWarning(COMPANION, "  X                                                     X");
            yCWarning(COMPANION, "  X  Author information: The most plausible reason is   X");
            yCWarning(COMPANION, "  X  that the destructor is deleting some pointer that  X");
            yCWarning(COMPANION, "  X  is not allocated in the constructor, and that is   X");
            yCWarning(COMPANION, "  X  not initialized to a null pointer.                 X");
            yCWarning(COMPANION, "  X                                                     X");
            yCWarning(COMPANION, "  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
            ok = false;
        }
        yarp::os::impl::signal(SIGSEGV, SIG_DFL);
        yarp::os::impl::signal(SIGABRT, SIG_DFL);
        return ok;
    }
}

void plugin_usage()
{
    yCInfo(COMPANION, "Print information about installed plugins");
    yCInfo(COMPANION);
    yCInfo(COMPANION, "Usage:");
#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
    yCInfo(COMPANION, " * Test a specific plugin:");
    yCInfo(COMPANION, "     yarp plugin [--verbose] <pluginname>");
    yCInfo(COMPANION, "     yarp plugin [--verbose] /path/to/plugin/<libraryname>.(so|dll|dylib) <pluginpart>");
    yCInfo(COMPANION, " * Test all the plugins:");
    yCInfo(COMPANION, "     yarp plugin [--verbose] --all");
    yCInfo(COMPANION, " * Print a list of plugins:");
    yCInfo(COMPANION, "     yarp plugin [--verbose] --list");
    yCInfo(COMPANION, " * Print plugin search path:");
    yCInfo(COMPANION, "     yarp plugin [--verbose] --search-path");
#else
    yCInfo(COMPANION, " * Test a specific plugin:");
    yCInfo(COMPANION, "     yarp plugin <pluginname>");
    yCInfo(COMPANION, "     yarp plugin /path/to/plugin/<libraryname>.(so|dll|dylib) <pluginpart>");
    yCInfo(COMPANION, " * Test all the plugins:");
    yCInfo(COMPANION, "     yarp plugin --all");
    yCInfo(COMPANION, " * Print a list of plugins:");
    yCInfo(COMPANION, "     yarp plugin --list");
    yCInfo(COMPANION, " * Print plugin search path:");
    yCInfo(COMPANION, "     yarp plugin --search-path");
#endif // YARP_NO_DEPRECATED
    yCInfo(COMPANION, " * Print this help and exit:");
    yCInfo(COMPANION, "     yarp plugin --help");
    yCInfo(COMPANION);
}

} // namespace

int Companion::cmdPlugin(int argc, char *argv[])
{
    if (argc<1) {
        plugin_usage();
        return 1;
    }

    std::string arg = argv[0];
    if (arg=="--help") {
        plugin_usage();
        return 0;
    }

#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
    bool verbose = false;
    if (arg=="--verbose") {
        yCWarning(COMPANION, "The verbose argument is deprecated.");
        argc--;
        argv++;
        arg = argv[0];
    }
#endif // YARP_NO_DEPRECATED

    YarpPluginSelector selector;
    selector.scan();

    if (arg=="--search-path") {
        Bottle lst = selector.getSearchPath();
        if (lst.size()==0) {
            yCInfo(COMPANION, "No search path.");
            return 1;
        }
        yCInfo(COMPANION, "Search path:");
        for (size_t i=0; i<lst.size(); i++) {
            Value& options = lst.get(i);
            std::string name = options.asList()->get(0).toString();
            std::string path = options.check("path", Value("unknown path")).asString();
            std::string type = options.check("type", Value("unknown type")).asString();
            if (type == "shared") {
                yCInfo(COMPANION, "  %15s:\t%s", name.c_str(), path.c_str());
            }
        }
        return 0;
    }

    if (arg=="--list") {
        Bottle lst = selector.getSelectedPlugins();
        for (size_t i=0; i<lst.size(); i++) {
            Value& options = lst.get(i);
            std::string name = options.check("name", Value("untitled")).asString();
            yCInfo(COMPANION, "%s", name.c_str());
        }
        return 0;
    }
    if (arg=="--all") {
        Bottle lst = selector.getSelectedPlugins();
        if (lst.size()==0) {
            yCInfo(COMPANION, "No plugins found.");
            return 1;
        }
        yCInfo(COMPANION, "Runtime plugins found:");
        bool ok = true;
        for (size_t i=0; i<lst.size(); i++) {
            Value& options = lst.get(i);
            std::string name = options.check("name", Value("untitled")).asString();
            std::string type = options.check("type", Value("unknown type")).asString();
            yCInfo(COMPANION);
            yCInfo(COMPANION, "%s %s", type.c_str(), name.c_str());
            yCInfo(COMPANION, "  * ini file:       %s", options.find("inifile").toString().c_str());
            options.asList()->pop();
            yCInfo(COMPANION, "  * config:         %s", options.toString().c_str());
            YarpPluginSettings settings;
#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
            settings.setVerboseMode(verbose);
YARP_WARNING_POP
#endif // YARP_NO_DEPRECATED
            settings.setSelector(selector);
            settings.readFromSearchable(options, name);
            ok &= plugin_test(settings);
        }
        return ok ? 0 : 1;
    } else {
        Property p;
        YarpPluginSettings settings;
#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
        settings.setVerboseMode(verbose);
YARP_WARNING_POP
#endif
        if (argc>=2) {
            settings.setLibraryMethodName(argv[0], argv[1]);
        } else {
            settings.setPluginName(argv[0]);
        }
        if (!settings.setSelector(selector)) {
            yCError(COMPANION, "cannot find a plugin with the specified name");
        } else {
            yCInfo(COMPANION, "Yes, this is a YARP plugin");
        }
        return plugin_test(settings) ? 0 : 1;
    }
}
