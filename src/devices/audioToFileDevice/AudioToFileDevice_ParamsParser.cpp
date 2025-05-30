/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */


// Generated by yarpDeviceParamParserGenerator (2.0)
// This is an automatically generated file. Please do not edit it.
// It will be re-generated if the cmake flag ALLOW_DEVICE_PARAM_PARSER_GERNERATION is ON.

// Generated on: Thu May 22 11:32:45 2025


#include "AudioToFileDevice_ParamsParser.h"
#include <yarp/os/LogStream.h>
#include <yarp/os/Value.h>

namespace {
    YARP_LOG_COMPONENT(AudioToFileDeviceParamsCOMPONENT, "yarp.device.AudioToFileDevice")
}


AudioToFileDevice_ParamsParser::AudioToFileDevice_ParamsParser()
{
}


std::vector<std::string> AudioToFileDevice_ParamsParser::getListOfParams() const
{
    std::vector<std::string> params;
    params.push_back("file_name");
    params.push_back("save_mode");
    params.push_back("add_marker");
    return params;
}


bool AudioToFileDevice_ParamsParser::getParamValue(const std::string& paramName, std::string& paramValue) const
{
    if (paramName =="file_name")
    {
        paramValue = m_file_name;
        return true;
    }
    if (paramName =="save_mode")
    {
        paramValue = m_save_mode;
        return true;
    }
    if (paramName =="add_marker")
    {
        if (m_add_marker==true) paramValue = "true";
        else paramValue = "false";
        return true;
    }

    yError() <<"parameter '" << paramName << "' was not found";
    return false;

}


std::string AudioToFileDevice_ParamsParser::getConfiguration() const
{
    //This is a sub-optimal solution.
    //Ideally getConfiguration() should return all parameters but it is currently
    //returning only user provided parameters (excluding default values)
    //This behaviour will be fixed in the near future.
    std::string s_cfg = m_provided_configuration;
    return s_cfg;
}

bool      AudioToFileDevice_ParamsParser::parseParams(const yarp::os::Searchable & config)
{
    //Check for --help option
    if (config.check("help"))
    {
        yCInfo(AudioToFileDeviceParamsCOMPONENT) << getDocumentationOfDeviceParams();
    }

    m_provided_configuration = config.toString();
    yarp::os::Property prop_check(m_provided_configuration.c_str());
    //Parser of parameter file_name
    {
        if (config.check("file_name"))
        {
            m_file_name = config.find("file_name").asString();
            yCInfo(AudioToFileDeviceParamsCOMPONENT) << "Parameter 'file_name' using value:" << m_file_name;
        }
        else
        {
            yCInfo(AudioToFileDeviceParamsCOMPONENT) << "Parameter 'file_name' using DEFAULT value:" << m_file_name;
        }
        prop_check.unput("file_name");
    }

    //Parser of parameter save_mode
    {
        if (config.check("save_mode"))
        {
            m_save_mode = config.find("save_mode").asString();
            yCInfo(AudioToFileDeviceParamsCOMPONENT) << "Parameter 'save_mode' using value:" << m_save_mode;
        }
        else
        {
            yCInfo(AudioToFileDeviceParamsCOMPONENT) << "Parameter 'save_mode' using DEFAULT value:" << m_save_mode;
        }
        prop_check.unput("save_mode");
    }

    //Parser of parameter add_marker
    {
        if (config.check("add_marker"))
        {
            m_add_marker = config.find("add_marker").asBool();
            yCInfo(AudioToFileDeviceParamsCOMPONENT) << "Parameter 'add_marker' using value:" << m_add_marker;
        }
        else
        {
            yCInfo(AudioToFileDeviceParamsCOMPONENT) << "Parameter 'add_marker' using DEFAULT value:" << m_add_marker;
        }
        prop_check.unput("add_marker");
    }

    /*
    //This code check if the user set some parameter which are not check by the parser
    //If the parser is set in strict mode, this will generate an error
    if (prop_check.size() > 0)
    {
        bool extra_params_found = false;
        for (auto it=prop_check.begin(); it!=prop_check.end(); it++)
        {
            if (m_parser_is_strict)
            {
                yCError(AudioToFileDeviceParamsCOMPONENT) << "User asking for parameter: "<<it->name <<" which is unknown to this parser!";
                extra_params_found = true;
            }
            else
            {
                yCWarning(AudioToFileDeviceParamsCOMPONENT) << "User asking for parameter: "<< it->name <<" which is unknown to this parser!";
            }
        }

       if (m_parser_is_strict && extra_params_found)
       {
           return false;
       }
    }
    */
    return true;
}


std::string      AudioToFileDevice_ParamsParser::getDocumentationOfDeviceParams() const
{
    std::string doc;
    doc = doc + std::string("\n=============================================\n");
    doc = doc + std::string("This is the help for device: AudioToFileDevice\n");
    doc = doc + std::string("\n");
    doc = doc + std::string("This is the list of the parameters accepted by the device:\n");
    doc = doc + std::string("'file_name': The name of the file written by the module\n");
    doc = doc + std::string("'save_mode': Affects the behavior of the module and defines the save mode, as described in the documentation.\n");
    doc = doc + std::string("'add_marker': If set, it will add a marker at the beginning and at the ending of each received waveform.\n");
    doc = doc + std::string("\n");
    doc = doc + std::string("Here are some examples of invocation command with yarpdev, with all params:\n");
    doc = doc + " yarpdev --device audioToFileDevice --file_name audio_out.wav --save_mode overwrite_file --add_marker false\n";
    doc = doc + std::string("Using only mandatory params:\n");
    doc = doc + " yarpdev --device audioToFileDevice\n";
    doc = doc + std::string("=============================================\n\n");    return doc;
}
