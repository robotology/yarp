/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_YARPPLUGINSELECTOR_H
#define YARP_OS_YARPPLUGINSELECTOR_H

#include <yarp/os/api.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Property.h>
#include <yarp/os/Mutex.h>
#include <yarp/os/LockGuard.h>

namespace yarp {
    namespace os {
        class YarpPluginSelector;
    }
}

/**
 *
 * Pick out a set of relevant plugins.  Override the select function
 * to define what "relevant" means.  The select function will be
 * called with [plugin] sections found in configuration files.
 *
 */
class YARP_OS_API yarp::os::YarpPluginSelector {
private:
    Bottle plugins;
    Bottle search_path;
    Property config;
    mutable yarp::os::Mutex mutex;
public:
    /**
     *
     * Destructor.
     *
     */
    virtual ~YarpPluginSelector() {}

    /**
     *
     * Determine whether a plugin is of interest.
     *
     * @return true if plugin is of interest.  By default, always returns true.
     *
     */
    virtual bool select(Searchable& options) { YARP_UNUSED(options); return true; }


    /**
     *
     * Find plugin configuration files, and run [plugin] sections
     * through the select method.
     *
     */
    void scan();

    /**
     *
     * @return a list of plugin sections that passed the select method
     * during the last call to scan.
     *
     */
    Bottle getSelectedPlugins() const {
        yarp::os::LockGuard guard(mutex);
        return plugins;
    }

    /**
     *
     * @return possible locations for plugin libraries found in [search]
     * sections.
     *
     */
    Bottle getSearchPath() const {
        yarp::os::LockGuard lock(mutex);
        return search_path;
    }
};

#endif // YARP_OS_YARPPLUGINSELECTOR_H
