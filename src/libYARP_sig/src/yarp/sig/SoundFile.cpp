/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/SoundFile.h>
#include <yarp/sig/SoundFileWav.h>
#include <yarp/sig/SoundFileMp3.h>

#include <yarp/conf/system.h>

#include <yarp/os/NetInt16.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/Vocab.h>

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

#include <cstdio>
#include <cstring>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::file;

namespace
{
    YARP_LOG_COMPONENT(SOUNDFILE, "yarp.sig.SoundFile")
}

//#######################################################################################################

bool yarp::sig::file::read(Sound& data, const char* filename)
{
    const char* file_ext = strrchr(filename, '.');
    if (file_ext == nullptr)
    {
        yCError(SOUNDFILE) << "cannot find file extension in file name";
        return false;
    }

    if (strcmp(file_ext, ".wav") == 0)
    {
        return read_wav_file(data, filename);
    }
    else if (strcmp(file_ext, ".mp3") == 0)
    {
        return read_mp3_file(data, filename);
    }
    yCError(SOUNDFILE) << "Unknown file format";
    return false;
}

bool yarp::sig::file::read_bytestream(Sound& data, const char* bytestream, size_t streamsize, std::string format)
{
    if (strcmp(format.c_str(), ".wav") == 0)
    {
        return read_wav_bytestream(data, bytestream);
    }
    else if (strcmp(format.c_str(), ".mp3") == 0)
    {
        return read_mp3_bytestream(data, bytestream, streamsize);
    }
    yCError(SOUNDFILE) << "Unknown file format";
    return false;
}

bool yarp::sig::file::write(const Sound& sound_data, const char* filename)
{
    const char* file_ext = strrchr(filename, '.');
    if (file_ext == nullptr)
    {
        yCError(SOUNDFILE) << "cannot find file extension in file name";
        return false;
    }

    if (strcmp(file_ext, ".wav") == 0)
    {
        return write_wav_file(sound_data, filename);
    }
    else if (strcmp(file_ext, ".mp3") == 0)
    {
        return write_mp3_file(sound_data, filename);
    }

    yCError(SOUNDFILE) << "Unknown file format";
    return false;
}

//#######################################################################################################

bool yarp::sig::file::soundStreamReader::open(const char *filename)
{
    bool b  = yarp::sig::file::read(m_sound_data,filename);
    if (!b)
    {
        yCError(SOUNDFILE, "Unable to read data from file %s", filename);
        return false;
    }

    m_index=0;
    m_totsize = m_sound_data.getSamples();
    return true;
}

bool yarp::sig::file::soundStreamReader::close()
{
    m_sound_data.resize(0,0);
    m_index = 0;
    m_totsize = 0;
    return true;
}

size_t yarp::sig::file::soundStreamReader::readBlock(Sound& dest, size_t block_size)
{
    if (m_totsize == 0)
    {
        yCError(SOUNDFILE, "File is not open yet");
        return false;
    }

    if (m_index+block_size>m_totsize) {block_size= m_totsize-m_index;}
    dest = m_sound_data.subSound(m_index,m_index+block_size);
    m_index += block_size;
    return block_size;
}

bool  yarp::sig::file::soundStreamReader::rewind(size_t sample_offset)
{
    if (m_totsize == 0)
    {
        yCError(SOUNDFILE, "File is not open yet");
        return false;
    }
    m_index=sample_offset;
    return true;
}

size_t yarp::sig::file::soundStreamReader::getIndex()
{
    if (m_totsize == 0)
    {
        yCError(SOUNDFILE, "File is not open yet");
        return false;
    }
    return m_index;
}
