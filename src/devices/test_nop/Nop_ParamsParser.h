/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */


// Generated by yarpDeviceParamParserGenerator (2.0)
// This is an automatically generated file. Please do not edit it.
// It will be re-generated if the cmake flag ALLOW_DEVICE_PARAM_PARSER_GERNERATION is ON.

// Generated on: Thu May 22 11:57:34 2025


#ifndef NOP_PARAMSPARSER_H
#define NOP_PARAMSPARSER_H

#include <yarp/os/Searchable.h>
#include <yarp/dev/IDeviceDriverParams.h>
#include <string>
#include <cmath>

/**
* This class is the parameters parser for class Nop.
*
* These are the used parameters:
* | Group name | Parameter name | Type  | Units | Default Value | Required | Description          | Notes |
* |:----------:|:--------------:|:-----:|:-----:|:-------------:|:--------:|:--------------------:|:-----:|
* | -          | period         | float | s     | 1             | 0        | period of the thread | -     |
*
* The device can be launched by yarpdev using one of the following examples (with and without all optional parameters):
* \code{.unparsed}
* yarpdev --device test_nop --period 1
* \endcode
*
* \code{.unparsed}
* yarpdev --device test_nop
* \endcode
*
*/

class Nop_ParamsParser : public yarp::dev::IDeviceDriverParams
{
public:
    Nop_ParamsParser();
    ~Nop_ParamsParser() override = default;

public:
    const std::string m_device_classname = {"Nop"};
    const std::string m_device_name = {"test_nop"};
    bool m_parser_is_strict = false;
    struct parser_version_type
    {
         int major = 2;
         int minor = 0;
    };
    const parser_version_type m_parser_version = {};

    std::string m_provided_configuration;

    const std::string m_period_defaultValue = {"1"};

    float m_period = {1};

    bool          parseParams(const yarp::os::Searchable & config) override;
    std::string   getDeviceClassName() const override { return m_device_classname; }
    std::string   getDeviceName() const override { return m_device_name; }
    std::string   getDocumentationOfDeviceParams() const override;
    std::vector<std::string> getListOfParams() const override;
    bool getParamValue(const std::string& paramName, std::string& paramValue) const override;
    std::string   getConfiguration() const override;
};

#endif
