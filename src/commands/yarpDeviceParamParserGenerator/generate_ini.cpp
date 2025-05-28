/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "generator.h"
#include <sstream>

// Example:

//((group: myGroupName)(name: myParamName)(type: string)(required: true)(units: myUnits)(defaultValue: myDefaultValue)(description: myDescription)(notes: myNotes))
//((group: myGroupName)(name: myParamName)(type: string)(required: true)(units: myUnits)(defaultValue: myDefaultValue)(description: myDescription)(notes: myNotes))
//((group: myGroupName)(name: myParamName)(type: string)(required: true)(units: myUnits)(defaultValue: myDefaultValue)(description: myDescription)(notes: myNotes)(optionalVariableName: myVar))
//((group: myGroupName)(name: myParamName)(type: string)(required: true)(units: myUnits)(defaultValue: myDefaultValue)(description: myDescription)(notes: myNotes))

std::string ParamsFilesGenerator::generateIniParams()
{
    std::ostringstream s;
    for (auto param : m_params)
    {
        std::string paramGroup = param.getFullGroupOnlyName();
        std::string paramName = param.getParamOnly();
        std::string type = param.type;
        std::string units = param.units;
        std::string defaultValue = param.defaultValue;
        std::string description = param.description;
        std::string notes = param.notes;
        if (paramGroup.empty()) paramGroup="-";
        if (paramName.empty()) paramName = "-";
        if (type.empty()) type = "-";
        if (units.empty()) units = "-";
        if (defaultValue.empty()) defaultValue = "-";
        if (description.empty()) description = "-";
        if (notes.empty()) notes = "-";

        s << "(";
        s << "(group: "        << paramGroup << " )";
        s << "(name: "         << paramName << " )";
        s << "(type: "         << type << " )";
        s << "(units: "        << units << " )";
        s << "(defaultValue: " << defaultValue << " )";
        s << "(required: "     << param.required << " )";
        s << "(description: "  << description << " )";
        s << "(notes: "        << notes << " )";
        if (!param.optional_variable_name.empty())
        {
            s << "(optionalVariableName: " << param.optional_variable_name << " )";
        }
        s << "(notes: " << notes << " )";
        s << ")\n";
    }
    return s.str();
}
