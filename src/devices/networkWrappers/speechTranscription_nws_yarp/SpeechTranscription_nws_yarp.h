/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_SPEECHTRANSCRIPTION_NWS_YARP_H
#define YARP_DEV_SPEECHTRANSCRIPTION_NWS_YARP_H

#include <cstdio>
#include <cstdlib>

#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ISpeechTranscription.h>
#include <yarp/sig/Sound.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/RpcServer.h>
#include <yarp/dev/WrapperSingle.h>
#include "ISpeechTranscriptionMsgs.h"
#include "SpeechTranscription_nws_yarp_ParamsParser.h"

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

class SerialPort_nws_yarp;

// Callback implementation after buffered input.
class ImplementCallbackHelper2 :
        public yarp::os::TypedReaderCallback<yarp::sig::Sound>
{
protected:
    yarp::dev::ISpeechTranscription* m_isptr{nullptr};
    yarp::os::Port* m_output_port{ nullptr };

public:
    ImplementCallbackHelper2() = delete;
    ImplementCallbackHelper2(yarp::dev::ISpeechTranscription* x, yarp::os::Port* output_port);
    virtual ~ImplementCallbackHelper2() override {};

    using yarp::os::TypedReaderCallback<yarp::sig::Sound>::onRead;
    void onRead(yarp::sig::Sound& b) override;
};

// rpc commands
class ISpeechTranscriptionMsgsd : public ISpeechTranscriptionMsgs
{
private:
    std::mutex   m_mutex;
    yarp::dev::ISpeechTranscription* m_isptr{ nullptr };
    yarp::os::Port* m_output_port{ nullptr };

public:
    virtual return_set_language set_language(const std::string& language) override;
    virtual return_get_language get_language() override;
    virtual return_transcribe   transcribe(const yarp::sig::Sound& sound) override;

public:
    void setInterfaces(yarp::dev::ISpeechTranscription* iser) { m_isptr = iser;}
    void setOutputPort(yarp::os::Port* port) { m_output_port = port; }
    std::mutex* getMutex() { return &m_mutex; }
};

/**
 * @ingroup dev_impl_nws_yarp
 *
 * \brief `SpeechTranscription_nws_yarp`: A wrapper for a plugin able to perform speech transcription.
 *
 * The network interface is composed by two ports.
 * When a yarp::sig::Sound is received on the input port, the attached subdevice processes it and sends
 * the transcription on the output port.
 * The same functionality is available also via rpc port, which also provides additional functionalities,
 * such as setting of the transcription language.
 *
 * Parameters required by this device are shown in class: SpeechTranscription_nws_yarp_ParamsParser
 *
 */
class SpeechTranscription_nws_yarp :
        public yarp::dev::DeviceDriver,
        public yarp::os::Thread,
        public yarp::dev::WrapperSingle,
        public yarp::os::PortReader,
        public SpeechTranscription_nws_yarp_ParamsParser
{
private:
    yarp::dev::ISpeechTranscription* m_isptr{ nullptr };
    yarp::os::Port            m_inputPort;
    yarp::os::Port            m_outputPort;
    yarp::os::Port            m_rpcPort;
    ISpeechTranscriptionMsgsd m_rpc;

    yarp::os::PortReaderBuffer <yarp::sig::Sound> input_buffer;
    ImplementCallbackHelper2* callback_impl{ nullptr };

    // yarp::dev::IWrapper
    bool  attach(yarp::dev::PolyDriver* deviceToAttach) override;
    bool  detach() override;

private:
    bool closeMain();

public:
    SpeechTranscription_nws_yarp() = default;
    SpeechTranscription_nws_yarp(const SpeechTranscription_nws_yarp&) = delete;
    SpeechTranscription_nws_yarp(SpeechTranscription_nws_yarp&&) = delete;
    SpeechTranscription_nws_yarp& operator=(const SpeechTranscription_nws_yarp&) = delete;
    SpeechTranscription_nws_yarp& operator=(SpeechTranscription_nws_yarp&&) = delete;
    virtual ~SpeechTranscription_nws_yarp() override;

    bool open(yarp::os::Searchable& config) override;
    bool close() override;
    bool read(yarp::os::ConnectionReader& connection) override;
    void run() override;
};

#endif // YARP_DEV_SERIALPORT_NWS_YARP_H
