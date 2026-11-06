/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_MANAGER_PARAMRESOLVER_H
#define YARP_MANAGER_PARAMRESOLVER_H

#include <map>
#include <set>
#include <string>

namespace yarp::manager {

/**
 * Application parameters handling.
 *
 * An application can contain placeholders written as $PAR{name}. Their
 * default values are declared once in the application with
 * <def-param-value name="name">default</def-param-value>, and an application
 * importing it can override them with <app-param name="name">value</app-param>
 * in its <application> block.
 * Placeholders are kept verbatim while loading the xml files and are replaced
 * once per application instance, when the application is put together.
 */
class ParamResolver
{
public:
    typedef std::map<std::string, std::string> ParamMap;

    typedef std::set<std::string> NameSet;

    static const std::string startKeyword;
    static const std::string endKeyword;

    /**
     * Adds to used the parameters used in text.
     * Fails on a malformed placeholder.
     */
    static bool collect(const std::string& text, NameSet& used, std::string& error);

    /**
     * Computes the value of each parameter: the override if present,
     * otherwise the default. Parameters with neither are left out of values.
     * Fails if overrides contains a parameter that is neither used nor has
     * a default value.
     */
    static bool merge(const NameSet& used,
                      const ParamMap& defaults,
                      const ParamMap& overrides,
                      ParamMap& values,
                      std::string& error);

    /**
     * Replaces each placeholder in text with its value. Replaced values are
     * not parsed again.
     * Fails on a malformed placeholder or if a parameter has no value.
     */
    static bool resolve(const std::string& text, const ParamMap& values, std::string& out, std::string& error);

private:
    struct Placeholder
    {
        size_t begin {0};
        size_t end {0};
        std::string name;
    };

    /**
     * Finds the first placeholder in text starting from pos.
     * Returns false if there is none; sets error if it is malformed.
     */
    static bool findNext(const std::string& text, size_t pos, Placeholder& placeholder, std::string& error);
};

} // namespace yarp::manager

#endif // YARP_MANAGER_PARAMRESOLVER_H
