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
