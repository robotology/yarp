/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <vector>
#include <cstdio>

#include <iostream>
#include <fstream>

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

#include "generator.h"

std::string ParamsFilesGenerator::generateFunction_getListOfParams()
{
    std::ostringstream s;
    ADD_DEBUG_COMMENT(s)
s << "\n\
std::vector<std::string> " << m_classname << "_ParamsParser::getListOfParams() const\n\
{\n";

s << S_TAB1 << "std::vector<std::string> params;\n";

    ADD_DEBUG_COMMENT(s)
for (const auto& param : m_params)
{
    s << S_TAB1 << "params.push_back(\"" << param.getFullParamName() << "\");\n";
}

    ADD_DEBUG_COMMENT(s)
s << S_TAB1 << "return params;\n\
}\n\
\n\
";

    return s.str();
}

std::string ParamsFilesGenerator::generateFunction_getDocumentationOfDeviceParams()
{
    std::ostringstream s;
    ADD_DEBUG_COMMENT(s)

    s << "\n";
    s << "std::string      " << m_classname << "_ParamsParser::getDocumentationOfDeviceParams() const\n";
    s << "{\n";
    s << S_TAB1 << "std::string doc;\n";

    s << S_TAB1 << "doc = doc + std::string(\"\\n=============================================\\n\");\n";
    s << S_TAB1 << "doc = doc + std::string(\"This is the help for device: " << m_classname << "\\n\");\n";
    s << S_TAB1 << "doc = doc + std::string(\"\\n\");\n";
    s << S_TAB1 << "doc = doc + std::string(\"This is the list of the parameters accepted by the device:\\n\");\n";

    for (const auto& param : m_params)
    {
         s << S_TAB1 << "doc = doc + std::string(\"'" << param.getFullParamName() << "': ";
         s << param.description;
         s << "\\n\");\n";
    }

    s << S_TAB1 << "doc = doc + std::string(\"\\n\");\n";
    s << S_TAB1 << "doc = doc + std::string(\"Here are some examples of invocation command with yarpdev, with all params:\\n\");\n";
    s << S_TAB1 << "doc = doc + \"" << generateYarpdevStringAllParams() << "\\n\";\n";
    s << S_TAB1 << "doc = doc + std::string(\"Using only mandatory params:\\n\");\n";
    s << S_TAB1 << "doc = doc + \"" << generateYarpdevStringMandatoryParamsOnly() << "\\n\";\n";
    s << S_TAB1 << "doc = doc + std::string(\"=============================================\\n\\n\");";
    s << S_TAB1 << "return doc;\n";
    s << "}\n";
    return s.str();
}

void ParamsFilesGenerator::generate_section(std::ostringstream& s, std::deque<std::string> vec, size_t count, size_t siz)
{
    if (vec.size()==0) return;

    //The first iteration should take data from config...(1)
    if (count == 0)
    {
        //The last iteration should put data in sectionp...(2)
        if (count == siz-1)
        {
            s << S_TAB2 << "yarp::os::Bottle sectionp" << ";\n";
            s << S_TAB2 << "sectionp" << " = config.findGroup(\"" << vec.front() << "\");\n";
        }
        //(2)...All other iteration should put data in sectionp<count>
        else
        {
            s << S_TAB2 << "yarp::os::Bottle sectionp" << count << ";\n";
            s << S_TAB2 << "sectionp" << count << " = config.findGroup(\"" << vec.front() << "\");\n";
        }
    }
    //(1)...All other iteration should take data from previous sectionp.
    else
    {
        //The last iteration should put data in sectionp...(2)
        if (count == siz-1)
        {
            s << S_TAB2 << "yarp::os::Bottle sectionp" << ";\n";
            s << S_TAB2 << "sectionp" << " = " << "sectionp" << count - 1 << ".findGroup(\"" << vec.front() << "\");\n";
        }
        //(2)...All other iteration should put data in sectionp<count>
        else
        {
            s << S_TAB2 << "yarp::os::Bottle sectionp" << count << ";\n";
            s << S_TAB2 << "sectionp" << count << " = " << "sectionp" << count - 1 << ".findGroup(\"" << vec.front() << "\");\n";
        }
    }

    vec.pop_front();
    generate_section(s,vec, count+1, siz);
}

void ParamsFilesGenerator::generate_param(std::string origin, std::ostringstream& s, const Parameter& param)
{
    s << \
        S_TAB2 << "if ("<<origin<<".check(\"" << param.getParamOnly() << "\"))\n" << \
        S_TAB2 << "{\n";

    s << \
        S_TAB3 << "m_" << param.getFullParamVariable() << " = "<<origin<<".find(\"" << param.getParamOnly() << "\")";

    if (param.type == "string")      { s << ".asString();\n"; }
    else if (param.type == "bool")   { s << ".asBool();\n"; }
    else if (param.type == "double") { s << ".asFloat64();\n"; }
    else if (param.type == "int")    { s << ".asInt64();\n"; }
    else if (param.type == "size_t") { s << ".asInt64();\n"; }
    else if (param.type == "float")  { s << ".asFloat32();\n"; }
    else {
        yFatal("ERROR: Unknown data type for param %s: %s",param.getFullParamName().c_str(), param.type.c_str()); //error
    }

    ADD_DEBUG_COMMENT(s)
        s << \
        S_TAB3 << "yCInfo("<< m_component<< ") << \"Parameter '" << param.getFullParamName() << "' using value:\" << m_" << param.getFullParamVariable() << ";\n";


    ADD_DEBUG_COMMENT(s)

        s << \
        S_TAB2 << "}\n" << \
        S_TAB2 << "else\n" << \
        S_TAB2 << "{\n";

    if (param.required)
    {
        ADD_DEBUG_COMMENT(s)
            s << \
            S_TAB3 << "yCError(" << m_component << ") << \"Mandatory parameter '" << param.getFullParamName() << "' not found!\";\n";
        if (!param.description.empty())
        {
            s << \
            S_TAB3 << "yCError(" << m_component << ") << \"Description of the parameter: " << param.description << "\";\n";
        }
        if (!param.units.empty())
        {
            s << \
            S_TAB3 << "yCError(" << m_component << ") << \"Remember: Units for this parameter are: '" << param.units << "'\";\n";
        }
        s << \
            S_TAB3 << "return false;\n";
    }
    else
    {
        ADD_DEBUG_COMMENT(s)
            s << \
            S_TAB3 << "yCInfo(" << m_component << ") << \"Parameter '" << param.getFullParamName() << "' using DEFAULT value:\" << m_" << param.getFullParamVariable() << ";\n";
    }


    ADD_DEBUG_COMMENT(s)
        s << \
        S_TAB2 << "}\n\
";
}

std::string ParamsFilesGenerator::generateFunction_parseParams()
{
    std::ostringstream s;
    ADD_DEBUG_COMMENT(s)
s << "\n\
bool      "<< m_classname << "_ParamsParser::parseParams(const yarp::os::Searchable & config)\n\
{\n";

    s << S_TAB1 << "//Check for --help option\n";
    s << S_TAB1 << "if (config.check(\"help\"))\n";
    s << S_TAB1 << "{\n";
    s << S_TAB1 << "    yCInfo(" << m_component << ") << getDocumentationOfDeviceParams();\n";
    s << S_TAB1 << "}\n";
    s << "\n";

    ADD_DEBUG_COMMENT(s)
    s << S_TAB1 << "std::string config_string = config.toString();\n";
    s << S_TAB1 << "yarp::os::Property prop_check(config_string.c_str());\n";

    ADD_DEBUG_COMMENT(s)
    auto copy_of_m_params = m_params;
    for (const auto& param : copy_of_m_params)
    {
        if   (param.getListOfGroups().empty())
        {
            s << S_TAB1 << "//Parser of parameter " <<  param.getParamOnly() <<"\n";
            s << S_TAB1 << "{\n";
            generate_param("config", s, param);
            s << S_TAB2 << "prop_check.unput(\"" << param.getParamOnly() << "\");\n";
            s << S_TAB1 << "}\n";
            s << "\n";
        }
        else
        {
            s << S_TAB1 << "//Parser of parameter " << param.getFullParamName() << "\n";
            s << S_TAB1 << "{\n";
            auto pg = param.getListOfGroups();
            generate_section (s,pg,0,pg.size());
            generate_param("sectionp", s, param);
            s << S_TAB2 << "prop_check.unput(\"" << param.getFullParamName() << "\");\n";
            s << S_TAB1 << "}\n";
            s << "\n";
        }
    }

    ADD_DEBUG_COMMENT(s)

    //commented because this code is not ready yet
    s << S_TAB1 << "/*\n";

    s << S_TAB1 << "//This code check if the user set some parameter which are not check by the parser\n";
    s << S_TAB1 << "//If the parser is set in strict mode, this will generate an error\n";
    s << S_TAB1 << "if (prop_check.size() > 0)\n";
    s << S_TAB1 << "{\n";
    s << S_TAB1 << "    bool extra_params_found = false;\n";
    s << S_TAB1 << "    for (auto it=prop_check.begin(); it!=prop_check.end(); it++)\n";
    s << S_TAB1 << "    {\n";
    s << S_TAB1 << "        if (m_parser_is_strict)\n";
    s << S_TAB1 << "        {\n";
    s << S_TAB1 << "            yCError(" << m_component << ") << \"User asking for parameter: \"<<it->name <<\" which is unknown to this parser!\";\n";
    s << S_TAB1 << "            extra_params_found = true;\n";
    s << S_TAB1 << "        }\n";
    s << S_TAB1 << "        else\n";
    s << S_TAB1 << "        {\n";
    s << S_TAB1 << "            yCWarning(" << m_component << ") << \"User asking for parameter: \"<< it->name <<\" which is unknown to this parser!\";\n";
    s << S_TAB1 << "        }\n";
    s << S_TAB1 << "    }\n";
    s << "\n";
    s << S_TAB1 << "   if (m_parser_is_strict && extra_params_found)\n";
    s << S_TAB1 << "   {\n";
    s << S_TAB1 << "       return false;\n";
    s << S_TAB1 << "   }\n";
    s << S_TAB1 << "}\n";

    //commented because this code is not ready yet
    s << S_TAB1 << "*/\n";

s <<\
S_TAB1 << "return true;\n\
}\n\
\n\
";

    return s.str();
}
