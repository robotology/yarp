/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "generator.h"
#include <sstream>

std::string ParamsFilesGenerator::generateDoxygenForHeaderClass()
{
    std::string md_banner= generateMdParams();

    md_banner = doxygenize_string(md_banner);

//Not used:
//*@ingroup dev_impl_media\n

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
{ s << doxygenize_string(generateYarpdevString()) << "\n"; }

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
    s << "\
\n\
#include <yarp/os/Searchable.h>\n\
#include <yarp/dev/IDeviceDriverParams.h>\n\
#include <string>\n\
\n\
";

    s << generateDoxygenForHeaderClass();

    s << "\
class " << m_classname << "_params : public yarp::dev::IDeviceDriverParams\n\
{\n\
public:\n\
    ~" << m_classname << "_params() override = default;\n\
\n\
public:\n";

    s << S_TAB1 << "const std::string m_device_type = {\"" << m_classname << "\"};\n";

    for (const auto& param : m_params)
    {
        s << S_TAB1;
        if (param.type == "bool") { s << "bool "; }
        else if (param.type == "string") { s << "std::string "; }
        else if (param.type == "double") { s << "double "; }
        else if (param.type == "int") { s << "int "; }
        else if (param.type == "size_t") { s << "size_t "; }
        else if (param.type == "float") { s << "float "; }
        else {
            s << "ERROR! Generation failed because of unknown data type.\n";
        }

        s << "m_" << param.getFullParamVariable();

        if (param.defaultValue != "")
        {
            if (param.type == "string")
            {
                s << " = {\"" << param.defaultValue << "\"};\n";
            }
            else
            {
                s << " = {" << param.defaultValue << "};\n";
            }
        }
        else
        {
            s << ";\n";
        }
    }

    s << "\n\
    bool          parseParams(const yarp::os::Searchable & config) override;\n\
    std::string   getDeviceType() const override { return m_device_type; }\n\
    std::string   getDocumentationOfDeviceParams() const override;\n\
    std::vector<std::string> getListOfParams() const override;\n\
};\n\
";
    return s.str();
}
