/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "generator.h"
#include <sstream>
#include <string>
#include <regex>
#include <iostream>

// Example:

//((group: myGroupName)(name: myParamName)(type: string)(required: true)(units: myUnits)(defaultValue: myDefaultValue)(description: myDescription)(notes: myNotes))
//((group: myGroupName)(name: myParamName)(type: string)(required: true)(units: myUnits)(defaultValue: myDefaultValue)(description: myDescription)(notes: myNotes))
//((group: myGroupName)(name: myParamName)(type: string)(required: true)(units: myUnits)(defaultValue: myDefaultValue)(description: myDescription)(notes: myNotes))

bool ParamsFilesGenerator::parseIniParams(const std::string inputfile)
{
    std::regex pattern(R"(\((\w+): ([^)]+)\))"); // Matches "(attribute: value)"
    std::smatch matches;
    Parameter param;

    auto textBegin = std::sregex_iterator(inputfile.begin(), inputfile.end(), pattern);
    auto textEnd = std::sregex_iterator();

    // Iterate over each line
    for (std::sregex_iterator i = textBegin; i != textEnd; ++i)
    {
        std::smatch match = *i;
        std::string attribute = match[1];
        std::string value = match[2];
        std::string group_string;
        std::string param_string;

        if (attribute == "group")
        {
            group_string = trimSpaces(value);
        }
        else if (attribute == "name")
        {
            param_string = trimSpaces(value);
        }
        else if (attribute == "type")
        {
            param.type = trimSpaces(value);
        }
        else if (attribute == "required")
        {
            if (value == "Yes" || value == "yes" || value == "True" || value == "true" || value == "1")
                param.required = true;
        }
        else if (attribute == "units")
        {
            param.units = trimSpaces(value);
        }
        else if (attribute == "defaultValue")
        {
            param.defaultValue = trimSpaces(value);
        }
        else if (attribute == "description")
        {
            param.description = trimSpaces(value);
        }
        else if (attribute == "notes")
        {
            param.notes = trimSpaces(value);
        }
        else
        {
            std::cout << "Syntax error in attribute:" << attribute << std::endl;
            return false;
        }

        //create the name of the param
        {
            std::string fully_scoped_param_name;
            if (!group_string.empty())
            {
                fully_scoped_param_name = group_string + std::string("::") + param_string;
            }
            else
            {
                fully_scoped_param_name = param_string;
            }
            param.setFullyScopedParamName(fully_scoped_param_name);
        }

        m_params.push_back(param);
        m_sectionGroup.insert(param);
    }

    return true;
}
