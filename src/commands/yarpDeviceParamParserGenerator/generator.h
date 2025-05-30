/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef  GENERATOR_H
#define  GENERATOR_H

#include <string>
#include <vector>
#include <deque>
#include <chrono>
#include <ctime>
#include <memory>

#include "parameter.h"
#include "paramGroupTree.h"
#include "utils.h"

const bool enable_debug_prints = false;
#define ADD_DEBUG_COMMENT(out) if (enable_debug_prints) { out << "/* " << __FUNCTION__ << ":" << __LINE__ << " */\n"; }
#define S_TAB1 "    "
#define S_TAB2 "        "
#define S_TAB3 "            "

#define MACRO_PARSER_VERSION_MAJOR 2
#define MACRO_PARSER_VERSION_MINOR 0

class ParamsFilesGenerator
{
public:
    ParamsFilesGenerator() = default;

    std::deque<Parameter> m_params;
    SectionHandler        m_sectionGroup;
    bool        m_parser_is_strict = false;

    std::string m_classname;
    std::string m_modulename;
    std::string m_component;
    std::string m_output_header_filename;
    std::string m_output_cpp_filename;
    std::string m_output_ini_filename;
    std::string m_output_md_filename;
    std::string m_output_yarpdev_filename;
    std::string m_output_yarprobotinterface_filename;
    std::string m_output_readme_md_filename;
    std::string m_extra_comments;

    void printParams();
    std::string generateCpp();
    std::string generateConstructor();
    std::string generateFunction_getListOfParams();
    std::string generateFunction_getParamValue();
    std::string generateFunction_getConfiguration();
    std::string generateFunction_parseParams();
    std::string generateFunction_getDeviceType() { return ""; };
    std::string generateFunction_getDocumentationOfDeviceParams();
    std::string generateIniParams();
    std::string generateMdParams();
    std::string generateYarprobotinterface();
    std::string generateYarpdevDoxyString();
    std::string generateYarpdevStringMandatoryParamsOnly();
    std::string generateYarpdevStringAllParams();
    std::string generateYarpdevFile();
    std::string generateHeader();
    std::string generateDoxygenForHeaderClass();
    std::string generateReadmeMd();

    bool parseIniParams(std::string inputfilename);
    bool parseMdParams(std::string inputfilename);
    bool parseExtraComments(std::string inputfilename);

    void generate_param(std::string origin, std::ostringstream& s, const Parameter& param);
    void generate_section (std::ostringstream& s, std::deque<std::string> vec, size_t count, size_t siz);

    bool nested_sections_found();
};

const std::string license_banner = "\
/*\n\
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)\n\
 * SPDX-License-Identifier: LGPL-2.1-or-later\n\
 */\n\
\n\
";

const std::string version_banner = "\
\n\
// Generated by yarpDeviceParamParserGenerator ("+ std::to_string(MACRO_PARSER_VERSION_MAJOR)+
std::string(".")+ std::to_string(MACRO_PARSER_VERSION_MINOR) + std::string(")\n\
// This is an automatically generated file. Please do not edit it.\n\
// It will be re-generated if the cmake flag ALLOW_DEVICE_PARAM_PARSER_GERNERATION is ON.\n\
\n\
");

#endif
