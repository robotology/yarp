/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "generator.h"
#include <sstream>
#include <cctype>
#include <algorithm>

std::string ParamsFilesGenerator::generateDoxygenForHeaderClass()
{
    std::string md_banner= generateMdParams();

    md_banner = doxygenize_string(md_banner);

    std::ostringstream s;
s <<  "/**\n\
* This class is the parameters parser for class " << m_classname << ".\n";

    if (!m_extra_comments.empty())
    {
        s << "*";
        s << doxygenize_string(m_extra_comments) <<"\n";
    }

s << "\
*\n\
* These are the used parameters:\n\
";

    s << "*" << md_banner << "";

    if (1) // generate yardev command line:
    { s << doxygenize_string(generateYarpdevDoxyString()) << "\n"; }

s << "\
*/\n\
\n";

    return s.str();
}

std::string ParamsFilesGenerator::generateHeader()
{
    std::ostringstream s;
    s << license_banner;
    s << version_banner;
    s << current_time();
    s << "\n";

    std::string hguard = m_classname + "_ParamsParser_H";
    std::transform(hguard.begin(), hguard.end(), hguard.begin(), ::toupper);

    s << "#ifndef " << hguard << "\n";
    s << "#define " << hguard << "\n";
    s << "\n";
    s << "\
#include <yarp/os/Searchable.h>\n\
#include <yarp/dev/IDeviceDriverParams.h>\n\
#include <string>\n\
#include <cmath>\n\
\n\
";

    s << generateDoxygenForHeaderClass();

    s << "\
class " << m_classname << "_ParamsParser : public yarp::dev::IDeviceDriverParams\n\
{\n\
public:\n\
    " << m_classname << "_ParamsParser();\n\
    ~" << m_classname << "_ParamsParser() override = default;\n\
\n\
public:\n";

    s << S_TAB1 << "const std::string m_device_classname = {\"" << m_classname << "\"};\n";
    s << S_TAB1 << "const std::string m_device_name = {\"" << m_modulename << "\"};\n";
    s << S_TAB1 << "bool m_parser_is_strict = false;\n";
    s << S_TAB1 << "struct parser_version_type\n";
    s << S_TAB1 << "{\n";
    s << S_TAB1 << "     int major = "<< std::to_string(MACRO_PARSER_VERSION_MAJOR) << ";\n";
    s << S_TAB1 << "     int minor = "<< std::to_string(MACRO_PARSER_VERSION_MINOR) << ";\n";
    s << S_TAB1 << "};\n";
    s << S_TAB1 << "const parser_version_type m_parser_version = {};\n";
    s << "\n";
    s << S_TAB1 << "std::string m_provided_configuration;\n";

    s << "\n";
    for (const auto& param : m_params)
    {
        s << S_TAB1 << "const std::string m_" << param.getFullParamVariable() << "_defaultValue = {\"" << escapeQuotes(param.defaultValue) << "\"};\n";
    }
    s << "\n";

    for (const auto& param : m_params)
    {
        std::string rmv;
        s << S_TAB1;
        if (param.type == "bool")                { s << "bool "; rmv = "false";}
        else if (param.type == "string")         { s << "std::string "; rmv = "";}
        else if (param.type == "double")         { s << "double "; rmv = "std::nan(\"1\")";}
        else if (param.type == "int")            { s << "int "; rmv = "0";}
        else if (param.type == "size_t")         { s << "size_t "; rmv = "0";}
        else if (param.type == "float")          { s << "float "; rmv = "std::nanf(\"1\")";}
        else if (param.type == "char")           { s << "char "; rmv = "0"; }
        else if (param.type == "vector<int>")    { s << "std::vector<int> "; rmv = ""; }
        else if (param.type == "vector<string>") { s << "std::vector<std::string> "; rmv = ""; }
        else if (param.type == "vector<double>") { s << "std::vector<double> "; rmv = ""; }
        else {
            s << "ERROR! Generation failed because of unknown data type.\n";
        }

        s << "m_" << param.getFullParamVariable();

        //if the parameter has a default value...
        if (param.defaultValue != "")
        {
            if (param.type == "vector<int>" ||
                param.type == "vector<string>" ||
                param.type == "vector<double>")
            {
                s << " = { }; //Default values for lists are managed in the class constructor. It is highly recommended to provide a suggested value also for optional string parameters.\n";
            }
            else if (param.type == "string")
            {
                s << " = {\"" << param.defaultValue << "\"};\n";
            }
            else
            {
                s << " = {" << param.defaultValue << "};\n";
            }
        }
        //if the parameter has not a default value
        else
        {
            //optional parameter must have a default value!!!
            if (param.required == false)
            {
                if (param.type == "string")
                {
                    s << " = {" << rmv << "}; //This default value of this string is an empty string. It is highly recommended to provide a suggested value also for optional string parameters.\n";
                }
                else if (param.type == "vector<int>" ||
                         param.type == "vector<string>" ||
                         param.type == "vector<double>")
                {
                    s << " = {" << rmv << "}; //The default value of this list is an empty list. It is highly recommended to provide a suggested value also for optional string parameters.\n";
                }
                else
                {
                    s << " = ERROR! Generation failed because this optional param MUST have a default value!\n";
                }
            }
            //mandatory parameter might not have a default value
            else
            {
                s << " = {" << rmv << "}; //This default value is autogenerated. It is highly recommended to provide a suggested value also for mandatory parameters.\n";
            }
        }
    }

    s << "\n\
    bool          parseParams(const yarp::os::Searchable & config) override;\n\
    std::string   getDeviceClassName() const override { return m_device_classname; }\n\
    std::string   getDeviceName() const override { return m_device_name; }\n\
    std::string   getDocumentationOfDeviceParams() const override;\n\
    std::vector<std::string> getListOfParams() const override;\n\
    bool getParamValue(const std::string& paramName, std::string& paramValue) const override;\n\
    std::string   getConfiguration() const override;\n\
};\n\
";

    s << "\n";
    s << "#endif\n";

    return s.str();
}
