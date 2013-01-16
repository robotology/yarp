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

namespace yarp {
    namespace os {
        class YarpPluginSelector;
        class YarpPluginSettings;
        class YarpPluginHelper;
        template <class T> class YarpPlugin;
    }
}

class YARP_OS_API yarp::os::YarpPluginSelector {
public:
    virtual ~YarpPluginSelector() {}
    virtual bool select(Searchable& options) { return true; }

    Bottle listPlugins();
};

class YARP_OS_API yarp::os::YarpPluginSettings {
private:
    bool subopen(SharedLibraryFactory& factory, const ConstString& dll_name,
                 const ConstString& fn_name);

    bool open(SharedLibraryFactory& factory, const ConstString& dll_name,
              const ConstString& fn_name);
public:
    yarp::os::ConstString name;
    yarp::os::ConstString dll_name;
    yarp::os::ConstString fn_name;
    yarp::os::ConstString fn_ext;
    bool verbose;
    
    YarpPluginSettings() {
        verbose = false;
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

