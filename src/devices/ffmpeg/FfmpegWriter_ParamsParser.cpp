/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */


// Generated by yarpDeviceParamParserGenerator (1.0)
// This is an automatically generated file. Please do not edit it.
// It will be re-generated if the cmake flag ALLOW_DEVICE_PARAM_PARSER_GERNERATION is ON.

// Generated on: Sat Apr  6 12:05:03 2024


#include "FfmpegWriter_ParamsParser.h"
#include <yarp/os/LogStream.h>
#include <yarp/os/Value.h>

namespace {
    YARP_LOG_COMPONENT(FfmpegWriterParamsCOMPONENT, "yarp.device.FfmpegWriter")
}


FfmpegWriter_ParamsParser::FfmpegWriter_ParamsParser()
{
}


std::vector<std::string> FfmpegWriter_ParamsParser::getListOfParams() const
{
    std::vector<std::string> params;
    params.push_back("width");
    params.push_back("height");
    params.push_back("framerate");
    params.push_back("audio");
    params.push_back("channels");
    params.push_back("sample_rate");
    params.push_back("out");
    return params;
}


bool      FfmpegWriter_ParamsParser::parseParams(const yarp::os::Searchable & config)
{
    //Check for --help option
    if (config.check("help"))
    {
        yCInfo(FfmpegWriterParamsCOMPONENT) << getDocumentationOfDeviceParams();
    }

    std::string config_string = config.toString();
    yarp::os::Property prop_check(config_string.c_str());
    //Parser of parameter width
    {
        if (config.check("width"))
        {
            m_width = config.find("width").asInt64();
            yCInfo(FfmpegWriterParamsCOMPONENT) << "Parameter 'width' using value:" << m_width;
        }
        else
        {
            yCInfo(FfmpegWriterParamsCOMPONENT) << "Parameter 'width' using DEFAULT value:" << m_width;
        }
        prop_check.unput("width");
    }

    //Parser of parameter height
    {
        if (config.check("height"))
        {
            m_height = config.find("height").asInt64();
            yCInfo(FfmpegWriterParamsCOMPONENT) << "Parameter 'height' using value:" << m_height;
        }
        else
        {
            yCInfo(FfmpegWriterParamsCOMPONENT) << "Parameter 'height' using DEFAULT value:" << m_height;
        }
        prop_check.unput("height");
    }

    //Parser of parameter framerate
    {
        if (config.check("framerate"))
        {
            m_framerate = config.find("framerate").asInt64();
            yCInfo(FfmpegWriterParamsCOMPONENT) << "Parameter 'framerate' using value:" << m_framerate;
        }
        else
        {
            yCInfo(FfmpegWriterParamsCOMPONENT) << "Parameter 'framerate' using DEFAULT value:" << m_framerate;
        }
        prop_check.unput("framerate");
    }

    //Parser of parameter audio
    {
        if (config.check("audio"))
        {
            m_audio = config.find("audio").asBool();
            yCInfo(FfmpegWriterParamsCOMPONENT) << "Parameter 'audio' using value:" << m_audio;
        }
        else
        {
            yCInfo(FfmpegWriterParamsCOMPONENT) << "Parameter 'audio' using DEFAULT value:" << m_audio;
        }
        prop_check.unput("audio");
    }

    //Parser of parameter channels
    {
        if (config.check("channels"))
        {
            m_channels = config.find("channels").asInt64();
            yCInfo(FfmpegWriterParamsCOMPONENT) << "Parameter 'channels' using value:" << m_channels;
        }
        else
        {
            yCInfo(FfmpegWriterParamsCOMPONENT) << "Parameter 'channels' using DEFAULT value:" << m_channels;
        }
        prop_check.unput("channels");
    }

    //Parser of parameter sample_rate
    {
        if (config.check("sample_rate"))
        {
            m_sample_rate = config.find("sample_rate").asInt64();
            yCInfo(FfmpegWriterParamsCOMPONENT) << "Parameter 'sample_rate' using value:" << m_sample_rate;
        }
        else
        {
            yCInfo(FfmpegWriterParamsCOMPONENT) << "Parameter 'sample_rate' using DEFAULT value:" << m_sample_rate;
        }
        prop_check.unput("sample_rate");
    }

    //Parser of parameter out
    {
        if (config.check("out"))
        {
            m_out = config.find("out").asString();
            yCInfo(FfmpegWriterParamsCOMPONENT) << "Parameter 'out' using value:" << m_out;
        }
        else
        {
            yCInfo(FfmpegWriterParamsCOMPONENT) << "Parameter 'out' using DEFAULT value:" << m_out;
        }
        prop_check.unput("out");
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
                yCError(FfmpegWriterParamsCOMPONENT) << "User asking for parameter: "<<it->name <<" which is unknown to this parser!";
                extra_params_found = true;
            }
            else
            {
                yCWarning(FfmpegWriterParamsCOMPONENT) << "User asking for parameter: "<< it->name <<" which is unknown to this parser!";
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


std::string      FfmpegWriter_ParamsParser::getDocumentationOfDeviceParams() const
{
    std::string doc;
    doc = doc + std::string("\n=============================================\n");
    doc = doc + std::string("This is the help for device: FfmpegWriter\n");
    doc = doc + std::string("\n");
    doc = doc + std::string("This is the list of the parameters accepted by the device:\n");
    doc = doc + std::string("'width': width of image (must be even)\n");
    doc = doc + std::string("'height': height of image (must be even)\n");
    doc = doc + std::string("'framerate': baseline images per second\n");
    doc = doc + std::string("'audio': should audio be included?\n");
    doc = doc + std::string("'channels': number of audio channels\n");
    doc = doc + std::string("'sample_rate': audio samples per second\n");
    doc = doc + std::string("'out': name of movie to write\n");
    doc = doc + std::string("\n");
    doc = doc + std::string("Here are some examples of invocation command with yarpdev, with all params:\n");
    doc = doc + " yarpdev --device ffmpeg_writer --width 0 --height 0 --framerate 30 --audio false --channels 1 --sample_rate 44100 --out movie.avi\n";
    doc = doc + std::string("Using only mandatory params:\n");
    doc = doc + " yarpdev --device ffmpeg_writer\n";
    doc = doc + std::string("=============================================\n\n");    return doc;
}