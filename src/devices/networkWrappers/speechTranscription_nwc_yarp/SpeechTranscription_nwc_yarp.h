/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_SPEECHTRANSCRIPTION_NWC_YARP_H
#define YARP_DEV_SPEECHTRANSCRIPTION_NWC_YARP_H

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
#include "SpeechTranscription_nwc_yarp_ParamsParser.h"

/**
 *  @ingroup dev_impl_nwc_yarp
 *
 * \section speechTranscription_nwc_yarp
 *
 * \brief `speechTranscription_nwc_yarp`: A network wrapper client that connects to a speechTranscription_nws_yarp to perform speech transcription.
 *
 * Parameters required by this device are shown in class: SpeechTranscription_nwc_yarp_ParamsParser
 */

class SpeechTranscription_nwc_yarp :
    public yarp::dev::DeviceDriver,
    public yarp::dev::ISpeechTranscription,
    public SpeechTranscription_nwc_yarp_ParamsParser
{
    // Methods
private:
    bool closeMain();

public:
    SpeechTranscription_nwc_yarp() = default;
    SpeechTranscription_nwc_yarp(const SpeechTranscription_nwc_yarp&) = delete;
    SpeechTranscription_nwc_yarp(SpeechTranscription_nwc_yarp&&) = delete;
    SpeechTranscription_nwc_yarp& operator=(const SpeechTranscription_nwc_yarp&) = delete;
    SpeechTranscription_nwc_yarp& operator=(SpeechTranscription_nwc_yarp&&) = delete;
    virtual ~SpeechTranscription_nwc_yarp() override;

    // yarp::dev::DeviceDriver
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //yarp::dev::ISpeechTranscription
    bool setLanguage(const std::string& language="auto") override;
    bool getLanguage(std::string& language) override;
    bool transcribe(const yarp::sig::Sound& sound, std::string& transcription, double& score) override;

    // Parameters
private:
    ISpeechTranscriptionMsgs m_thriftClient;

protected:
    yarp::os::Port                m_thriftClientPort;
};


#endif // YARP_DEV_SPEECHTRANSCRIPTION_NWC_YARP_H
