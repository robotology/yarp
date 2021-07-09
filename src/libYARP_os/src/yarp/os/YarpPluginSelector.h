/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_YARPPLUGINSELECTOR_H
#define YARP_OS_YARPPLUGINSELECTOR_H

#include <yarp/os/api.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Property.h>

#include <mutex>

namespace yarp {
namespace os {

/**
 *
 * Pick out a set of relevant plugins.  Override the select function
 * to define what "relevant" means.  The select function will be
 * called with [plugin] sections found in configuration files.
 *
 */
class YARP_os_API YarpPluginSelector
{
private:
    Bottle plugins;
    Bottle search_path;
    Property config;
    mutable std::mutex mutex;

public:
    /**
     * Destructor.
     */
    virtual ~YarpPluginSelector() = default;

    /**
     * Determine whether a plugin is of interest.
     *
     * @return true if plugin is of interest.  By default, always returns true.
     */
    virtual bool select(Searchable& options)
    {
        YARP_UNUSED(options);
        return true;
    }


    /**
     * Find plugin configuration files, and run [plugin] sections
     * through the select method.
     */
    void scan();

    /**
     * @return a list of plugin sections that passed the select method
     * during the last call to scan.
     */
    Bottle getSelectedPlugins() const
    {
        std::lock_guard<std::mutex> guard(mutex);
        return plugins;
    }

    /**
     * @return possible locations for plugin libraries found in [search]
     * sections.
     */
    Bottle getSearchPath() const
    {
        std::lock_guard<std::mutex> guard(mutex);
        return search_path;
    }

    /**
     * Checks if a pluigin of the given type is available.
     */
    static bool checkPlugin(const std::string& name, const std::string& type = {})
    {
        yarp::os::YarpPluginSelector selector;
        selector.scan();
        const yarp::os::Bottle lst = selector.getSelectedPlugins();
        for (size_t i = 0; i < lst.size(); i++) {
            const yarp::os::Value& options = lst.get(i);
            if (name == options.check("name", yarp::os::Value("untitled")).asString()) {
                if (!type.empty()) {
                    return true;
                }
                if (type == options.check("type", yarp::os::Value("untitled")).asString()) {
                    return true;
                }
            }
        }
        return false;
    }
};

} // namespace os
} // namespace yarp

#endif // YARP_OS_YARPPLUGINSELECTOR_H
