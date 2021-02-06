/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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

typedef unsigned short int audio_sample_16t;

audioToFileDevice::audioToFileDevice() :
        m_audio_filename("audio_out")
{
}

audioToFileDevice::~audioToFileDevice()
{
    close();
}

bool audioToFileDevice::open(yarp::os::Searchable &config)
{
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
    if (m_sounds.size() == 0) return;

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
    size_t lastindex = m_audio_filename.find(".wav");
    std::string rawname = m_audio_filename.substr(0, lastindex);

    if (m_save_mode == save_mode_t::save_rename_file)
    {
        rawname = rawname +std::to_string(m_filename_counter++);
    }
    rawname = rawname +".wav";
    bool ok = yarp::sig::file::write(m_audioFile, rawname.c_str());
    if (ok)
    {
        yCDebug(AUDIOTOFILE) << "Wrote audio to:" << rawname;
    }
}

bool audioToFileDevice::close()
{
    save_to_file();
    return true;
}

bool audioToFileDevice::startPlayback()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    yCDebug(AUDIOTOFILE) << "start";
    m_playback_running = true;
    if (m_save_mode != save_mode_t::save_append_data)
    {
        m_sounds.clear();
    }
    return true;
}

bool audioToFileDevice::stopPlayback()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    yCDebug(AUDIOTOFILE) << "stop";
    m_playback_running = false;
    if (m_save_mode != save_mode_t::save_append_data)
    {
        save_to_file();
    }
    return true;
}

bool audioToFileDevice::renderSound(const yarp::sig::Sound& sound)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_playback_running)
    {
        m_sounds.push_back(sound);
    }
    return true;
}

bool audioToFileDevice::setHWGain(double gain)
{
    yCError(AUDIOTOFILE, "Not yet implemented");
    return false;
}
