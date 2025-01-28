/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "SpeechTranscription_nws_yarp.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/Os.h>

namespace {
YARP_LOG_COMPONENT(SPEECHTR_NWS, "yarp.devices.speechTranscription_nws_yarp")
}

SpeechTranscription_nws_yarp::~SpeechTranscription_nws_yarp()
{
    closeMain();
}

bool SpeechTranscription_nws_yarp::close()
{
    return closeMain();
}

bool SpeechTranscription_nws_yarp::open(Searchable& config)
{
    if (!parseParams(config)) { return false; }

    input_buffer.attach(m_inputPort);
    m_inputPort.open(m_name +"/sound:i");
    m_outputPort.open(m_name + "/text:o");
    if (!m_rpcPort.open(m_name +"/rpc"))
    {
        yCError(SPEECHTR_NWS, "Failed to open rpc port");
        return false;
    }
    m_rpcPort.setReader(*this);

    yCInfo(SPEECHTR_NWS, "Device waiting for attach...");
    return true;
}

void SpeechTranscription_nws_yarp::run()
{
    yCInfo(SPEECHTR_NWS, "Starting");

    //double before, now;
    while (!isStopping())
    {
        yarp::os::SystemClock::delaySystem(0.100);
        //do nothing (for now..)
    }
    yCInfo(SPEECHTR_NWS, "Stopping");
}

bool  SpeechTranscription_nws_yarp::attach(yarp::dev::PolyDriver* deviceToAttach)
{
    if (deviceToAttach->isValid())
    {
        deviceToAttach->view(m_isptr);
    }

    if (nullptr == m_isptr)
    {
        yCError(SPEECHTR_NWS, "Subdevice passed to attach method is invalid");
        return false;
    }

    yCInfo(SPEECHTR_NWS, "Attach done");

    callback_impl = new ImplementCallbackHelper2(m_isptr,&m_outputPort);
    input_buffer.useCallback(*callback_impl);
    m_rpc.setInterfaces(m_isptr);
    m_rpc.setOutputPort(&m_outputPort);

    start();
    return true;
}

bool  SpeechTranscription_nws_yarp::detach()
{
    m_isptr = nullptr;
    return true;
}

bool SpeechTranscription_nws_yarp::closeMain()
{
    if (Thread::isRunning()) {
        Thread::stop();
    }
    //close the port connection here
    input_buffer.disableCallback();
    m_inputPort.close();
    m_outputPort.close();
    m_rpcPort.close();
    if (callback_impl) {delete callback_impl; callback_impl=nullptr;}
    return true;
}

bool SpeechTranscription_nws_yarp::read(yarp::os::ConnectionReader& connection)
{
    if (!connection.isValid()) { return false;}

    bool b = m_rpc.read(connection);
    if (b)
    {
        return true;
    }
    else
    {
        yCError(SPEECHTR_NWS, "read() Command failed");
        return false;
    }
}

//--------------------------------------------------
// RPC methods
return_set_language ISpeechTranscriptionMsgsd::set_language(const std::string& language)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return_set_language ret;
    if (m_isptr)
    {
        return m_isptr->setLanguage(language);
    }
    yCError(SPEECHTR_NWS, "ISpeechTranscription interface was not set");
    ret.ret = ReturnValue::return_code::return_value_error_generic;
    return ret;
}

return_get_language ISpeechTranscriptionMsgsd::get_language()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return_get_language ret;
    if (m_isptr)
    {
        std::string language;
        ReturnValue b =  m_isptr->getLanguage(language);
        ret.ret = b;
        ret.language = language;
        return ret;
    }
    yCError(SPEECHTR_NWS, "ISpeechTranscription interface was not set");
    ret.ret = ReturnValue::return_code::return_value_error_generic;
    return ret;
}

return_transcribe ISpeechTranscriptionMsgsd::transcribe(const yarp::sig::Sound& sound)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return_transcribe ret;
    if (m_isptr)
    {
        std::string transcription;
        double score;
        ReturnValue b = m_isptr->transcribe(sound, transcription, score);
        ret.ret = b;
        ret.transcription = transcription;
        ret.score = score;

        if (m_output_port && m_output_port->getOutputCount()>0)
        {
            yarp::os::Bottle bot;
            bot.addString(transcription);
            bot.addFloat64(score);
            m_output_port->write(bot);
        }

        return ret;
    }
    yCError(SPEECHTR_NWS, "ISpeechTranscription interface was not set");
    ret.ret = ReturnValue::return_code::return_value_error_generic;
    return ret;
}

//--------------------------------------------------
// ImplementCallbackHelper class.
ImplementCallbackHelper2::ImplementCallbackHelper2(yarp::dev::ISpeechTranscription*x, yarp::os::Port* p)
{
    if (x ==nullptr || p==nullptr)
    {
        yCError(SPEECHTR_NWS, "Could not get ISpeechTranscription interface/output port");
        std::exit(1);
    }
    m_isptr = x;
    m_output_port = p;
}

void ImplementCallbackHelper2::onRead(yarp::sig::Sound &b)
{
    if (m_isptr)
    {
        std::string transcription;
        double score=0;
        bool ok = m_isptr->transcribe(b, transcription, score);
        if (!ok)
        {
            yCError(SPEECHTR_NWS, "Problems during transcription");
        }
        else
        {
            if (m_output_port && m_output_port->getOutputCount() > 0)
            {
                yarp::os::Bottle b;
                b.addString(transcription);
                b.addFloat64(score);
                m_output_port->write(b);
            }
        }
    }
    else
    {
        yCError(SPEECHTR_NWS, "ISpeechTranscription interface was not set");
    }
}
