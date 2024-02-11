/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */


// Generated by yarpDeviceParamParserGenerator (1.0)
// This is an automatically generated file. Please do not edit it.
// It will be re-generated if the cmake flag ALLOW_DEVICE_PARAM_PARSER_GERNERATION is ON.

// Generated on: Sun Feb 11 10:36:33 2024


#ifndef FAKESERIALPORT_PARAMSPARSER_H
#define FAKESERIALPORT_PARAMSPARSER_H

#include <yarp/os/Searchable.h>
#include <yarp/dev/IDeviceDriverParams.h>
#include <string>
#include <cmath>

/**
* This class is the parameters parser for class FakeSerialPort.
*
* These are the used parameters:
* | Group name | Parameter name | Type   | Units | Default Value | Required | Description   | Notes                  |
* |:----------:|:--------------:|:------:|:-----:|:-------------:|:--------:|:-------------:|:----------------------:|
* | -          | period         | double | s     | 1.0           | 0        | thread period | optional, default 1.0s |
*
* The device can be launched by yarpdev using one of the following examples:
* \code{.unparsed}
* yarpdev --device FakeSerialPort --period 1.0
* \endcode
*
* \code{.unparsed}
* yarpdev --device FakeSerialPort
* \endcode
*
*/

class FakeSerialPort_ParamsParser : public yarp::dev::IDeviceDriverParams
{
public:
    FakeSerialPort_ParamsParser() = default;
    ~FakeSerialPort_ParamsParser() override = default;

public:
    const std::string m_device_type = {"FakeSerialPort"};
    bool m_parser_is_strict = false;
    struct parser_version_type
    {
         int major = 1;
         int minor = 0;
    };
    const parser_version_type m_parser_version = {};
    double m_period = {1.0};

    bool          parseParams(const yarp::os::Searchable & config) override;
    std::string   getDeviceType() const override { return m_device_type; }
    std::string   getDocumentationOfDeviceParams() const override;
    std::vector<std::string> getListOfParams() const override;
};

#endif