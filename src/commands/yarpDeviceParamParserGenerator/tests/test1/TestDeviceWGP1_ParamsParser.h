/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */


// Generated by yarpDeviceParamParserGenerator (2.0)
// This is an automatically generated file. Please do not edit it.
// It will be re-generated if the cmake flag ALLOW_DEVICE_PARAM_PARSER_GERNERATION is ON.

// Generated on: Thu May 22 11:34:45 2025


#ifndef TESTDEVICEWGP1_PARAMSPARSER_H
#define TESTDEVICEWGP1_PARAMSPARSER_H

#include <yarp/os/Searchable.h>
#include <yarp/dev/IDeviceDriverParams.h>
#include <string>
#include <cmath>

/**
* This class is the parameters parser for class TestDeviceWGP1.
*TestDeviceWGP is a fake device developed for testing the various functionalities of YarpDeviceParamParserGenerator tool.
*This block has been written with the purpose of testing the --input_extra_comments option.
*This last line has no specific meaning.
*Here there are some extra symbols used in the markdown format, such as the 'code' syntax.
*The **bold text** , the *italic text*, the list:
*1. First element
*2. Second element
*- unordered element 1
*- unordered element 2
*
*
* These are the used parameters:
* | Group name        | Parameter name | Type           | Units | Default Value       | Required | Description   | Notes |
* |:-----------------:|:--------------:|:--------------:|:-----:|:-------------------:|:--------:|:-------------:|:-----:|
* | -                 | param_1        | string         | -     | -                   | 1        | Test param 01 | Note  |
* | -                 | param_2        | double         | -     | -                   | 1        | Test param 02 | Note  |
* | -                 | param_3        | bool           | -     | -                   | 1        | Test param 03 | Note  |
* | -                 | param_4        | int            | -     | -                   | 1        | Test param 04 | Note  |
* | -                 | param_5        | char           | -     | -                   | 1        | Test param 05 | Note  |
* | -                 | param_6        | size_t         | -     | -                   | 1        | Test param 06 | Note  |
* | -                 | param_7        | float          | -     | -                   | 1        | Test param 07 | Note  |
* | group1            | param_1        | string         | -     | -                   | 1        | Test param 08 | Note  |
* | group1            | param_2        | double         | -     | -                   | 1        | Test param 09 | Note  |
* | group1            | param_3        | bool           | -     | -                   | 1        | Test param 10 | Note  |
* | group1            | param_4        | int            | -     | -                   | 1        | Test param 11 | Note  |
* | group2            | param_1        | string         | -     | def1                | 1        | Test param 12 | Note  |
* | group2            | param_2        | double         | -     | 1.0                 | 1        | Test param 13 | Note  |
* | group2            | param_3        | bool           | -     | true                | 1        | Test param 14 | Note  |
* | group2            | param_4        | int            | -     | 1000                | 1        | Test param 15 | Note  |
* | group3            | param_1        | string         | -     | def1                | 0        | Test param 16 | Note  |
* | group3            | param_2        | double         | -     | 1.0                 | 0        | Test param 17 | Note  |
* | group3            | param_3        | bool           | -     | true                | 0        | Test param 18 | Note  |
* | group3            | param_4        | int            | -     | 1000                | 0        | Test param 19 | Note  |
* | group4::subgroup1 | param_1        | int            | s     | 0                   | 0        | Test param 20 | Note  |
* | group4::subgroup1 | param_2        | int            | s     | 1                   | 0        | Test param 21 | Note  |
* | group4::subgroup1 | param_3        | int            | s     | 2                   | 0        | Test param 22 | Note  |
* | group4::subgroup1 | param_4        | int            | s     | 3                   | 0        | Test param 23 | Note  |
* | group4::subgroup2 | param_1        | int            | ms    | 10                  | 0        | Test param 24 | Note  |
* | group4::subgroup2 | param_2        | int            | ms    | 11                  | 0        | Test param 25 | Note  |
* | group4::subgroup2 | param_3        | int            | ms    | 12                  | 0        | Test param 26 | Note  |
* | group4::subgroup2 | param_4        | int            | ms    | 13                  | 0        | Test param 27 | Note  |
* | group5::subgroup1 | param_1        | int            | N     | 20                  | 1        | Test param 28 | Note  |
* | group5::subgroup1 | param_2        | int            | N     | 21                  | 1        | Test param 29 | Note  |
* | group5::subgroup1 | param_3        | int            | N     | 22                  | 1        | Test param 30 | Note  |
* | group5::subgroup1 | param_4        | int            | N     | 23                  | 1        | Test param 31 | Note  |
* | group5::subgroup2 | param_1        | int            | mN    | 30                  | 1        | Test param 32 | Note  |
* | group5::subgroup2 | param_2        | int            | mN    | 31                  | 1        | Test param 33 | Note  |
* | group5::subgroup2 | param_3        | int            | mN    | 32                  | 1        | Test param 34 | Note  |
* | group5::subgroup2 | param_4        | int            | mN    | 33                  | 1        | Test param 35 | Note  |
* | -                 | param_vec1     | vector<int>    | m     | -                   | 0        | Test param 36 | Note  |
* | -                 | param_vec2     | vector<double> | m     | -                   | 0        | Test param 37 | Note  |
* | -                 | param_vec3     | vector<string> | m     | -                   | 0        | Test param 38 | Note  |
* | -                 | param_vec4     | vector<int>    | m     | (1 2 3)             | 0        | Test param 39 | Note  |
* | -                 | param_vec5     | vector<double> | m     | (1.0 2.0 3.0)       | 0        | Test param 40 | Note  |
* | -                 | param_vec6     | vector<string> | m     | ("sa1" "sa2" "sa3") | 0        | Test param 41 | Note  |
* | -                 | param_vec7     | vector<int>    | m     | -                   | 1        | Test param 42 | Note  |
* | -                 | param_vec8     | vector<double> | m     | -                   | 1        | Test param 43 | Note  |
* | -                 | param_vec9     | vector<string> | m     | -                   | 1        | Test param 44 | Note  |
*
* The device can be launched by yarpdev using one of the following examples (with and without all optional parameters):
* \code{.unparsed}
* yarpdev --device testDeviceWGP1 --param_1 <mandatory_value> --param_2 <mandatory_value> --param_3 <mandatory_value> --param_4 <mandatory_value> --param_5 <mandatory_value> --param_6 <mandatory_value> --param_7 <mandatory_value> --group1::param_1 <mandatory_value> --group1::param_2 <mandatory_value> --group1::param_3 <mandatory_value> --group1::param_4 <mandatory_value> --group2::param_1 def1 --group2::param_2 1.0 --group2::param_3 true --group2::param_4 1000 --group3::param_1 def1 --group3::param_2 1.0 --group3::param_3 true --group3::param_4 1000 --group4::subgroup1::param_1 0 --group4::subgroup1::param_2 1 --group4::subgroup1::param_3 2 --group4::subgroup1::param_4 3 --group4::subgroup2::param_1 10 --group4::subgroup2::param_2 11 --group4::subgroup2::param_3 12 --group4::subgroup2::param_4 13 --group5::subgroup1::param_1 20 --group5::subgroup1::param_2 21 --group5::subgroup1::param_3 22 --group5::subgroup1::param_4 23 --group5::subgroup2::param_1 30 --group5::subgroup2::param_2 31 --group5::subgroup2::param_3 32 --group5::subgroup2::param_4 33 --param_vec1 <optional_value> --param_vec2 <optional_value> --param_vec3 <optional_value> --param_vec4 \" (1 2 3) \" --param_vec5 \" (1.0 2.0 3.0) \" --param_vec6 \" (\"sa1\" \"sa2\" \"sa3\") \" --param_vec7 <mandatory_value> --param_vec8 <mandatory_value> --param_vec9 <mandatory_value>
* \endcode
*
* \code{.unparsed}
* yarpdev --device testDeviceWGP1 --param_1 <mandatory_value> --param_2 <mandatory_value> --param_3 <mandatory_value> --param_4 <mandatory_value> --param_5 <mandatory_value> --param_6 <mandatory_value> --param_7 <mandatory_value> --group1::param_1 <mandatory_value> --group1::param_2 <mandatory_value> --group1::param_3 <mandatory_value> --group1::param_4 <mandatory_value> --group2::param_1 def1 --group2::param_2 1.0 --group2::param_3 true --group2::param_4 1000 --group5::subgroup1::param_1 20 --group5::subgroup1::param_2 21 --group5::subgroup1::param_3 22 --group5::subgroup1::param_4 23 --group5::subgroup2::param_1 30 --group5::subgroup2::param_2 31 --group5::subgroup2::param_3 32 --group5::subgroup2::param_4 33 --param_vec7 <mandatory_value> --param_vec8 <mandatory_value> --param_vec9 <mandatory_value>
* \endcode
*
*/

class TestDeviceWGP1_ParamsParser : public yarp::dev::IDeviceDriverParams
{
public:
    TestDeviceWGP1_ParamsParser();
    ~TestDeviceWGP1_ParamsParser() override = default;

public:
    const std::string m_device_classname = {"TestDeviceWGP1"};
    const std::string m_device_name = {"testDeviceWGP1"};
    bool m_parser_is_strict = false;
    struct parser_version_type
    {
         int major = 2;
         int minor = 0;
    };
    const parser_version_type m_parser_version = {};

    std::string m_provided_configuration;

    const std::string m_param_1_defaultValue = {""};
    const std::string m_param_2_defaultValue = {""};
    const std::string m_param_3_defaultValue = {""};
    const std::string m_param_4_defaultValue = {""};
    const std::string m_param_5_defaultValue = {""};
    const std::string m_param_6_defaultValue = {""};
    const std::string m_param_7_defaultValue = {""};
    const std::string m_group1_param_1_defaultValue = {""};
    const std::string m_group1_param_2_defaultValue = {""};
    const std::string m_group1_param_3_defaultValue = {""};
    const std::string m_group1_param_4_defaultValue = {""};
    const std::string m_group2_param_1_defaultValue = {"def1"};
    const std::string m_group2_param_2_defaultValue = {"1.0"};
    const std::string m_group2_param_3_defaultValue = {"true"};
    const std::string m_group2_param_4_defaultValue = {"1000"};
    const std::string m_group3_param_1_defaultValue = {"def1"};
    const std::string m_group3_param_2_defaultValue = {"1.0"};
    const std::string m_group3_param_3_defaultValue = {"true"};
    const std::string m_group3_param_4_defaultValue = {"1000"};
    const std::string m_group4_subgroup1_param_1_defaultValue = {"0"};
    const std::string m_group4_subgroup1_param_2_defaultValue = {"1"};
    const std::string m_group4_subgroup1_param_3_defaultValue = {"2"};
    const std::string m_group4_subgroup1_param_4_defaultValue = {"3"};
    const std::string m_group4_subgroup2_param_1_defaultValue = {"10"};
    const std::string m_group4_subgroup2_param_2_defaultValue = {"11"};
    const std::string m_group4_subgroup2_param_3_defaultValue = {"12"};
    const std::string m_group4_subgroup2_param_4_defaultValue = {"13"};
    const std::string m_group5_subgroup1_param_1_defaultValue = {"20"};
    const std::string m_group5_subgroup1_param_2_defaultValue = {"21"};
    const std::string m_group5_subgroup1_param_3_defaultValue = {"22"};
    const std::string m_group5_subgroup1_param_4_defaultValue = {"23"};
    const std::string m_group5_subgroup2_param_1_defaultValue = {"30"};
    const std::string m_group5_subgroup2_param_2_defaultValue = {"31"};
    const std::string m_group5_subgroup2_param_3_defaultValue = {"32"};
    const std::string m_group5_subgroup2_param_4_defaultValue = {"33"};
    const std::string m_param_vec1_defaultValue = {""};
    const std::string m_param_vec2_defaultValue = {""};
    const std::string m_param_vec3_defaultValue = {""};
    const std::string m_param_vec4_defaultValue = {"(1 2 3)"};
    const std::string m_param_vec5_defaultValue = {"(1.0 2.0 3.0)"};
    const std::string m_param_vec6_defaultValue = {"(\"sa1\" \"sa2\" \"sa3\")"};
    const std::string m_param_vec7_defaultValue = {""};
    const std::string m_param_vec8_defaultValue = {""};
    const std::string m_param_vec9_defaultValue = {""};

    std::string m_param_1 = {}; //This default value is autogenerated. It is highly recommended to provide a suggested value also for mandatory parameters.
    double m_param_2 = {std::nan("1")}; //This default value is autogenerated. It is highly recommended to provide a suggested value also for mandatory parameters.
    bool m_param_3 = {false}; //This default value is autogenerated. It is highly recommended to provide a suggested value also for mandatory parameters.
    int m_param_4 = {0}; //This default value is autogenerated. It is highly recommended to provide a suggested value also for mandatory parameters.
    char m_param_5 = {0}; //This default value is autogenerated. It is highly recommended to provide a suggested value also for mandatory parameters.
    size_t m_param_6 = {0}; //This default value is autogenerated. It is highly recommended to provide a suggested value also for mandatory parameters.
    float m_param_7 = {std::nanf("1")}; //This default value is autogenerated. It is highly recommended to provide a suggested value also for mandatory parameters.
    std::string m_group1_param_1 = {}; //This default value is autogenerated. It is highly recommended to provide a suggested value also for mandatory parameters.
    double m_group1_param_2 = {std::nan("1")}; //This default value is autogenerated. It is highly recommended to provide a suggested value also for mandatory parameters.
    bool m_group1_param_3 = {false}; //This default value is autogenerated. It is highly recommended to provide a suggested value also for mandatory parameters.
    int m_group1_param_4 = {0}; //This default value is autogenerated. It is highly recommended to provide a suggested value also for mandatory parameters.
    std::string m_group2_param_1 = {"def1"};
    double m_group2_param_2 = {1.0};
    bool m_group2_param_3 = {true};
    int m_group2_param_4 = {1000};
    std::string m_group3_param_1 = {"def1"};
    double m_group3_param_2 = {1.0};
    bool m_group3_param_3 = {true};
    int m_group3_param_4 = {1000};
    int m_group4_subgroup1_param_1 = {0};
    int m_group4_subgroup1_param_2 = {1};
    int m_group4_subgroup1_param_3 = {2};
    int m_group4_subgroup1_param_4 = {3};
    int m_group4_subgroup2_param_1 = {10};
    int m_group4_subgroup2_param_2 = {11};
    int m_group4_subgroup2_param_3 = {12};
    int m_group4_subgroup2_param_4 = {13};
    int m_group5_subgroup1_param_1 = {20};
    int m_group5_subgroup1_param_2 = {21};
    int m_group5_subgroup1_param_3 = {22};
    int m_group5_subgroup1_param_4 = {23};
    int m_group5_subgroup2_param_1 = {30};
    int m_group5_subgroup2_param_2 = {31};
    int m_group5_subgroup2_param_3 = {32};
    int m_group5_subgroup2_param_4 = {33};
    std::vector<int> m_param_vec1 = {}; //The default value of this list is an empty list. It is highly recommended to provide a suggested value also for optional string parameters.
    std::vector<double> m_param_vec2 = {}; //The default value of this list is an empty list. It is highly recommended to provide a suggested value also for optional string parameters.
    std::vector<std::string> m_param_vec3 = {}; //The default value of this list is an empty list. It is highly recommended to provide a suggested value also for optional string parameters.
    std::vector<int> m_param_vec4 = { }; //Default values for lists are managed in the class constructor. It is highly recommended to provide a suggested value also for optional string parameters.
    std::vector<double> m_param_vec5 = { }; //Default values for lists are managed in the class constructor. It is highly recommended to provide a suggested value also for optional string parameters.
    std::vector<std::string> m_param_vec6 = { }; //Default values for lists are managed in the class constructor. It is highly recommended to provide a suggested value also for optional string parameters.
    std::vector<int> m_param_vec7 = {}; //This default value is autogenerated. It is highly recommended to provide a suggested value also for mandatory parameters.
    std::vector<double> m_param_vec8 = {}; //This default value is autogenerated. It is highly recommended to provide a suggested value also for mandatory parameters.
    std::vector<std::string> m_param_vec9 = {}; //This default value is autogenerated. It is highly recommended to provide a suggested value also for mandatory parameters.

    bool          parseParams(const yarp::os::Searchable & config) override;
    std::string   getDeviceClassName() const override { return m_device_classname; }
    std::string   getDeviceName() const override { return m_device_name; }
    std::string   getDocumentationOfDeviceParams() const override;
    std::vector<std::string> getListOfParams() const override;
    bool getParamValue(const std::string& paramName, std::string& paramValue) const override;
    std::string   getConfiguration() const override;
};

#endif
