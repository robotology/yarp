/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "SpeechTranscription_nwc_yarp.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Os.h>

namespace {
YARP_LOG_COMPONENT(SPEECHTR_NWC, "yarp.devices.speechTranscription_nwc_yarp")
}

SpeechTranscription_nwc_yarp::~SpeechTranscription_nwc_yarp()
{
    closeMain();
}

bool SpeechTranscription_nwc_yarp::close()
{
    return closeMain();
}

bool SpeechTranscription_nwc_yarp::open(yarp::os::Searchable& config)
{
    m_local_name.clear();
    m_thrift_server_name.clear();

    if(!config.check("local"))
    {
        yCError(SPEECHTR_NWC) << "No local name specified";

        return false;
    }
    m_local_name = config.find("local").asString();

    if(!config.check("remote"))
    {
        yCError(SPEECHTR_NWC) << "No remote name specified";

        return false;
    }
    m_thrift_server_name = config.find("remote").asString();

    if(config.check("carrier"))
    {
        m_carrier_name = config.find("carrier").asString();
    }

    std::string thriftClientPortName = m_local_name + "/thrift:c";
    std::string thriftServerPortName = m_thrift_server_name + "/rpc";

    if(!m_thriftClientPort.open(thriftClientPortName))
    {
        yCError(SPEECHTR_NWC) << "open() failed: could not open" << thriftClientPortName <<"check the network";

        return false;
    }

    if(!yarp::os::Network::connect(thriftClientPortName, thriftServerPortName, m_carrier_name))
    {
        yCError(SPEECHTR_NWC) << "Network::connect() failed: could not connect" << thriftClientPortName << "with" << thriftServerPortName;

        return false;
    }

    if(!m_thriftClient.yarp().attachAsClient(m_thriftClientPort))
    {
        yCError(SPEECHTR_NWC) << "Error! Cannot attach the m_thriftClientPort (" << thriftClientPortName << ") port as a client";

        return false;
    }

    return true;
}

bool SpeechTranscription_nwc_yarp::closeMain()
{
    // Close thrift client port
    m_thriftClientPort.close();

    return true;
}

bool SpeechTranscription_nwc_yarp::setLanguage(const std::string& language)
{
    if(!m_thriftClient.set_language(language))
    {
        yCError(SPEECHTR_NWC) << "Error while setting language to" << language;
        return false;
    }

    return true;
}

bool SpeechTranscription_nwc_yarp::getLanguage(std::string& language)
{
    return_get_language result = m_thriftClient.get_language();
    if(!result.ret)
    {
        yCError(SPEECHTR_NWC) << "Error while retrieving language";
        return false;
    }

    language = result.language;

    return true;
}

bool SpeechTranscription_nwc_yarp::transcribe(const yarp::sig::Sound& sound, std::string& transcription, double& score)
{
    YARP_UNUSED(sound);
    YARP_UNUSED(transcription);
    YARP_UNUSED(score);

    return_transcribe result = m_thriftClient.transcribe(sound);

    if(!result.ret)
    {
        yCError(SPEECHTR_NWC) << "Error while transcribing the audio signal";
        return false;
    }

    transcription = result.transcription;
    score = result.score;

    return true;
}
