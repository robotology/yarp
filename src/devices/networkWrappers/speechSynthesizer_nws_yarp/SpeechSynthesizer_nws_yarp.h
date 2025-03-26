/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_SPEECHSYNTH_NWS_YARP_H
#define YARP_DEV_SPEECHSYNTH_NWS_YARP_H

#include <cstdio>
#include <cstdlib>
#include <memory>

#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ISpeechSynthesizer.h>
#include <yarp/sig/Sound.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/RpcServer.h>
#include <yarp/dev/WrapperSingle.h>
#include "ISpeechSynthesizerMsgs.h"
#include "SpeechSynthesizer_nws_yarp_ParamsParser.h"

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

// Callback implementation after buffered input.
class SpeechSynthesizer_CallbackHelper :
        public yarp::os::TypedReaderCallback<yarp::os::Bottle>
{
protected:
    yarp::dev::ISpeechSynthesizer* m_isptr {nullptr};
    yarp::os::Port* m_output_port {nullptr};

public:
    SpeechSynthesizer_CallbackHelper() = delete;
    SpeechSynthesizer_CallbackHelper(yarp::dev::ISpeechSynthesizer* x, yarp::os::Port* output_port);
    virtual ~SpeechSynthesizer_CallbackHelper() override {};

    using yarp::os::TypedReaderCallback<yarp::os::Bottle>::onRead;
    void onRead(yarp::os::Bottle& b) override;
};

// rpc commands
class ISpeechSynthesizerMsgsd : public ISpeechSynthesizerMsgs
{
private:
    std::mutex   m_mutex;
    yarp::dev::ISpeechSynthesizer* m_isptr{ nullptr };
    yarp::os::Port* m_output_port{ nullptr };

public:
    virtual return_set_language set_language(const std::string& language) override;
    virtual return_get_language get_language() override;
    virtual return_set_voice    set_voice(const std::string& language) override;
    virtual return_get_voice    get_voice() override;
    virtual return_set_speed    set_speed(double speed) override;
    virtual return_get_speed    get_speed() override;
    virtual return_set_pitch    set_pitch(double pitch) override;
    virtual return_get_pitch    get_pitch() override;
    virtual return_synthesize   synthesize(const std::string& text) override;

public:
    void setInterfaces(yarp::dev::ISpeechSynthesizer* iser) { m_isptr = iser;}
    void setOutputPort(yarp::os::Port* port) { m_output_port = port; }
    std::mutex* getMutex() { return &m_mutex; }
};


/**
 * @ingroup dev_impl_nws_yarp
 *
 * \brief `SpeechSynthesizer_nws_yarp`: A wrapper for a plugin able to perform speech synthesis.
 *
 * The network interface is composed by two ports.
 * When a text bottle is received on the input port, the attached subdevice processes it and generates
 * a yarp::sig::Sound on the output port.
 * The same functionality is available also via rpc port, which also provides additional functionalities,
 * such as setting of the voice or the language.
 *
 * Parameters required by this device are shown in class: SpeechSynthesizer_nws_yarp_ParamsParser
 */
class SpeechSynthesizer_nws_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::os::Thread,
        public yarp::dev::WrapperSingle,
        public yarp::os::PortReader,
        public SpeechSynthesizer_nws_yarp_ParamsParser
{
private:
    yarp::dev::ISpeechSynthesizer* m_isptr{ nullptr };
    yarp::os::Port            m_inputPort;
    yarp::os::Port            m_outputPort;
    yarp::os::Port            m_rpcPort;
    ISpeechSynthesizerMsgsd   m_rpc;

    yarp::os::PortReaderBuffer <yarp::os::Bottle> input_buffer;
    std::unique_ptr<SpeechSynthesizer_CallbackHelper> callback_impl;

    // yarp::dev::IWrapper
    bool  attach(yarp::dev::PolyDriver* deviceToAttach) override;
    bool  detach() override;

private:
    bool closeMain();

public:
    SpeechSynthesizer_nws_yarp() = default;
    SpeechSynthesizer_nws_yarp(const SpeechSynthesizer_nws_yarp&) = delete;
    SpeechSynthesizer_nws_yarp(SpeechSynthesizer_nws_yarp&&) = delete;
    SpeechSynthesizer_nws_yarp& operator=(const SpeechSynthesizer_nws_yarp&) = delete;
    SpeechSynthesizer_nws_yarp& operator=(SpeechSynthesizer_nws_yarp&&) = delete;
    virtual ~SpeechSynthesizer_nws_yarp() override;

    bool open(yarp::os::Searchable& config) override;
    bool close() override;
    bool read(yarp::os::ConnectionReader& connection) override;
    void run() override;
};

#endif // YARP_DEV_SPEECHSYNTH_NWS_YARP_H
