/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <vector>

#include <cstdio>
#include <cmath>
#include <mutex>

#include <iostream>
#include <fstream>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

#include "generator.h"

using namespace yarp::os;
#define RETURN_CODE_ERROR 1
#define RETURN_CODE_OK    0

void ParamsFilesGenerator::printParams()
{
    for (const auto& param : m_params)
    {
        std::cout << "Full Parameter name: " << param.getFullParamName() << std::endl;
        std::cout << "Type: " << param.type << std::endl;
        std::cout << "Units: " << param.units << std::endl;
        std::cout << "Default Value: " << param.defaultValue << std::endl;
        std::cout << "Required: " << param.required << std::endl;
        std::cout << "Description: " << param.description << std::endl;
        std::cout << "Notes: " << param.notes << std::endl;
        std::cout << "------------------------" << std::endl;
    }
}

bool ParamsFilesGenerator::nested_sections_found()
{
    bool b = true;
    for (auto param : m_params)
    {
        //aaa::bbb         is not nested (size == 1)
        //aaa::bbb::ccc    is nested (size == 2)
        if (param.getListOfGroups().size() > 1)
            b = false;
    }
    return !b;
}

void print_help()
{
    std::cout << "Welcome to YarpDeviceParamParserGenerator tool. Syntax:\n";
    std::cout << "1) YarpDeviceParamParserGenerator --class_name \"className\" --input_filename_md \"filename.md\" [--generate_md] [--generate_ini] [--generate_yarpdev] [--generate_yarprobotinterface] [--generate_all] [--output_dir \"output_path\"]\n";
    std::cout << "or:\n";
    std::cout << "2) YarpDeviceParamParserGenerator --class_name \"className\" --input_filename_ini \"filename.ini\" [--generate_md] [--generate_ini] [--generate_yarpdev] [--generate_yarprobotinterface] [--generate_all] [--output_dir \"output_path\"]\n";
}

int main(int argc, char *argv[])
{
    bool generate_ini_input_file = false;
    bool generate_md_input_file = false;
    bool generate_readme_md_file = false;
    bool generate_yarprobotinterface_file = false;
    bool generate_yarpdev_file = false;
    bool generate_code = true;
    std::string input_filename_type;
    std::string input_filename;
    std::string output_dir=".";
    std::string class_name;

   if (argc == 1)
   {
       print_help();
       return RETURN_CODE_ERROR;
   }

   for (int i = 1; i < argc; ++i)
   {
        std::string arg = argv[i];
        if (arg == "--help") {
            print_help();
            return RETURN_CODE_ERROR;
        }
        else if (arg == "--generate_md") {
            generate_md_input_file = true;
        }
        else if (arg == "--generate_ini") {
            generate_ini_input_file = true;
        }
        else if (arg == "--generate_yarpdev") {
            generate_yarpdev_file = true;
        }
        else if (arg == "--generate_yarprobotinterface") {
            generate_yarprobotinterface_file = true;
        }
        else if (arg == "--generate_readme_md_file") {
            generate_readme_md_file = true;
        }
        else if (arg == "--generate_all") {
            generate_readme_md_file = true;
            generate_yarprobotinterface_file = true;
            generate_yarpdev_file = true;
            generate_ini_input_file = true;
            generate_md_input_file = true;
        }
        else if (arg == "--input_filename_md"  && i+1 < argc && argv[i+1][0] != '-') {
            input_filename_type = "md";
            input_filename = argv[i+1];
            i++;
        }
        else if (arg == "--input_filename_ini" && i+1 < argc && argv[i+1][0] != '-') {
            input_filename_type = "ini";
            input_filename = argv[i+1];
            i++;
        }
        else if (arg == "--output_dir" && i+1 < argc && argv[i+1][0] != '-') {
            output_dir = argv[i+1];
            i++;
        }
        else if (arg == "--class_name" && i + 1 < argc && argv[i + 1][0] != '-') {
            class_name = argv[i + 1];
            i++;
        }
    }

    if (input_filename_type.empty())
    {
        std::cerr << "Invalid file type. Check parameter --input_filename_md or --input_filename_ini\n";
        return RETURN_CODE_ERROR;
    }
    if (input_filename.empty())
    {
        std::cerr << "Invalid file name. Check parameter --input_filename_md or --input_filename_ini\n";
        return RETURN_CODE_ERROR;
    }
    if (class_name.empty())
    {
        std::cerr << "Invalid class name. Check parameter --class_name\n";
        return RETURN_CODE_ERROR;
    }

    ParamsFilesGenerator pgen;
    if (input_filename_type == "md")
    {
        if (!pgen.parseMdParams(input_filename))
        {
            std::cerr<<"parseMdParams failed";
            return RETURN_CODE_ERROR;
        }
    }
    else if (input_filename_type == "ini")
    {
        if (!pgen.parseIniParams(input_filename))
        {
            std::cerr << "parseMdParams failed";
            return RETURN_CODE_ERROR;
        }
    }
    else
    {
        std::cerr << "Invalid input file name";
        return RETURN_CODE_ERROR;
    }

    //prepare the output path
    while (!output_dir.empty() && (output_dir.back() == '\\' || output_dir.back() == '/')) {
        output_dir.pop_back();}
    output_dir += '/';
    pgen.m_classname = class_name;
    pgen.m_component = pgen.m_classname + "ParamsCOMPONENT";
    std::string output_filename = class_name + "Params";
    pgen.m_output_header_filename = output_dir + output_filename + ".h";
    pgen.m_output_cpp_filename = output_dir + output_filename + ".cpp";
    pgen.m_output_ini_filename = output_dir + output_filename + ".bot";
    pgen.m_output_md_filename = output_dir + output_filename + ".md";
    pgen.m_output_readme_md_filename = output_dir + "readme.md";
    pgen.m_output_yarpdev_filename = output_dir + output_filename + ".ini";
    pgen.m_output_yarprobotinterface_filename = output_dir + output_filename + ".xml";

    //print params
    pgen.printParams();

    if (generate_ini_input_file)
    {
        std::string iniParamfile = pgen.generateIniParams();
        std::ofstream file_iniParamfile(pgen.m_output_ini_filename);
        bool b = file_iniParamfile.is_open();
        if (!b) { std::cerr << "Failed to write file:" << pgen.m_output_ini_filename; return RETURN_CODE_ERROR; }
        file_iniParamfile << iniParamfile;
        file_iniParamfile.close();
    }

    if (generate_md_input_file)
    {
        std::string mdParamfile = pgen.generateMdParams();
        std::ofstream file_mdParamfile(pgen.m_output_md_filename);
        bool b = file_mdParamfile.is_open();
        if (!b) { std::cerr << "Failed to write file:" << pgen.m_output_md_filename; return RETURN_CODE_ERROR; }
        file_mdParamfile << mdParamfile;
        file_mdParamfile.close();
    }

    if (generate_readme_md_file)
    {
        std::string readmeFile = pgen.generateReadmeMd();
        std::ofstream file_readmeParamfile(pgen.m_output_readme_md_filename);
        bool b = file_readmeParamfile.is_open();
        if (!b) { std::cerr << "Failed to write file:" << pgen.m_output_md_filename; return RETURN_CODE_ERROR; }
        file_readmeParamfile << readmeFile;
        file_readmeParamfile.close();
    }

    if (generate_yarpdev_file)
    {
        //yarpdev files (.ini format) have some limitations: they cannot be nested. For example:
        //aaa::bbb         This is ok, it is not nested (size of getListOfGroups() of param bbbb == 1)
        //aaa::bbb::ccc    is nested (size of getListOfGroups of param ccc == 2)
        if (!pgen.nested_sections_found())
        {
            std::string yarpdevParamfile = pgen.generateYarpdevFile();
            std::ofstream file_yarpdevParamfile(pgen.m_output_yarpdev_filename);
            bool b = file_yarpdevParamfile.is_open();
            if (!b) { std::cerr << "Failed to write file:" << pgen.m_output_yarpdev_filename; return RETURN_CODE_ERROR; }
            file_yarpdevParamfile << yarpdevParamfile;
            file_yarpdevParamfile.close();
        }
        else
        {
            std::cerr << "a yarpdev file with nested sections cannot be generated";
            std::ofstream file_yarpdevParamfile(pgen.m_output_yarpdev_filename);
            file_yarpdevParamfile << "a yarpdev file with nested sections cannot be generated";
            file_yarpdevParamfile.close();
        }
    }

    if (generate_yarprobotinterface_file)
    {
        //yarprobotinterface files (.xml format) support multiple levels of nesting
        std::string yrobotParamfile = pgen.generateYarprobotinterface();
        std::ofstream file_yrobotParamfile(pgen.m_output_yarprobotinterface_filename);
        bool b = file_yrobotParamfile.is_open();
        if (!b) { std::cerr << "Failed to write file:" << pgen.m_output_yarprobotinterface_filename; return RETURN_CODE_ERROR; }
        file_yrobotParamfile << yrobotParamfile;
        file_yrobotParamfile.close();
    }

    if (generate_code)
    {
        std::string hfile = pgen.generateHeader();
        std::ofstream file_hfile(pgen.m_output_header_filename);
        bool bh = file_hfile.is_open();
        if (!bh) { std::cerr << "Failed to write file:" << pgen.m_output_header_filename; return RETURN_CODE_ERROR; }
        file_hfile << hfile;
        file_hfile.close();

        std::string cppfile = pgen.generateCpp();
        std::ofstream file_cppfile(pgen.m_output_cpp_filename);
        bool bc = file_cppfile.is_open();
        if (!bc) { std::cerr << "Failed to write file:" << pgen.m_output_cpp_filename; return RETURN_CODE_ERROR; }
        file_cppfile << cppfile;
        file_cppfile.close();
    }

    std::cout << "Generation process successfully completed.";
    return RETURN_CODE_OK;
}

std::string ParamsFilesGenerator::generateCpp()
{
    std::ostringstream s;
s << license_banner;
s << version_banner;
s << current_time();
s << "\
\n\
#include \"" << m_classname << "Params.h\"\n\
#include <yarp/os/LogStream.h>\n\
#include <yarp/os/Value.h>\n\
\n\
namespace {\n\
    YARP_LOG_COMPONENT("<< m_component << ", \"yarp.device." << m_classname << "\")\n\
}\n\
\n";

    s << generateFunction_getListOfParams();
    s << generateFunction_parseParams();
    s << generateFunction_getDeviceType();
    s << generateFunction_getDocumentationOfDeviceParams();
    s << "\n";

    return s.str();
}
