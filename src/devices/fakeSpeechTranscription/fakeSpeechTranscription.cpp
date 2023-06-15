/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fakeSpeechTranscription.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/LogComponent.h>

#include <cstdio>
#include <cstdlib>

using namespace yarp::os;
using namespace yarp::dev;

namespace {
YARP_LOG_COMPONENT(FAKE_SPEECHTR, "yarp.device.FakeSpeechTranscription")
}

FakeSpeechTranscription::FakeSpeechTranscription()
{
}

FakeSpeechTranscription::~FakeSpeechTranscription()
{
    close();
}

bool FakeSpeechTranscription::open(yarp::os::Searchable& config)
{
    return true;
}

bool FakeSpeechTranscription::close()
{
    return true;
}

bool FakeSpeechTranscription::setLanguage(const std::string language)
{
    m_language=language;
    yCInfo(FAKE_SPEECHTR) << "Language set to" << language;
    return true;
}

bool FakeSpeechTranscription::getLanguage(std::string& language)
{
    language = m_language;
    return true;
}

bool FakeSpeechTranscription::transcribe(const yarp::sig::Sound& sound, std::string& transcription, double& score)
{
    if (sound.getSamples() == 0 ||
        sound.getChannels() == 0)
    {
        yCError(FAKE_SPEECHTR) << "Invalid Sound sample received";
        transcription = "";
        score = 0.0;
        return false;
    }

    transcription = "hello world";
    score = 1.0;
    return true;
}
