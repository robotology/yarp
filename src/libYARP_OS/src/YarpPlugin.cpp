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
#include <yarp/os/Property.h>

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
        fprintf(stderr,"Trying to find: [lib]%s.so/dll/... containing function '%s' -- %s\n", dll_name.c_str(), fn_name.c_str(), ok?"found":"not found");
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
    if (dll_name!=""||fn_name!="") {
        return open(factory,dll_name, fn_name);
    }
#ifdef YARP_HAS_ACE
    bool ok = false;
    ConstString str(name);
    ConstString libName = name;
    ConstString fnName = name;
    int sindex = str.find(":");
    if (sindex>=0) {
        libName = str.substr(0,sindex);
        fnName = str.substr(sindex+1);
        ok = open(factory,libName,fnName);
    }
    if ((!ok) && sindex<0) {
        libName = ConstString("yarp_") + name;
        fnName = name;
        ok = open(factory,libName,fnName);
    }
    if ((!ok) && sindex<0) {
        int index = str.find("_");
        if (index>=0) {
            libName = ConstString("yarp_") + str.substr(0,index);
            fnName = str;
            ok = open(factory,libName,fnName);
        }
    }
    return ok;
#else
    return factory.open(dll_name.c_str(), fn_name.c_str());
#endif
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


Bottle YarpPluginSelector::listPlugins() {
    Bottle result;
    ConstString suffix = "/etc/yarp/plugins";
    ConstString dirname = suffix;
    ACE_DIR *dir = ACE_OS::opendir(dirname.c_str());
    if (!dir) {
        YARP_SPRINTF1(Logger::get(),debug,"Could not find %s", dirname.c_str());

#ifdef YARP_HAS_ACE
        char buf[4192] = "";
        // as a placeholder, we use the path to libYARP_OS.so as a 
        // reference.  Requires LD_LIBRARY_PATH or equivalent to be
        // set.  This will be evaporating.
        ACE::ldfind("YARP_OS",buf,sizeof(buf));
        printf("FOUND %s\n", buf);
        if (buf[0]!='\0') {
            String s(buf);
            YARP_STRING_INDEX n = s.rfind('/');
            if (n == String::npos) {
                n = s.rfind('\\');
            }
            if (n != String::npos) {
                s[n] = '\0';
            }
            ConstString prefix(s.c_str());
            dirname = prefix + "/" + suffix;
            dir = ACE_OS::opendir(dirname.c_str());
            if (!dir) {
                dirname = prefix + "/.." + suffix;
                dir = ACE_OS::opendir(dirname.c_str());
                if (!dir) {
                    dirname = prefix + "/../.." + suffix;
                    dir = ACE_OS::opendir(dirname.c_str());
                }
            }
            if (!dir) {
                YARP_SPRINTF2(Logger::get(),debug,"Could not find %s/.../%s", buf, dirname.c_str());
                return result;
            }
        } else {
            return result;
        }
#else
        return result;
#endif
    }
    struct YARP_DIRENT *ent = YARP_readdir(dir);
    while (ent) {
        ConstString name = ent->d_name;
        ent = ACE_OS::readdir(dir);
        int len = (int)name.length();
        if (len<4) continue;
        if (name.substr(len-4)!=".ini") continue;
        ConstString fname = dirname + "/" + name;
        Property config;
        config.fromConfigFile(fname);
        Bottle plugins = config.findGroup("plugin").tail();
        for (int i=0; i<plugins.size(); i++) {
            ConstString plugin_name = plugins.get(i).asString();
            Bottle group = config.findGroup(plugin_name);
            if (select(group)) {
                result.addList() = group;
            }
        }
    }
    YARP_closedir(dir);
    dir = NULL;
    return result;
}

