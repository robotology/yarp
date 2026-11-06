/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/manager/impl/paramresolver.h>

using namespace yarp::manager;

const std::string ParamResolver::startKeyword = "$PAR{";
const std::string ParamResolver::endKeyword = "}";

bool ParamResolver::findNext(const std::string& text, size_t pos, Placeholder& placeholder, std::string& error)
{
    error.clear();
    size_t s = text.find(startKeyword, pos);
    if (s == std::string::npos) {
        return false;
    }

    size_t e = text.find(endKeyword, s + startKeyword.size());
    if (e == std::string::npos) {
        error = "unterminated '" + startKeyword + "' in \"" + text + "\"";
        return false;
    }

    std::string content = text.substr(s + startKeyword.size(), e - s - startKeyword.size());
    if (content.find(startKeyword) != std::string::npos) {
        error = "nested '" + startKeyword + "' in \"" + text + "\"";
        return false;
    }
    if (content.empty()) {
        error = "parameter with empty name in \"" + text + "\"";
        return false;
    }
    size_t eq = content.find('=');
    if (eq != std::string::npos) {
        std::string name = content.substr(0, eq);
        error = "invalid parameter name '" + content + "' in \"" + text + "\": default values are declared with "
                + "<def-param-value name=\"" + name + "\">" + content.substr(eq + 1) + "</def-param-value>";
        return false;
    }
    placeholder.begin = s;
    placeholder.end = e + endKeyword.size();
    placeholder.name = content;
    return true;
}

bool ParamResolver::collect(const std::string& text, NameSet& used, std::string& error)
{
    Placeholder ph;
    size_t pos = 0;
    while (findNext(text, pos, ph, error)) {
        used.insert(ph.name);
        pos = ph.end;
    }
    return error.empty();
}

bool ParamResolver::merge(const NameSet& used,
                          const ParamMap& defaults,
                          const ParamMap& overrides,
                          ParamMap& values,
                          std::string& error)
{
    error.clear();
    for (const auto& ov : overrides) {
        if (used.find(ov.first) == used.end() && defaults.find(ov.first) == defaults.end()) {
            error = "parameter '" + ov.first + "' is overridden but never used";
            return false;
        }
    }
    values = defaults;
    for (const auto& ov : overrides) {
        values[ov.first] = ov.second;
    }
    return true;
}

bool ParamResolver::resolve(const std::string& text, const ParamMap& values, std::string& out, std::string& error)
{
    std::string result;
    Placeholder ph;
    size_t pos = 0;
    while (findNext(text, pos, ph, error)) {
        auto it = values.find(ph.name);
        if (it == values.end()) {
            error = "parameter '" + ph.name + "' has no value";
            return false;
        }
        result += text.substr(pos, ph.begin - pos) + it->second;
        pos = ph.end;
    }
    if (!error.empty()) {
        return false;
    }
    out = result + text.substr(pos);
    return true;
}
