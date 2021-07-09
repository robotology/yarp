/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_YARPPLUGIN_H
#define YARP_OS_YARPPLUGIN_H

#include <yarp/os/YarpPluginSelector.h>
#include <yarp/os/YarpPluginSettings.h>

namespace yarp {
namespace os {

/**
 *
 * Type-safe access to a plugin.
 *
 */
template <class T>
class YarpPlugin
{
private:
    SharedLibraryClassFactory<T>* factory;
    SharedLibraryClass<T> content;
    YarpPluginSettings settings;

public:
    /**
     *
     * Constructor.
     *
     */
    YarpPlugin()
    {
        factory = nullptr;
    }

    /**
     *
     * Destructor.
     *
     */
    virtual ~YarpPlugin()
    {
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
    bool open(YarpPluginSettings& settings)
    {
        close();
        factory = new SharedLibraryClassFactory<T>();
        if (!factory)
            return false;
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
    bool close()
    {
        if (!factory) {
            return true;
        }
        factory->removeRef();
        if (factory->getReferenceCount() <= 0) {
            delete factory;
            factory = nullptr;
        }
        return true;
    }

    /**
     *
     * @return true if the plugin is correctly loaded
     *
     */
    bool isValid() const
    {
        return (factory != nullptr);
    }

    /**
     *
     * Create an object using the plugin.
     *
     * @return an object of the type the plugin creates (nullptr on
     *         failure)
     *
     */
    T* create()
    {
        if (!factory) {
            return nullptr;
        }
        return factory->create();
    }

    /**
     *
     * Destroy an object previously created using the plugin.
     *
     * @param obj the object to destroy
     *
     */
    void destroy(T* obj)
    {
        if (!factory) {
            return;
        }
        factory->destroy(obj);
    }

    /**
     *
     * @return the name of the objects constructed by this plugin
     *
     */
    std::string getName()
    {
        if (!factory) {
            return {};
        }
        return factory->getName();
    }

    /**
     *
     * @return the type of the objects constructed by this plugin
     *
     */
    std::string getClassName()
    {
        if (!factory) {
            return {};
        }
        return factory->getClassName();
    }

    /**
     *
     * @return the base class the objects constructed by this plugin
     *
     */
    std::string getBaseClassName()
    {
        if (!factory) {
            return {};
        }
        return factory->getBaseClassName();
    }

    /**
     *
     * @return the factory object associated with the plugin
     *
     */
    SharedLibraryClassFactory<T>* getFactory() const
    {
        return factory;
    }
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_YARPPLUGIN_H
