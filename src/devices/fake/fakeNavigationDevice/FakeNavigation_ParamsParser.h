/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */


// Generated by yarpDeviceParamParserGenerator (2.0)
// This is an automatically generated file. Please do not edit it.
// It will be re-generated if the cmake flag ALLOW_DEVICE_PARAM_PARSER_GERNERATION is ON.

// Generated on: Thu May 22 11:32:34 2025


#ifndef FAKENAVIGATION_PARAMSPARSER_H
#define FAKENAVIGATION_PARAMSPARSER_H

#include <yarp/os/Searchable.h>
#include <yarp/dev/IDeviceDriverParams.h>
#include <string>
#include <cmath>

/**
* This class is the parameters parser for class FakeNavigation.
*
* These are the used parameters:
* | Group name | Parameter name  | Type | Units | Default Value | Required | Description     | Notes |
* |:----------:|:---------------:|:----:|:-----:|:-------------:|:--------:|:---------------:|:-----:|
* | -          | navigation_time | int  | -     | 500           | 0        | navigation_time | -     |
* | -          | reached_time    | int  | -     | 100           | 0        | reached_time    | -     |
*
* The device can be launched by yarpdev using one of the following examples (with and without all optional parameters):
* \code{.unparsed}
* yarpdev --device fakeNavigation --navigation_time 500 --reached_time 100
* \endcode
*
* \code{.unparsed}
* yarpdev --device fakeNavigation
* \endcode
*
*/

class FakeNavigation_ParamsParser : public yarp::dev::IDeviceDriverParams
{
public:
    FakeNavigation_ParamsParser();
    ~FakeNavigation_ParamsParser() override = default;

public:
    const std::string m_device_classname = {"FakeNavigation"};
    const std::string m_device_name = {"fakeNavigation"};
    bool m_parser_is_strict = false;
    struct parser_version_type
    {
         int major = 2;
         int minor = 0;
    };
    const parser_version_type m_parser_version = {};

    std::string m_provided_configuration;

    const std::string m_navigation_time_defaultValue = {"500"};
    const std::string m_reached_time_defaultValue = {"100"};

    int m_navigation_time = {500};
    int m_reached_time = {100};

    bool          parseParams(const yarp::os::Searchable & config) override;
    std::string   getDeviceClassName() const override { return m_device_classname; }
    std::string   getDeviceName() const override { return m_device_name; }
    std::string   getDocumentationOfDeviceParams() const override;
    std::vector<std::string> getListOfParams() const override;
    bool getParamValue(const std::string& paramName, std::string& paramValue) const override;
    std::string   getConfiguration() const override;
};

#endif
