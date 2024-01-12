/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "generator.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <deque>

std::string ParamsFilesGenerator::generateYarpdevString()
{
    std::ostringstream s;
    s << "The device can be launched by yarpdev using the following syntax:\n";
    s << "yarpdev --device " << this->m_classname;
    for (auto param : m_params)
    {
        s << " --" << param.getFullParamName() << " ";
        if (!param.defaultValue.empty())
        {
            s << param.defaultValue << " ";
        }
        else
        {
            if (param.required)
            {
                s << "<mandatory_value>";
            }
            else
            {
                s << "<optional_value>";
            }
        }
    }
    s << "\n";
    return s.str();
}

std::string ParamsFilesGenerator::generateYarpdevFile()
{
    std::ostringstream s;
    m_sectionGroup.iterator_start();

    auto param_old = m_sectionGroup.iterator_get();
    auto param_curr = m_sectionGroup.iterator_get();
    do
    {
        param_old = param_curr;
        param_curr = m_sectionGroup.iterator_get();

        //case 1
        if (param_curr->isParameter())
        {
            s << param_curr->param.getParamOnly() << " ";

            if (!param_curr->param.defaultValue.empty())
            {
                s << param_curr->param.defaultValue << "\n";
            }
            else
            {
                if (param_curr->param.required)
                {
                    s << "<mandatory_value>" << "\n";
                }
                else
                {
                    s << "<optional_value>" << "\n";
                }
            }
        }
        //case 2: hen this is a group of Parameters. Is this the same group?
        else if (param_curr != param_old)
        {
            s << "\n";
            s << "[ " << param_curr->name << " ]\n";
        }
    }
    while (m_sectionGroup.iterator_next() != nullptr);
    return s.str();
}
