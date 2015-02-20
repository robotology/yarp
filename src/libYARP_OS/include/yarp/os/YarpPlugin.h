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
    /**
     *
     * Constructor.
     *
     */
    YarpPlugin() {
        factory = 0/*NULL*/;
    }

    /**
     *
     * Destructor.
     *
     */
    virtual ~YarpPlugin() {
        close();
    }

    /**
     *
     * Load a library and prepare an object factory, based on the
     * hints supplied.
     *
     * @param settings the hints to use in finding the library
     *
     * @return true on success
     *
     */
    bool open(YarpPluginSettings& settings) {
        close();
        factory = new SharedLibraryClassFactory<T>();
        if (!factory) return false;
        if (!settings.open(*factory)) {
            settings.reportStatus(*factory);
            close();
            return false;
        }
        this->settings = settings;
        return true;
    }

    /**
     *
     * End this use of the plugin.
     *
     * @return true on success
     *
     */
    bool close() {
        if (!factory) return true;
        factory->removeRef();
        if (factory->getReferenceCount()<=0) {
            delete factory;
            factory = 0/*NULL*/;
        }
        return true;
    }

    /**
     *
     * @return true if the plugin is correctly loaded
     *
     */
    bool isValid() const {
        return (factory!=0/*NULL*/);
    }

    /**
     *
     * Create an object using the plugin.
     *
     * @return an object of the type the plugin creates (NULL on failure)
     *
     */
    T *create() {
        if (!factory) return 0/*NULL*/;
        return factory->create();
    }

    /**
     *
     * Destroy an object previously created using the plugin.
     *
     * @param obj the object to destroy
     *
     */
    void destroy(T *obj) {
        if (!factory) return;
        factory->destroy(obj);
    }

    /**
     *
     * @return the name of the objects constructed by this plugin
     *
     */
    ConstString getName() {
        if (!factory) return ConstString();
        return factory->getName();
    }

    /**
     *
     * @return the type of the objects constructed by this plugin
     *
     */
    ConstString getClassName() {
        if (!factory) return ConstString();
        return factory->getClassName();
    }

    /**
     *
     * @return the base class the objects constructed by this plugin
     *
     */
    ConstString getBaseClassName() {
        if (!factory) return ConstString();
        return factory->getBaseClassName();
    }

    /**
     *
     * @return the factory object associated with the plugin
     *
     */
    SharedLibraryClassFactory<T> *getFactory() const {
        return factory;
    }
};

#endif

