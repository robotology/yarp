/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "generator.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <stack>

/*
< ? xml version = "1.0" encoding = "UTF-8" ? >
<!DOCTYPE robot PUBLIC "-//YARP//DTD yarprobotinterface 3.0//EN" "http://www.yarp.it/DTD/yarprobotinterfaceV3.0.dtd">
<robot name = "robot1" build = "1" xmlns:xi = "http://www.w3.org/2001/XInclude">
<devices>
    <device xmlns:xi="http://www.w3.org/2001/XInclude" name="device_example" type="embObjBattery">
        <param name="param_1">            0      </param>
        <param name="param_2">            0      </param>
        <group name="GROUP_PARAM1">
             <param name="param_3">       0      </param>
             <param name="param_4">       0      </param>
        </group>
    </devices>
</robot>
*/

//ids stands for indentation spaces
inline std::string ids(size_t indent)
{
    std::string spaces(indent*4, ' ');
    return spaces;
}

std::string ParamsFilesGenerator::generateYarprobotinterface()
{
    std::ostringstream s;
    s << "\
<?xml version = \"1.0\" encoding = \"UTF-8\" ?>\n\
<!DOCTYPE robot PUBLIC \"-//YARP//DTD yarprobotinterface 3.0//EN\" \"http://www.yarp.it/DTD/yarprobotinterfaceV3.0.dtd\">\n\
<robot name = \"robot1\" build = \"1\" xmlns:xi = \"http://www.w3.org/2001/XInclude\">\n\
<devices>\n\
    <device xmlns:xi = \"http://www.w3.org/2001/XInclude\" name = \"device_example\" type = \"" << m_modulename << "\">\n\
";

    m_sectionGroup.iterator_start();

    auto param_old = m_sectionGroup.iterator_get();
    auto param_curr = m_sectionGroup.iterator_get();

    std::stack <std::string> groups_to_be_closed;
    do
    {
        param_old = param_curr;
        param_curr = m_sectionGroup.iterator_get();

        // Open a group section
        if (param_curr->nestingLevel > param_old->nestingLevel)
        {
            s << "\n";
            s << ids(groups_to_be_closed.size()) << "        <group name=\"" << param_old->name << "\">\n";
            groups_to_be_closed.push (param_old->name);
        }

        // Close a group section
        if (param_curr->nestingLevel < param_old->nestingLevel)
        {
            s << ids(groups_to_be_closed.size()-1) << "        </group>\n";
            groups_to_be_closed.pop();
        }

        //This block generates <param name="xxxx">   yyyy  </param>
        if (param_curr->nextNode.size() == 0)
        {
            Parameter* pp = &(param_curr->param);

            s << ids(groups_to_be_closed.size()) << "        <param name=\"" << pp->getParamOnly() << "\"> ";
            if (!pp->defaultValue.empty())
            {
                s << pp->defaultValue;
            }
            else
            {
                if (pp->required)
                {
                    s << "mandatory_value";
                }
                else
                {
                    s << "optional_value";
                }
            }
            s << " </param>\n";
        }
    }
    while (m_sectionGroup.iterator_next() != nullptr);

    //empty all stack, closing all open group sections
    while (groups_to_be_closed.size() > 0)
    {
        //s << "[ close " << groups_to_be_closed.top() << "]\n"; //debug only
        s << ids(groups_to_be_closed.size()-1) << "        </group>\n";
        groups_to_be_closed.pop();
    }

    s << "\
    </device>\n\
</devices>\n\
</robot>\n";

    return s.str();
}
