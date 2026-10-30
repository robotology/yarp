/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_MANAGER_PARAMRESOLVER_H
#define YARP_MANAGER_PARAMRESOLVER_H

#include <map>
#include <string>

namespace yarp::manager {

/**
 * Application parameters handling.
 *
 * An application can contain placeholders written as $PAR{name} or
 * $PAR{name=default}. An application importing it can override their values
 * with <app-param name="name">value</app-param> in its <application> block.
 * Placeholders are kept verbatim while loading the xml files and are replaced
 * once per application instance, when the application is put together.
 */
class ParamResolver
{
public:
    typedef std::map<std::string, std::string> ParamMap;

    struct Declaration
    {
        bool hasDefault {false};
        std::string defaultValue;
    };
    typedef std::map<std::string, Declaration> DeclMap;

    static const std::string startKeyword;
    static const std::string endKeyword;

    /**
     * Adds to decls the parameters used in text.
     * Fails on a malformed placeholder or if a parameter gets two different
     * default values.
     */
    static bool collect(const std::string& text, DeclMap& decls, std::string& error);

    /**
     * Computes the value of each declared parameter: the override if present,
     * otherwise the default. Parameters with neither are left out of values.
     * Fails if overrides contains a parameter that is not declared.
     */
    static bool merge(const DeclMap& decls, const ParamMap& overrides, ParamMap& values, std::string& error);

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
        Declaration decl;
    };

    /**
     * Finds the first placeholder in text starting from pos.
     * Returns false if there is none; sets error if it is malformed.
     */
    static bool findNext(const std::string& text, size_t pos, Placeholder& placeholder, std::string& error);
};

} // namespace yarp::manager

#endif // YARP_MANAGER_PARAMRESOLVER_H
