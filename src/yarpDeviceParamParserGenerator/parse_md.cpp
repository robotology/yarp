/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "generator.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>

// Example:

// | Parameter name | SubParameter | Type      | Units    | Default Value  | Required  | Description   | Notes   |
// | :----------- : | : -------- : | : --- - : | : ---- : | : -------- - : | : --------| : --------- : | : ----: |
// | myGroupName    | myParamName  | string    |  myUnits | myDefaultValue | true      | myDescription | myNotes |
// | myGroupName    | myParamName  | string    |  myUnits | myDefaultValue | true      | myDescription | myNotes |
// | myGroupName    | myParamName  | string    |  myUnits | myDefaultValue | true      | myDescription | myNotes |

bool ParamsFilesGenerator::parseMdParams(const std::string inputfilename)
{
    std::string line;

    std::ifstream inputfile(inputfilename);
    std::vector<Parameter> params;
    bool b = inputfile.is_open();
    if (!b)
    {
        std::cout << "Unable to open file: " << inputfilename << std::endl;
        return false;
    }

    // Read the file line by line
    while (std::getline(inputfile, line))
    {
        std::stringstream ss(line);
        std::string item;
        Parameter param;

        // Leggi i valori separati da '|'
        std::getline(ss, item, '|'); // Ignora la prima barra vuota
        std::getline(ss, item, '|');
        std::string group_string = trimSpaces(item);
        if (containsOnlySymbols(group_string)) group_string = "";
        std::getline(ss, item, '|');
        std::string param_string = trimSpaces(item);
        if (containsOnlySymbols(param_string)) param_string = "";

        std::string fully_scoped_param_name;
        if (!group_string.empty())
            {fully_scoped_param_name = group_string+std::string("::")+param_string;}
        else
            {fully_scoped_param_name = param_string;}

        param.setFullyScopedParamName(fully_scoped_param_name);


        std::getline(ss, item, '|');
        param.type = trimSpaces(item);

        std::getline(ss, item, '|');
        param.units = trimSpaces(item);
        if (containsOnlySymbols(param.units)) param.units = "";

        std::getline(ss, item, '|');
        param.defaultValue = trimSpaces(item);
        if (containsOnlySymbols(param.defaultValue)) param.defaultValue = "";

        std::getline(ss, item, '|');
        std::string req_s = trimSpaces(item);
        if (req_s == "Yes" || req_s == "yes" || req_s == "True" || req_s == "true" || req_s == "1")
            param.required = true;

        std::getline(ss, item, '|');
        param.description = trimSpaces(item);
        if (containsOnlySymbols(param.description)) param.description = "";

        std::getline(ss, item, '|');
        param.notes = trimSpaces(item);
        if (containsOnlySymbols(param.notes)) param.notes = "";

        std::getline(ss, item, '\n');

        // Add the param
        m_params.push_back(param);
        m_sectionGroup.insert(param);
    }

//just for test
#if 0
    Parameter ppp1;
    ppp1.setFullyScopedParamName(std::string("aa::bb::cc::kk"));
    ppp1.type="string";
    m_params.push_back(ppp1);
    m_sectionGroup.insert(ppp1);

    Parameter ppp2;
    ppp2.setFullyScopedParamName(std::string("aa::dd::ee::kk"));
    ppp2.type = "string";
    m_params.push_back(ppp2);
    m_sectionGroup.insert(ppp2);
#endif

    inputfile.close();
    return true;
}
