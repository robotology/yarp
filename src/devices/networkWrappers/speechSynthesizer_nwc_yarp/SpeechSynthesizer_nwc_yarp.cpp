/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "SpeechSynthesizer_nwc_yarp.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Os.h>

namespace {
YARP_LOG_COMPONENT(SPEECHSYNTH_NWC, "yarp.devices.speechSynthesizer_nwc_yarp")
}

using namespace yarp::dev;

SpeechSynthesizer_nwc_yarp::~SpeechSynthesizer_nwc_yarp()
{
    closeMain();
}

bool SpeechSynthesizer_nwc_yarp::close()
{
    return closeMain();
}

bool SpeechSynthesizer_nwc_yarp::open(yarp::os::Searchable& config)
{
    if (!parseParams(config)) { return false; }

    std::string thriftClientPortName = m_local + "/thrift:c";
    std::string thriftServerPortName = m_remote + "/rpc";

    if(!m_thriftClientPort.open(thriftClientPortName))
    {
        yCError(SPEECHSYNTH_NWC) << "open() failed: could not open" << thriftClientPortName <<"check the network";

        return false;
    }

    if(!yarp::os::Network::connect(thriftClientPortName, thriftServerPortName))
    {
        yCError(SPEECHSYNTH_NWC) << "Network::connect() failed: could not connect" << thriftClientPortName << "with" << thriftServerPortName;

        return false;
    }

    if(!m_thriftClient.yarp().attachAsClient(m_thriftClientPort))
    {
        yCError(SPEECHSYNTH_NWC) << "Error! Cannot attach the m_thriftClientPort (" << thriftClientPortName << ") port as a client";

        return false;
    }

    return true;
}

bool SpeechSynthesizer_nwc_yarp::closeMain()
{
    // Close thrift client port
    m_thriftClientPort.close();

    return true;
}

ReturnValue SpeechSynthesizer_nwc_yarp::setLanguage(const std::string& language)
{
    auto result = m_thriftClient.set_language(language);
    if(!result.ret)
    {
        yCError(SPEECHSYNTH_NWC) << "Error while setting language to" << language;
        return result.ret;
    }

    return result.ret;
}

ReturnValue SpeechSynthesizer_nwc_yarp::getLanguage(std::string& language)
{
    auto result = m_thriftClient.get_language();
    if(!result.ret)
    {
        yCError(SPEECHSYNTH_NWC) << "Error while retrieving language";
        return result.ret;
    }

    language = result.language;

    return result.ret;
}

ReturnValue SpeechSynthesizer_nwc_yarp::setVoice(const std::string& voice_name)
{
    auto result = m_thriftClient.set_voice(voice_name);
    if(!result.ret)
    {
        yCError(SPEECHSYNTH_NWC) << "Error while setting voice to" << voice_name;
        return result.ret;
    }

    return result.ret;
}

ReturnValue SpeechSynthesizer_nwc_yarp::getVoice(std::string& voice_name)
{
    auto result = m_thriftClient.get_voice();
    if(!result.ret)
    {
        yCError(SPEECHSYNTH_NWC) << "Error while retrieving the voice name";
        return result.ret;
    }

    voice_name = result.voice;

    return result.ret;
}

ReturnValue SpeechSynthesizer_nwc_yarp::setSpeed(const double speed)
{
    auto result = m_thriftClient.set_speed(speed);
    if(!result.ret)
    {
        yCError(SPEECHSYNTH_NWC) << "Error while setting voice speed to" << speed;
        return result.ret;
    }

    return result.ret;
}

ReturnValue SpeechSynthesizer_nwc_yarp::getSpeed(double& speed)
{
    auto result = m_thriftClient.get_speed();
    if(!result.ret)
    {
        yCError(SPEECHSYNTH_NWC) << "Error while retrieving the voice speed";
        return result.ret;
    }

    speed = result.speed;

    return result.ret;
}

ReturnValue SpeechSynthesizer_nwc_yarp::setPitch(const double pitch)
{
    auto result = m_thriftClient.set_pitch(pitch);
    if(!result.ret)
    {
        yCError(SPEECHSYNTH_NWC) << "Error while setting voice pitch to" << pitch;
        return result.ret;
    }

    return result.ret;
}

ReturnValue SpeechSynthesizer_nwc_yarp::getPitch(double& pitch)
{
    auto result = m_thriftClient.get_pitch();
    if(!result.ret)
    {
        yCError(SPEECHSYNTH_NWC) << "Error while retrieving the voice pitch";
        return result.ret;
    }

    pitch = result.pitch;

    return result.ret;
}

ReturnValue SpeechSynthesizer_nwc_yarp::synthesize(const std::string& text, yarp::sig::Sound& sound)
{
    auto result = m_thriftClient.synthesize(text);
    if(!result.ret)
    {
        yCError(SPEECHSYNTH_NWC) << "Error while performing voice synthesis";
        return result.ret;
    }
    sound = result.sound;

    return result.ret;
}
