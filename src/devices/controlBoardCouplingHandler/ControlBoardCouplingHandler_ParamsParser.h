/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */


// Generated by yarpDeviceParamParserGenerator (1.0)
// This is an automatically generated file. Please do not edit it.
// It will be re-generated if the cmake flag ALLOW_DEVICE_PARAM_PARSER_GERNERATION is ON.

// Generated on: Wed Mar  6 13:18:29 2024


#ifndef CONTROLBOARDCOUPLINGHANDLER_PARAMSPARSER_H
#define CONTROLBOARDCOUPLINGHANDLER_PARAMSPARSER_H

#include <yarp/os/Searchable.h>
#include <yarp/dev/IDeviceDriverParams.h>
#include <string>
#include <cmath>

/**
* This class is the parameters parser for class ControlBoardCouplingHandler.
*
* These are the used parameters:
* | Group name | Parameter name  | Type   | Units | Default Value | Required | Description                                  | Notes |
* |:----------:|:---------------:|:------:|:-----:|:-------------:|:--------:|:--------------------------------------------:|:-----:|
* | -          | coupling_device | string | -     | -             | 1        | Name of the device that handles the coupling | -     |
*
* The device can be launched by yarpdev using one of the following examples:
* \code{.unparsed}
* yarpdev --device controlBoardCouplingHandler --coupling_device <mandatory_value>
* \endcode
*
* \code{.unparsed}
* yarpdev --device controlBoardCouplingHandler --coupling_device <mandatory_value>
* \endcode
*
*/

class ControlBoardCouplingHandler_ParamsParser : public yarp::dev::IDeviceDriverParams
{
public:
    ControlBoardCouplingHandler_ParamsParser();
    ~ControlBoardCouplingHandler_ParamsParser() override = default;

public:
    const std::string m_device_classname = {"ControlBoardCouplingHandler"};
    const std::string m_device_name = {"controlBoardCouplingHandler"};
    bool m_parser_is_strict = false;
    struct parser_version_type
    {
         int major = 1;
         int minor = 0;
    };
    const parser_version_type m_parser_version = {};

    const std::string m_coupling_device_defaultValue = {""};

    std::string m_coupling_device = {}; //This default value is autogenerated. It is highly recommended to provide a suggested value also for mandatory parameters.

    bool          parseParams(const yarp::os::Searchable & config) override;
    std::string   getDeviceClassName() const override { return m_device_classname; }
    std::string   getDeviceName() const override { return m_device_name; }
    std::string   getDocumentationOfDeviceParams() const override;
    std::vector<std::string> getListOfParams() const override;
};

#endif