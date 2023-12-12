/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "audioToFileDeviceParams.h"
#include <yarp/os/LogStream.h>
#include <yarp/os/Value.h>

namespace {
    YARP_LOG_COMPONENT(AUDIOTOFILE, "yarp.device.audioToFileDevice")
}

bool audioToFileDevice_params::parseParams(const yarp::os::Searchable& config)
{
    if (config.check("help"))
    {
        yCInfo(AUDIOTOFILE, "Some examples:");
        yCInfo(AUDIOTOFILE, "yarpdev --device audioToFileDevice --help");
        yCInfo(AUDIOTOFILE, "yarpdev --device AudioPlayerWrapper --subdevice audioToFileDevice --start");
        yCInfo(AUDIOTOFILE, "yarpdev --device AudioPlayerWrapper --subdevice audioToFileDevice --start --file_name audio_out.wav --save_mode overwrite_file");
        yCInfo(AUDIOTOFILE, "save_mode can be overwrite_file, append_file, rename_file, break_file");
        yCInfo(AUDIOTOFILE, "use --add_marker option to add a marker at the beginning and at the ending of each received waveform");
        return false;
    }

    if (config.check("file_name"))
    {
        m_audio_filename = config.find("file_name").asString();
        yCInfo(AUDIOTOFILE) << "Audio will be saved on exit to file:" << m_audio_filename;
    }
    else
    {
        yCInfo(AUDIOTOFILE) << "No `file_name` option specified. Audio will be saved on exit to default file:" << m_audio_filename;
    }

    if (config.check("add_marker"))
    {
        m_add_marker = true;
        yCInfo(AUDIOTOFILE) << "`add_marker` option enabled";
    }
    else
    {
        yCInfo(AUDIOTOFILE) << "No `add_marker` option specified. Default is:" << m_add_marker;
    }

    if (config.check("save_mode"))
    {
        m_save_mode_s = config.find("save_mode").toString();
        yCInfo(AUDIOTOFILE) << "`save_mode` selected:" << m_save_mode_s;
    }
    else
    {
        yCInfo(AUDIOTOFILE) << "No `save_mode` option specified. Default is:" << m_audio_filename;
    }
    return true;
}

std::string audioToFileDevice_params::getDocumentationOfDeviceParams() const
{
 return std::string ("\
 Parameters required by this device are :\n\
 | Parameter name | SubParameter | Type      | Units     | Default Value  | Required | Description | Notes |\n\
 | : ---------- : | : -------- : | : ----- : | : ------: | : ---------- : | : ---- : | : ---------------------------------------------------------------- - : | : ---- - : |\n\
 | file_name      | -            | string    | -         | audio_out.wav  | No       | The name of the file written by the module | Only.wav and .mp3 files are supported |\n\
 | save_mode      | -            | string    | -         | overwrite_file | No       | Affects the behavior of the module and defines the save mode, as described in the documentation.   |       |\n\
 | add_marker     | -            | bool      | -         | -              | No       | If set, it will add a marker at the beginning and at the ending of each received waveform.   |       |\n\
 \n\
 Three different operating modes are available, defined by the optional string parameter `save_mode`:\n\
 if save_mode == append_data, the file is written only when the module terminates.\n\
 Every start / stop operation just pauses the module.On resume, the new data is concatenated at the end of the file.\n\
 \n\
 If save_mode == overwrite_file, the output file is written every time the stop() method is called or when the module terminates.\n\
 If the file already exists, it will be overwritten with the new data.\n\
 \n\
 If save_mode == rename_file, the output file is written to a NEW file every time the stop() method is called or when the module terminates.\n\
 The file name is modified, using an incremental counter appended at the end of the file name.\n\
 \n\
 If save_mode = break_file, the output file is written to a NEW file every time a yarp::sig::sound is received or when the module terminates.\n\
 The file name is modified, using an incremental counter appended at the end of the file name.\n\
");
}

std::vector<std::string> audioToFileDevice_params::getListOfParams() const
{
    std::vector<std::string> params;
    params.push_back("file_name");
    params.push_back("save_mode");
    params.push_back("add_marker");
    return params;
}
