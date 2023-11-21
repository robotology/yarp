/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FakeSpeechSynthesizer.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/LogComponent.h>

#include <cstdio>
#include <cstdlib>

using namespace yarp::os;
using namespace yarp::dev;

namespace {
YARP_LOG_COMPONENT(FAKE_SPEECHSYN, "yarp.device.FakeSpeechSynthesizer")
}

FakeSpeechSynthesizer::FakeSpeechSynthesizer()
{
}

FakeSpeechSynthesizer::~FakeSpeechSynthesizer()
{
    close();
}

bool FakeSpeechSynthesizer::open(yarp::os::Searchable& config)
{
    if (!this->parseParams(config)) {return false;}

    return true;
}

bool FakeSpeechSynthesizer::close()
{
    return true;
}

yarp_ret_value FakeSpeechSynthesizer::setLanguage(const std::string& language)
{
    m_language=language;
    yCInfo(FAKE_SPEECHSYN) << "Language set to" << language;
    return yarp_ret_value_ok;
}

yarp_ret_value FakeSpeechSynthesizer::getLanguage(std::string& language)
{
    language = m_language;
    return yarp_ret_value_ok;
}

yarp_ret_value FakeSpeechSynthesizer::setVoice(const std::string& voice)
{
    m_voice = voice;
    yCInfo(FAKE_SPEECHSYN) << "Voice set to" << voice;
    return yarp_ret_value_ok;
}

yarp_ret_value FakeSpeechSynthesizer::getVoice(std::string& voice)
{
    voice = m_voice;
    return yarp_ret_value_ok;
}

yarp_ret_value FakeSpeechSynthesizer::setSpeed(const double speed)
{
    m_speed = speed;
    yCInfo(FAKE_SPEECHSYN) << "Speed set to" << speed;
    return yarp_ret_value_ok;
}

yarp_ret_value FakeSpeechSynthesizer::getSpeed(double& speed)
{
    speed = m_speed;
    return yarp_ret_value_ok;
}

yarp_ret_value FakeSpeechSynthesizer::setPitch(const double pitch)
{
    m_pitch = pitch;
    yCInfo(FAKE_SPEECHSYN) << "Pitch set to" << pitch;
    return yarp_ret_value_ok;
}

yarp_ret_value FakeSpeechSynthesizer::getPitch(double& pitch)
{
    pitch = m_pitch;
    return yarp_ret_value_ok;
}

yarp_ret_value FakeSpeechSynthesizer::synthesize(const std::string& text, yarp::sig::Sound& sound)
{
    if (text == "")
    {
        yCError(FAKE_SPEECHSYN) << "Text is empty";
        return yarp_ret_value::return_code::return_value_error_method_failed;
    }

    sound.resize(100,2);

    return yarp_ret_value_ok;
}
