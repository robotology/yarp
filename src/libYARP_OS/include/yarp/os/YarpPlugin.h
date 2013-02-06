// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_YARPPLUGIN_
#define _YARP2_YARPPLUGIN_

#include <yarp/os/YarpPluginSelector.h>
#include <yarp/os/YarpPluginSettings.h>

namespace yarp {
    namespace os {
        template <class T> class YarpPlugin;
    }
}

/**
 *
 * Type-safe access to a plugin.
 *
 */
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

    T *create() {
        if (!factory) return 0/*NULL*/;
        return factory->create();
    }

    void destroy(T *obj) {
        if (!factory) return;
        factory->destroy(obj);
    }
};

#endif

