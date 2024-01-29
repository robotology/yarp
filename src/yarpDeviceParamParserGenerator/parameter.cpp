/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string>
#include <vector>
#include <deque>
#include <chrono>
#include <ctime>
#include <memory>

#include "parameter.h"

const bool enable_debug_prints = false;
#define ADD_DEBUG_COMMENT(out) if (enable_debug_prints) { out << "/* " << __FUNCTION__ << ":" << __LINE__ << " */\n"; }
#define S_TAB1 "    "
#define S_TAB2 "        "
#define S_TAB3 "            "

void Parameter::setFullyScopedParamName(std::string fullyScopedParamName)
{
    this->fullyScopedName = fullyScopedParamName;
    const std::string delimiter = "::";
    size_t pos = 0;
    std::string token;


    while ((pos = fullyScopedParamName.find(delimiter)) != std::string::npos) {
        token = fullyScopedParamName.substr(0, pos);
        paramGroups.push_back(token);
        fullyScopedParamName.erase(0, pos + delimiter.length());
    }

    if (!fullyScopedParamName.empty())
    {
        this->paramOnlyName=fullyScopedParamName;
    }
}

std::string Parameter::getFullGroupOnlyName() const
{
    if (this->paramGroups.size() == 0)
        return std::string();

    std::string full_groupname = this->paramGroups[0];
    for (size_t i = 1; i < this->paramGroups.size(); i++)
        full_groupname = full_groupname + std::string("::") + this->paramGroups[i];

    return full_groupname;
}

std::string Parameter::getFullParamName() const
{
    if (this->paramGroups.size() == 0)
        return this->paramOnlyName;

    return getFullGroupOnlyName() + std::string("::") + this->paramOnlyName;
}

std::string Parameter::getFullGroupOnlyVariable() const
{
    if (!optional_variable_name.empty())
        return optional_variable_name;

    if (this->paramGroups.size() == 0)
        return std::string();

    std::string full_groupname = this->paramGroups[0];
    for (size_t i = 1; i < this->paramGroups.size(); i++)
        full_groupname = full_groupname + std::string("_") + this->paramGroups[i];

    return full_groupname;
}

std::string Parameter::getFullParamVariable() const
{
    //If the user has requested a special name for that variable, then use it
    if (!this->optional_variable_name.empty())
        return this->optional_variable_name;

    //Otherwise generate it from the parameter name
    if (this->paramGroups.size() == 0)
        return this->paramOnlyName;

    return getFullGroupOnlyVariable() + std::string("_") + this->paramOnlyName;
}

std::string Parameter::getParamOnly() const
{
    return this->paramOnlyName;
}

std::deque<std::string> Parameter::getListOfGroups() const
{
    return this->paramGroups;
}
