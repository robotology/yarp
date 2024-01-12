/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "generator.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <string>

// Example:

// | Parameter name | SubParameter | Type      | Units    | Default Value  | Required  | Description   | Notes   |
// | :----------- : | : -------- : | : --- - : | : ---- : | : -------- - : | : --------| : --------- : | : ----: |
// | myGroupName    | myParamName  | string    |  myUnits | myDefaultValue | true      | myDescription | myNotes |
// | myGroupName    | myParamName  | string    |  myUnits | myDefaultValue | true      | myDescription | myNotes |
// | myGroupName    | myParamName  | string    |  myUnits | myDefaultValue | true      | myDescription | myNotes |

std::string ParamsFilesGenerator::generateMdParams()
{
    std::ostringstream s;

    std::vector<size_t> fill_siz(8);
    fill_siz[0] = std::string("Group name").length();
    fill_siz[1] = std::string("Parameter name").length();
    fill_siz[2] = std::string("Type").length();
    fill_siz[3] = std::string("Units").length();
    fill_siz[4] = std::string("Default Value").length();
    fill_siz[5] = std::string("Required").length();
    fill_siz[6] = std::string("Description").length();
    fill_siz[7] = std::string("Notes").length();

    for (const auto& param : m_params) {
        std::string fullgroupname = param.getFullGroupOnlyName();
        std::string paramname = param.getParamOnly();
        fill_siz[0] = std::max(fill_siz[0], fullgroupname.length());
        fill_siz[1] = std::max(fill_siz[1], paramname.length());
        fill_siz[2] = std::max(fill_siz[2], param.type.length());
        fill_siz[3] = std::max(fill_siz[3], param.units.length());
        fill_siz[4] = std::max(fill_siz[4], param.defaultValue.length());
        fill_siz[5] = std::max(fill_siz[5], std::string("true").length());
        fill_siz[6] = std::max(fill_siz[6], param.description.length());
        fill_siz[7] = std::max(fill_siz[7], param.notes.length());
    }

    s << " | " << std::setw(fill_siz[0]) << std::left << "Group name";
    s << " | " << std::setw(fill_siz[1]) << std::left << "Parameter name";
    s << " | " << std::setw(fill_siz[2]) << std::left << "Type";
    s << " | " << std::setw(fill_siz[3]) << std::left << "Units";
    s << " | " << std::setw(fill_siz[4]) << std::left << "Default Value";
    s << " | " << std::setw(fill_siz[5]) << std::left << "Required";
    s << " | " << std::setw(fill_siz[6]) << std::left << "Description";
    s << " | " << std::setw(fill_siz[7]) << std::left << "Notes";
    s << " |\n";

    s << std::setfill('-');
    s << " |:" << std::setw(fill_siz[0]) << std::left << " ";
    s << ":|:" << std::setw(fill_siz[1]) << std::left << " ";
    s << ":|:" << std::setw(fill_siz[2]) << std::left << " ";
    s << ":|:" << std::setw(fill_siz[3]) << std::left << " ";
    s << ":|:" << std::setw(fill_siz[4]) << std::left << " ";
    s << ":|:" << std::setw(fill_siz[5]) << std::left << " ";
    s << ":|:" << std::setw(fill_siz[6]) << std::left << " ";
    s << ":|:" << std::setw(fill_siz[7]) << std::left << " ";
    s << ":|\n";

    s << std::setfill(' ');
    for (auto param : m_params)
    {
        std::string paramGroup = param.getFullGroupOnlyName();
        std::string paramName = param.getParamOnly();
        std::string type = param.type;
        std::string units = param.units;
        std::string defaultValue = param.defaultValue;
        std::string description = param.description;
        std::string notes = param.notes;
        if (paramGroup.empty()) paramGroup = "-";
        if (paramName.empty()) paramName = "-";
        if (type.empty()) type = "-";
        if (units.empty()) units = "-";
        if (defaultValue.empty()) defaultValue = "-";
        if (description.empty()) description = "-";
        if (notes.empty()) notes = "-";

        s << " | " << std::setw(fill_siz[0]) << std::left << paramGroup;
        s << " | " << std::setw(fill_siz[1]) << std::left << paramName;
        s << " | " << std::setw(fill_siz[2]) << std::left << type;
        s << " | " << std::setw(fill_siz[3]) << std::left << units;
        s << " | " << std::setw(fill_siz[4]) << std::left << defaultValue;
        s << " | " << std::setw(fill_siz[5]) << std::left << param.required;
        s << " | " << std::setw(fill_siz[6]) << std::left << description;
        s << " | " << std::setw(fill_siz[7]) << std::left << notes;
        s << " |\n";
    }
    s << "\n";
    return s.str();
}

std::string ParamsFilesGenerator::generateReadmeMd()
{
    std::ostringstream s;
    s << "These are the parameters used by class:" << m_classname << ".\n";
    s << generateMdParams();
    return s.str();
}
