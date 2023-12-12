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

audioToFileDevice::audioToFileDevice()
{
}

audioToFileDevice::~audioToFileDevice()
{
    close();
}

bool audioToFileDevice::open(yarp::os::Searchable &config)
{
    bool b = false;
    b = parseParams(config);
    if (!b) {return false;}

    b = configurePlayerAudioDevice(config.findGroup("AUDIO_BASE"), "audioToFileDevice");
    if (!b) { return false; }

    if      (m_save_mode_s == "overwrite_file") { m_save_mode = save_mode_t::save_overwrite_file; yCInfo(AUDIOTOFILE) << "overwrite_file mode selected. File will be saved both on exit and on stop";}
    else if (m_save_mode_s == "append_data")    { m_save_mode = save_mode_t::save_append_data; yCInfo(AUDIOTOFILE) << "append_data mode selected. File will be saved on exit only";}
    else if (m_save_mode_s == "rename_file")    { m_save_mode = save_mode_t::save_rename_file; yCInfo(AUDIOTOFILE) << "rename_file mode selected. File will be saved both on exit and on stop";}
    else if (m_save_mode_s == "break_file")     { m_save_mode = save_mode_t::save_break_file;  yCInfo(AUDIOTOFILE) << "break_file mode selected.";}
    else                                        { yError() << "Unsupported value for save_mode parameter"; return false; }

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
        yarp::sig::Sound curr_sound = m_sounds.front();
        size_t ss_size = curr_sound.getSamples();
        size_t ch_size = curr_sound.getChannels();

        if (!m_add_marker)
        {
            m_audioFile += curr_sound;
        }
        else
        {
            //if required, create a sound with a marker at the beginning and at the end
            yarp::sig::Sound marked_sound;
            marked_sound.setFrequency(curr_sound.getFrequency());
            marked_sound.resize(ss_size + 5, ch_size);

            for (size_t c = 0; c < ch_size; c++)
            {
                for (size_t i = 0; i < ss_size; i++)
                {
                    marked_sound.set(curr_sound.get(i, c), i, c);
                }
                for (size_t i = ss_size; i < ss_size + 5; i++)
                {
                    marked_sound.set(32000, i, c);
                }
                marked_sound.set(-32000, 0, c);
            }

            m_audioFile += marked_sound;
        }
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

    if (m_save_mode == save_mode_t::save_rename_file ||
        m_save_mode == save_mode_t::save_break_file)
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
    if (m_save_mode == save_break_file)
    {
       m_sounds.push_back(sound);
       save_to_file();
       return true;
    }
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
