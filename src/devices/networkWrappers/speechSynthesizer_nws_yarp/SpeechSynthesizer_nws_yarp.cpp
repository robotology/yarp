/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "SpeechSynthesizer_nws_yarp.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/Os.h>
#include <memory>

namespace {
YARP_LOG_COMPONENT(SPEECHSYNTH_NWS, "yarp.devices.speechSynthesizer_nws_yarp")
}

SpeechSynthesizer_nws_yarp::~SpeechSynthesizer_nws_yarp()
{
    closeMain();
}

bool SpeechSynthesizer_nws_yarp::close()
{
    return closeMain();
}

bool SpeechSynthesizer_nws_yarp::open(Searchable& config)
{
    if (!parseParams(config)) { return false; }

    input_buffer.attach(m_inputPort);
    m_inputPort.open(m_name +"/text:i");
    m_outputPort.open(m_name + "/sound:o");
    if (!m_rpcPort.open(m_name +"/rpc"))
    {
        yCError(SPEECHSYNTH_NWS, "Failed to open rpc port");
        return false;
    }
    m_rpcPort.setReader(*this);

    yCInfo(SPEECHSYNTH_NWS, "Device waiting for attach...");
    return true;
}

void SpeechSynthesizer_nws_yarp::run()
{
    yCInfo(SPEECHSYNTH_NWS, "Starting");

    //double before, now;
    while (!isStopping())
    {
        yarp::os::SystemClock::delaySystem(0.100);
        //do nothing (for now..)
    }
    yCInfo(SPEECHSYNTH_NWS, "Stopping");
}

bool  SpeechSynthesizer_nws_yarp::attach(yarp::dev::PolyDriver* deviceToAttach)
{
    if (deviceToAttach->isValid())
    {
        deviceToAttach->view(m_isptr);
    }

    if (nullptr == m_isptr)
    {
        yCError(SPEECHSYNTH_NWS, "Subdevice passed to attach method is invalid");
        return false;
    }

    yCInfo(SPEECHSYNTH_NWS, "Attach done");

    callback_impl = std::make_unique<SpeechSynthesizer_CallbackHelper>(m_isptr,&m_outputPort);
    input_buffer.useCallback(*callback_impl);
    m_rpc.setInterfaces(m_isptr);
    m_rpc.setOutputPort(&m_outputPort);

    start();
    return true;
}

bool  SpeechSynthesizer_nws_yarp::detach()
{
    m_isptr = nullptr;
    return true;
}

bool SpeechSynthesizer_nws_yarp::closeMain()
{
    if (Thread::isRunning()) {
        Thread::stop();
    }
    //close the port connection here
    input_buffer.disableCallback();
    m_inputPort.close();
    m_outputPort.close();
    m_rpcPort.close();
    return true;
}

bool SpeechSynthesizer_nws_yarp::read(yarp::os::ConnectionReader& connection)
{
    if (!connection.isValid()) { return false;}

    bool b = m_rpc.read(connection);
    if (b)
    {
        return true;
    }
    else
    {
        yCError(SPEECHSYNTH_NWS, "read() Command failed");
        return false;
    }
}

//--------------------------------------------------
// RPC methods
return_set_language ISpeechSynthesizerMsgsd::set_language(const std::string& language)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return_set_language ret;
    if (m_isptr)
    {
        return m_isptr->setLanguage(language);
    }
    yCError(SPEECHSYNTH_NWS, "ISpeechSynthesizer interface was not set");
    ret.ret = ReturnValue::return_code::return_value_error_generic;
    return ret;
}

return_get_language ISpeechSynthesizerMsgsd::get_language()
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
    yCError(SPEECHSYNTH_NWS, "ISpeechSynthesizer interface was not set");
    ret.ret = ReturnValue::return_code::return_value_error_generic;
    return ret;
}

return_set_voice ISpeechSynthesizerMsgsd::set_voice(const std::string& voice)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return_set_voice ret;
    if (m_isptr)
    {
        return m_isptr->setVoice(voice);
    }
    yCError(SPEECHSYNTH_NWS, "ISpeechSynthesizer interface was not set");
    return ret;
}

return_get_voice ISpeechSynthesizerMsgsd::get_voice()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return_get_voice ret;
    if (m_isptr)
    {
        std::string voice;
        ReturnValue b = m_isptr->getVoice(voice);
        ret.ret = b;
        ret.voice = voice;
        return ret;
    }
    yCError(SPEECHSYNTH_NWS, "ISpeechSynthesizer interface was not set");
    ret.ret = ReturnValue::return_code::return_value_error_generic;
    return ret;
}

return_set_pitch ISpeechSynthesizerMsgsd::set_pitch(double pitch)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return_set_pitch ret;
    if (m_isptr)
    {
        return m_isptr->setPitch(pitch);
    }
    yCError(SPEECHSYNTH_NWS, "ISpeechSynthesizer interface was not set");
    ret.ret = ReturnValue::return_code::return_value_error_generic;
    return ret;
}

return_get_pitch ISpeechSynthesizerMsgsd::get_pitch()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return_get_pitch ret;
    if (m_isptr)
    {
        double pitch;
        ReturnValue b = m_isptr->getPitch(pitch);
        ret.ret = b;
        ret.pitch = pitch;
        return ret;
    }
    yCError(SPEECHSYNTH_NWS, "ISpeechSynthesizer interface was not set");
    return ret;
}

return_set_speed ISpeechSynthesizerMsgsd::set_speed(double speed)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return_set_speed ret;
    if (m_isptr)
    {
        return m_isptr->setSpeed(speed);
    }
    yCError(SPEECHSYNTH_NWS, "ISpeechSynthesizer interface was not set");
    ret.ret = ReturnValue::return_code::return_value_error_generic;
    return ret;
}

return_get_speed ISpeechSynthesizerMsgsd::get_speed()
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return_get_speed ret;
    if (m_isptr)
    {
        double speed;
        ReturnValue b = m_isptr->getSpeed(speed);
        ret.ret = b;
        ret.speed = speed;
        return ret;
    }
    yCError(SPEECHSYNTH_NWS, "ISpeechSynthesizer interface was not set");
    ret.ret = ReturnValue::return_code::return_value_error_generic;
    return ret;
}

return_synthesize ISpeechSynthesizerMsgsd::synthesize(const std::string& text)
{
    std::lock_guard <std::mutex> lg(m_mutex);
    return_synthesize ret;
    if (m_isptr)
    {
        yarp::sig::Sound snd;
        double score;
        ReturnValue b = m_isptr->synthesize(text, snd);
        ret.ret = b;
        ret.sound = snd;

        if (m_output_port && m_output_port->getOutputCount()>0)
        {
            m_output_port->write(snd);
        }

        return ret;
    }
    yCError(SPEECHSYNTH_NWS, "ISpeechSynthesizer interface was not set");
    ret.ret = ReturnValue::return_code::return_value_error_generic;
    return ret;
}

//--------------------------------------------------
// ImplementCallbackHelper class.
SpeechSynthesizer_CallbackHelper::SpeechSynthesizer_CallbackHelper(yarp::dev::ISpeechSynthesizer*x, yarp::os::Port* p)
{
    if (x ==nullptr || p==nullptr)
    {
        yCError(SPEECHSYNTH_NWS, "Could not get ISpeechSynthesizer interface/output port");
        std::exit(1);
    }
    m_isptr = x;
    m_output_port = p;
}

void SpeechSynthesizer_CallbackHelper::onRead(yarp::os::Bottle &b)
{
    if (m_isptr)
    {
        yarp::sig::Sound snd;
        bool ok = m_isptr->synthesize(b.get(0).asString(), snd);
        if (!ok)
        {
            yCError(SPEECHSYNTH_NWS, "Problems during speech synthesis");
        }
        else
        {
            if (m_output_port && m_output_port->getOutputCount() > 0)
            {
                m_output_port->write(snd);
            }
        }
    }
    else
    {
        yCError(SPEECHSYNTH_NWS, "ISpeechSynthesizer interface was not set");
    }
}
