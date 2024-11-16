/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef  PARAMETER_H
#define  PARAMETER_H

#include <string>
#include <vector>
#include <deque>
#include <memory>

// The name of the parameter is fully scoped. This means:
// 1)  myParam
// 2)  mySection1::myParam1
// 3)  mySection1::myParam2::myParam3
// etc.
class Parameter
{
    private:
    std::string paramOnlyName;
    std::string fullyScopedName;
    std::deque<std::string> paramGroups;

    public:
    std::string type;
    std::string units;
    std::string defaultValue;
    bool required = false;
    std::string description;
    std::string notes;
    std::string optional_variable_name;

    public:
    void setFullyScopedParamName(std::string fullyScopedParamName);

    //give param mySection1::myParam1::myParam2, the following functions will return:

    //mySection1::myParam1
    std::string getFullGroupOnlyName() const;

    //mySection1::myParam1::myParam2
    std::string getFullParamName() const;

    //myParam2
    std::string getParamOnly() const;

    //mySection1_myParam1_myParam2
    std::string getFullParamVariable() const;

    //mySection1, myParam1
    std::deque<std::string> getListOfGroups() const;

    private:
    ///mySection1_myParam1
    std::string getFullGroupOnlyVariable() const;
};

#endif
