// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_YARPPLUGIN_
#define _YARP2_YARPPLUGIN_

#include <yarp/os/api.h>
#include <yarp/os/SharedLibraryClass.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Property.h>

namespace yarp {
    namespace os {
        class YarpPluginSelector;
        class YarpPluginSettings;
        class YarpPluginHelper;
        template <class T> class YarpPlugin;
    }
}

class YARP_OS_API yarp::os::YarpPluginSelector {
private:
    Bottle plugins;
    Bottle search_path;
    Property config;
public:
    virtual ~YarpPluginSelector() {}

    virtual bool select(Searchable& options) { return true; }

    void scan();

    Bottle getSelectedPlugins() const {
        return plugins;
    }

    Bottle getSearchPath() const {
        return search_path;
    }
};

class YARP_OS_API yarp::os::YarpPluginSettings {
private:
    bool subopen(SharedLibraryFactory& factory, const ConstString& dll_name,
                 const ConstString& fn_name);

    bool open(SharedLibraryFactory& factory, const ConstString& dll_name,
              const ConstString& fn_name);
public:
    ConstString name;
    ConstString dll_name;
    ConstString fn_name;
    ConstString fn_ext;
    YarpPluginSelector *selector;
    bool verbose;
    
    YarpPluginSettings() {
        verbose = false;
        selector = 0 /*NULL*/;
    }

    YarpPluginSettings(YarpPluginSelector& selector, 
                       Searchable& options,
                       const ConstString& xname) { 
        this->selector = &selector;
        readFromSearchable(options,xname);
    }

    bool readFromSelector(const char *name) { 
        if (!selector) return false;
        Bottle plugins = selector->getSelectedPlugins();
        Bottle group = plugins.findGroup(name).tail();
        readFromSearchable(group,name);
        return true;
    }

    bool readFromSearchable(Searchable& options, const char *name) { 
        ConstString iname = options.find("library").toString().c_str();
        ConstString pname = options.find("part").toString().c_str();
        if (iname=="") iname = name;
        if (pname=="") pname = name;
        this->name = iname.c_str();
        this->dll_name = iname;
        this->fn_name = pname;
        verbose = false;
        return true;
    }

    bool open(SharedLibraryFactory& factory);
    void reportStatus(SharedLibraryFactory& factory) const;
    void reportFailure() const;
};

template <class T>
class yarp::os::YarpPlugin {
private:
    SharedLibraryClassFactory<T> *factory;
    SharedLibraryClass<T> content;
    YarpPluginSettings settings;

public:
    YarpPlugin() {
        factory = 0/*NULL*/;
    }

    bool open(YarpPluginSettings& settings) {
        close();
        factory = new SharedLibraryClassFactory<T>();
        if (!factory) return false;
        factory->addRef();
        if (!settings.open(*factory)) {
            settings.reportStatus(*factory);
            close();
            return false;
        }
        this->settings = settings;
        return true;
    }

    bool initialize(SharedLibraryClass<T>& content) {
        if (!factory) return false;
        if (!content.open(*factory)) {
            settings.reportFailure();
        }
        return true;
    }

    bool close() {
        if (!factory) return true;
        factory->removeRef();
        if (factory->getReferenceCount()<=0) {
            delete factory;
            factory = 0/*NULL*/;
        }
        return true;
    }

    virtual ~YarpPlugin() {
        close();
    }

    bool isValid() {
        return (factory!=0/*NULL*/);
    }

    SharedLibraryClassFactory<T> *getFactory() { 
        return factory;
    }
};

#endif

