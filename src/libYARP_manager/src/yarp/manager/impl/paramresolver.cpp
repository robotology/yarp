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
    size_t eq = content.find('=');
    placeholder.begin = s;
    placeholder.end = e + endKeyword.size();
    placeholder.name = content.substr(0, eq);
    placeholder.decl = Declaration();
    if (eq != std::string::npos) {
        placeholder.decl.hasDefault = true;
        placeholder.decl.defaultValue = content.substr(eq + 1);
    }

    if (placeholder.name.empty()) {
        error = "parameter with empty name in \"" + text + "\"";
        return false;
    }
    return true;
}

bool ParamResolver::collect(const std::string& text, DeclMap& decls, std::string& error)
{
    Placeholder ph;
    size_t pos = 0;
    while (findNext(text, pos, ph, error)) {
        auto it = decls.find(ph.name);
        if (it == decls.end()) {
            decls[ph.name] = ph.decl;
        } else if (ph.decl.hasDefault) {
            if (!it->second.hasDefault) {
                it->second = ph.decl;
            } else if (it->second.defaultValue != ph.decl.defaultValue) {
                error = "parameter '" + ph.name + "' has two different default values: '"
                        + it->second.defaultValue + "' and '" + ph.decl.defaultValue + "'";
                return false;
            }
        }
        pos = ph.end;
    }
    return error.empty();
}

bool ParamResolver::merge(const DeclMap& decls, const ParamMap& overrides, ParamMap& values, std::string& error)
{
    error.clear();
    values.clear();
    for (const auto& ov : overrides) {
        if (decls.find(ov.first) == decls.end()) {
            error = "parameter '" + ov.first + "' is overridden but never used";
            return false;
        }
    }
    for (const auto& decl : decls) {
        auto ov = overrides.find(decl.first);
        if (ov != overrides.end()) {
            values[decl.first] = ov->second;
        } else if (decl.second.hasDefault) {
            values[decl.first] = decl.second.defaultValue;
        }
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
