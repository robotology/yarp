/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "audioToFileDevice.h"

#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

#include <mutex>
#include <string>


using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(AUDIOTOFILE, "yarp.device.audioToFileDevice")
}

audioToFileDevice::audioToFileDevice() :
        m_audio_filename("audio_out.wav")
{
}

audioToFileDevice::~audioToFileDevice()
{
    close();
}

bool audioToFileDevice::open(yarp::os::Searchable &config)
{
    if (config.check("help"))
    {
        yCInfo(AUDIOTOFILE, "Some examples:");
        yCInfo(AUDIOTOFILE, "yarpdev --device audioToFileDevice --help");
        yCInfo(AUDIOTOFILE, "yarpdev --device AudioPlayerWrapper --subdevice audioToFileDevice --start");
        yCInfo(AUDIOTOFILE, "yarpdev --device AudioPlayerWrapper --subdevice audioToFileDevice --start --audio_out.wav --save_mode overwrite_file");
        return false;
    }

    bool b = configurePlayerAudioDevice(config.findGroup("AUDIO_BASE"), "audioToFileDevice");
    if (!b) { return false; }

    if (config.check("file_name"))
    {
        m_audio_filename=config.find("file_name").asString();
        yCInfo(AUDIOTOFILE) << "Audio will be saved on exit to file:" << m_audio_filename;
        return true;
    }
    else
    {
        yCInfo(AUDIOTOFILE) << "No `file_name` option specified. Audio will be saved on exit to default file:" << m_audio_filename;
    }

    if      (config.find("save_mode").toString() == "overwrite_file") { m_save_mode = save_mode_t::save_overwrite_file;}
    else if (config.find("save_mode").toString() == "append_data")    { m_save_mode = save_mode_t::save_append_data; }
    else if (config.find("save_mode").toString() == "rename_file")    { m_save_mode = save_mode_t::save_rename_file; }
    else if (config.check("save_mode")) {yError() << "Unsupported value for save_mode parameter"; return false;}

    if      (m_save_mode == save_mode_t::save_overwrite_file) { yCInfo(AUDIOTOFILE) << "overwrite_file mode selected. File will be saved both on exit and on stop"; }
    else if (m_save_mode == save_mode_t::save_append_data)    { yCInfo(AUDIOTOFILE) << "append_data mode selected. File will be saved on exit only"; }
    else if (m_save_mode == save_mode_t::save_rename_file)    { yCInfo(AUDIOTOFILE) << "rename_file mode selected. File will be saved both on exit and on stop"; }

    return true;
}

void audioToFileDevice::save_to_file()
{
    //of the buffer is empty, there is nothing to save
    if (m_sounds.size() == 0) {
        return;
    }

    //we need to set the number of channels and the frequency before calling the
    //concatenation operator
    m_audioFile.setFrequency(m_sounds.front().getFrequency());
    m_audioFile.resize(0, m_sounds.front().getChannels());
    while (!m_sounds.empty())
    {
        m_audioFile += m_sounds.front();
        m_sounds.pop_front();
    }

    //remove the extension .wav from the filename
    size_t lastindex = m_audio_filename.find_last_of(".");
    std::string trunc_filename  = m_audio_filename.substr(0, lastindex);
    std::string trunc_extension =".wav";
    if (lastindex!= std::string::npos)
    {
        trunc_extension = m_audio_filename.substr(lastindex, std::string::npos);
    }

    if (m_save_mode == save_mode_t::save_rename_file)
    {
        trunc_filename = trunc_filename +std::to_string(m_filename_counter++);
    }

    std::string complete_filename = trunc_filename + trunc_extension;
    bool ok = yarp::sig::file::write(m_audioFile, complete_filename.c_str());
    if (ok)
    {
        yCDebug(AUDIOTOFILE) << "Wrote audio to:" << complete_filename;
    }
}

bool audioToFileDevice::close()
{
    save_to_file();
    return true;
}

bool audioToFileDevice::startPlayback()
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    yCDebug(AUDIOTOFILE) << "start";
    m_playback_enabled = true;
    if (m_save_mode != save_mode_t::save_append_data)
    {
        m_sounds.clear();
    }
    return true;
}

bool audioToFileDevice::stopPlayback()
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    yCDebug(AUDIOTOFILE) << "stop";
    m_playback_enabled = false;
    if (m_save_mode != save_mode_t::save_append_data)
    {
        save_to_file();
    }
    return true;
}

bool audioToFileDevice::renderSound(const yarp::sig::Sound& sound)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    if (m_playback_enabled)
    {
        m_sounds.push_back(sound);
    }
    return true;
}

bool audioToFileDevice::setHWGain(double gain)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    if (gain > 0)
    {
        m_hw_gain = gain;
        return true;
    }
    return false;
}

bool audioToFileDevice::configureDeviceAndStart()
{
    yCError(AUDIOTOFILE, "configureDeviceAndStart() Not yet implemented");
    return true;
}

bool audioToFileDevice::interruptDeviceAndClose()
{
    yCError(AUDIOTOFILE, "interruptDeviceAndClose() Not yet implemented");
    return true;
}

void audioToFileDevice::waitUntilPlaybackStreamIsComplete()
{
    yCError(AUDIOTOFILE, "waitUntilPlaybackStreamIsComplete() Not yet implemented");
    return;
}
