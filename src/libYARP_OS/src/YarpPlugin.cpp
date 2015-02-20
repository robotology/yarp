// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>

#include <yarp/os/YarpPlugin.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/PlatformStdlib.h>
#include <yarp/os/impl/String.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>

using namespace yarp::os;
using namespace yarp::os::impl;

bool YarpPluginSettings::open(SharedLibraryFactory& factory,
                              const ConstString& dll_name,
                              const ConstString& fn_name) {
    bool ok = subopen(factory,dll_name,fn_name);
    if (!ok) {
        if (factory.getStatus() ==
            SharedLibraryFactory::STATUS_FACTORY_NOT_FOUND) {
            if (fn_ext!="") {
                ok = subopen(factory,dll_name,fn_name + fn_ext);
            } else {
                ok = subopen(factory,dll_name,fn_name + "_carrier");
                if (!ok) {
                    ok = subopen(factory,dll_name,fn_name + "_device");
                }
            }
        }
    }
    return ok;
}

bool YarpPluginSettings::subopen(SharedLibraryFactory& factory,
                                 const ConstString& dll_name,
                                 const ConstString& fn_name) {
    YARP_SPRINTF2(impl::Logger::get(),debug,"Trying plugin [dll: %s] [fn: %s]",
                  dll_name.c_str(),fn_name.c_str());
    bool ok = factory.open(dll_name.c_str(),fn_name.c_str());
    if (verbose) {
        fprintf(stderr,"Trying to find library '%s' containing function '%s' -- %s\n", dll_name.c_str(), fn_name.c_str(), ok?"found":"not found");
    }
    if (ok) {
        YARP_SPRINTF2(impl::Logger::get(),debug,
                      "Found plugin [dll: %s] [fn: %s]",
                      dll_name.c_str(),fn_name.c_str());
        this->dll_name = dll_name;
        this->fn_name = fn_name;
    }
    return ok;
}

bool YarpPluginSettings::open(SharedLibraryFactory& factory) {
    YARP_SPRINTF3(impl::Logger::get(),debug,"Plugin [name: %s] [dll: %s] [fn: %s]",
                  name.c_str(),dll_name.c_str(),fn_name.c_str());
    if (selector!=NULL && name != "") {
        // we may have a YARP-specific search path available,
        // and a proper name for the DLL
        Bottle paths = selector->getSearchPath();
        for (int i=0; i<paths.size(); i++) {
            Searchable& options = paths.get(i);
            ConstString path = options.find("path").asString();
            ConstString ext = options.find("extension").asString();
            ConstString basename = (dll_name.find(".")!=basename.npos) ? name : dll_name;
            ConstString fn = (fn_name=="")?name:fn_name;

            // Basic name
            if (subopen(factory, path + "/" + basename + ext, fn))
                return true;

            // Debug name
            if (subopen(factory, path + "/" + basename + "d" + ext, fn))
                return true;

#ifdef CMAKE_INTDIR
            // On multi-config system, try to find the plugin in the
            // current config subdirectory

            // Basic name
            if (subopen(factory, path + "/" +  CMAKE_INTDIR + "/" + dll_name + ext, fn))
                return true;

            // Debug name
            if (subopen(factory, path + "/" +  CMAKE_INTDIR + "/" + dll_name + "d" + ext, fn))
                return true;
#endif
        }
    }
    if (dll_name!=""||fn_name!="") {
        return open(factory, dll_name, fn_name);
    }
    return factory.open((ConstString("yarp_") + name).c_str(),
                        (fn_name=="")?name.c_str():fn_name.c_str());
}

void YarpPluginSettings::reportStatus(SharedLibraryFactory& factory) const {
    int problem = factory.getStatus();
    if (problem==0) return;
    switch (problem) {
    case SharedLibraryFactory::STATUS_LIBRARY_NOT_LOADED:
        if (verbose) {
            fprintf(stderr,"Cannot load plugin from shared library (%s)\n", dll_name.c_str());
        }
        break;
    case SharedLibraryFactory::STATUS_FACTORY_NOT_FOUND:
        fprintf(stderr,"cannot find YARP hook in shared library (%s:%s)\n", dll_name.c_str(), fn_name.c_str());
        break;
    case SharedLibraryFactory::STATUS_FACTORY_NOT_FUNCTIONAL:
        fprintf(stderr,"YARP hook in shared library misbehaved (%s:%s)\n", dll_name.c_str(), fn_name.c_str());
        fprintf(stderr,"(the library may be too old/new and need to be recompiled to match YARP version)\n");
        break;
    default:
        fprintf(stderr,"Unknown error (%s:%s)\n", dll_name.c_str(), fn_name.c_str());
        break;
    }
}

void YarpPluginSettings::reportFailure() const {
    fprintf(stderr,"Failed to create %s from shared library %s\n",
            fn_name.c_str(), dll_name.c_str());
}


void YarpPluginSelector::scan() {
    config.clear();
    plugins.clear();
    search_path.clear();
    NetworkBase::lock();
    Property& state = NameClient::getNameClient().getPluginState();
    config = state;
    NetworkBase::unlock();
    bool need_scan = true;
    if (config.check("last_update_time")) {
        if (Time::now()-config.find("last_update_time").asDouble()<5) {
            need_scan = false;
        }
    }
    if (need_scan) {
        YARP_SPRINTF0(Logger::get(),
                      debug,
                      "Scanning. I'm scanning. I hope you like scanning too.");
        NetworkBase::lock();
        ResourceFinder& rf = ResourceFinder::getResourceFinderSingleton();
        if (!rf.isConfigured()) {
            rf.configure(0,NULL);
        }
        rf.setQuiet(true);
        Bottle plugins = rf.findPaths("plugins");
        if (plugins.size()==0) {
            plugins = rf.findPaths("share/yarp/plugins");
        }
        if (plugins.size()>0) {
            for (int i=0; i<plugins.size(); i++) {
                ConstString target = plugins.get(i).asString();
                YARP_SPRINTF1(Logger::get(),
                              debug,
                              "Loading configuration files related to plugins from %s.", target.c_str());
                config.fromConfigFile(target,false);
            }
        } else {
            YARP_SPRINTF0(Logger::get(),
                          debug,
                          "Plugin directory not found");
        }
        config.put("last_update_time",Time::now());
        state = config;
        NetworkBase::unlock();
    }
    Bottle lst = config.findGroup("plugin").tail();
    for (int i=0; i<lst.size(); i++) {
        ConstString plugin_name = lst.get(i).asString();
        Bottle group = config.findGroup(plugin_name);
        if (select(group)) {
            plugins.addList() = group;
        }
    }
    lst = config.findGroup("search").tail();
    for (int i=0; i<lst.size(); i++) {
        ConstString search_name = lst.get(i).asString();
        Bottle group = config.findGroup(search_name);
        search_path.addList() = group;
    }
}
