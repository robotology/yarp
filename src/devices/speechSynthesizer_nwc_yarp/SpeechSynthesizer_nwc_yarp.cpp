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
    m_local_name.clear();
    m_thrift_server_name.clear();

    if(!config.check("local"))
    {
        yCError(SPEECHSYNTH_NWC) << "No local name specified";

        return false;
    }
    m_local_name = config.find("local").asString();

    if(!config.check("remote"))
    {
        yCError(SPEECHSYNTH_NWC) << "No remote name specified";

        return false;
    }
    m_thrift_server_name = config.find("remote").asString();

    std::string thriftClientPortName = m_local_name + "/thrift:c";
    std::string thriftServerPortName = m_thrift_server_name + "/rpc";

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

bool SpeechSynthesizer_nwc_yarp::setLanguage(const std::string& language)
{
    if(!m_thriftClient.set_language(language))
    {
        yCError(SPEECHSYNTH_NWC) << "Error while setting language to" << language;
        return false;
    }

    return true;
}

bool SpeechSynthesizer_nwc_yarp::getLanguage(std::string& language)
{
    return_get_language result = m_thriftClient.get_language();
    if(!result.ret)
    {
        yCError(SPEECHSYNTH_NWC) << "Error while retrieving language";
        return false;
    }

    language = result.language;

    return true;
}

bool SpeechSynthesizer_nwc_yarp::setVoice(const std::string& voice_name)
{
    if(!m_thriftClient.set_voice(voice_name))
    {
        yCError(SPEECHSYNTH_NWC) << "Error while setting voice to" << voice_name;
        return false;
    }

    return true;
}

bool SpeechSynthesizer_nwc_yarp::getVoice(std::string& voice_name)
{
    return_get_voice result = m_thriftClient.get_voice();
    if(!result.ret)
    {
        yCError(SPEECHSYNTH_NWC) << "Error while retrieving the voice name";
        return false;
    }

    voice_name = result.voice;

    return true;
}

bool SpeechSynthesizer_nwc_yarp::setSpeed(const double speed)
{
    if(!m_thriftClient.set_speed(speed))
    {
        yCError(SPEECHSYNTH_NWC) << "Error while setting voice speed to" << speed;
        return false;
    }

    return true;
}

bool SpeechSynthesizer_nwc_yarp::getSpeed(double& speed)
{
    return_get_speed result = m_thriftClient.get_speed();
    if(!result.ret)
    {
        yCError(SPEECHSYNTH_NWC) << "Error while retrieving the voice speed";
        return false;
    }

    speed = result.speed;

    return true;
}

bool SpeechSynthesizer_nwc_yarp::setPitch(const double pitch)
{
    if(!m_thriftClient.set_pitch(pitch))
    {
        yCError(SPEECHSYNTH_NWC) << "Error while setting voice pitch to" << pitch;
        return false;
    }

    return true;
}

bool SpeechSynthesizer_nwc_yarp::getPitch(double& pitch)
{
    return_get_pitch result = m_thriftClient.get_pitch();
    if(!result.ret)
    {
        yCError(SPEECHSYNTH_NWC) << "Error while retrieving the voice pitch";
        return false;
    }

    pitch = result.pitch;

    return true;
}

bool SpeechSynthesizer_nwc_yarp::synthesize(const std::string& text, yarp::sig::Sound& sound)
{
    return_synthesize result = m_thriftClient.synthesize(text);
    if(!result.ret)
    {
        yCError(SPEECHSYNTH_NWC) << "Error while performing voice synthesis";
        return false;
    }
    sound = result.sound;

    return true;
}
